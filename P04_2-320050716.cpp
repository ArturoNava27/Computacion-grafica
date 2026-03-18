/*Práctica 4: Modelado Jerárquico.
Se implementa el uso de matrices adicionales para almacenar información de transformaciones geométricas que se quiere
heredar entre diversas instancias para que estén unidas
Teclas de la F a la K para rotaciones de articulaciones
*/
#include <stdio.h>
#include <string.h>
#include<cmath>
#include<vector>
#include <glew.h>
#include <glfw3.h>
//glm
#include<glm.hpp>
#include<gtc\matrix_transform.hpp>
#include<gtc\type_ptr.hpp>
#include <gtc\random.hpp>
//clases para dar orden y limpieza al còdigo
#include"Mesh.h"
#include"Shader.h"
#include"Sphere.h"
#include"Window.h"
#include"Camera.h"
//tecla E: Rotar sobre el eje X
//tecla R: Rotar sobre el eje Y
//tecla T: Rotar sobre el eje Z
using std::vector;
//Dimensiones de la ventana
const float toRadians = 3.14159265f / 180.0; //grados a radianes
const float PI = 3.14159265f;
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;
Camera camera;
Window mainWindow;
vector<Mesh*> meshList;
vector<Shader>shaderList;
//Vertex Shader
static const char* vShader = "shaders/shader.vert";
static const char* fShader = "shaders/shader.frag";
Sphere sp = Sphere(1.0, 20, 20); //recibe radio, slices, stacks
float rotLlanta1 = 0.0f, rotLlanta2 = 0.0f, rotLlanta3 = 0.0f, rotLlanta4 = 0.0f;
float velocidadRot = 5.0f;

void CrearCubo()
{
	unsigned int cubo_indices[] = {
		// front
		0, 1, 2,
		2, 3, 0,
		// right
		1, 5, 6,
		6, 2, 1,
		// back
		7, 6, 5,
		5, 4, 7,
		// left
		4, 0, 3,
		3, 7, 4,
		// bottom
		4, 5, 1,
		1, 0, 4,
		// top
		3, 2, 6,
		6, 7, 3
	};

	GLfloat cubo_vertices[] = {
		// front
		-0.5f, -0.5f,  0.5f,
		0.5f, -0.5f,  0.5f,
		0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,
		// back
		-0.5f, -0.5f, -0.5f,
		0.5f, -0.5f, -0.5f,
		0.5f,  0.5f, -0.5f,
		-0.5f,  0.5f, -0.5f
	};
	Mesh* cubo = new Mesh();
	cubo->CreateMesh(cubo_vertices, cubo_indices, 24, 36);
	meshList.push_back(cubo);
}

// Pirámide triangular regular
void CrearPiramideTriangular()
{
	unsigned int indices_piramide_triangular[] = {
			0,1,2,
			1,3,2,
			3,0,2,
			1,0,3

	};
	GLfloat vertices_piramide_triangular[] = {
		-0.5f, -0.5f,0.0f,	//0
		0.5f,-0.5f,0.0f,	//1
		0.0f,0.5f, -0.25f,	//2
		0.0f,-0.5f,-0.5f,	//3

	};
	Mesh* obj1 = new Mesh();
	obj1->CreateMesh(vertices_piramide_triangular, indices_piramide_triangular, 12, 12);
	meshList.push_back(obj1);

}
/*
Crear cilindro y cono con arreglos dinámicos vector creados en el Semestre 2023 - 1 : por Sánchez Pérez Omar Alejandro
*/
void CrearCilindro(int res, float R) {
	float h = 1.0f;
	float semiH = h / 2.0f;
	vector<GLfloat> vertices;
	vector<unsigned int> indices;

	// 1. GENERACIÓN DE VÉRTICES

	for (int i = 0; i <= res; i++) {
		float theta = (2.0f * PI * i) / (float)res;
		float x = R * cos(theta);
		float z = R * sin(theta);

		// Vértices para pared (superior i*2, inferior i*2 + 1)
		vertices.push_back(x); vertices.push_back(semiH);  vertices.push_back(z);
		vertices.push_back(x); vertices.push_back(-semiH); vertices.push_back(z);
	}

	// 2. ÍNDICES DE LAS PAREDES
	for (int i = 0; i < res; i++) {
		unsigned int top1 = i * 2;
		unsigned int bottom1 = i * 2 + 1;
		unsigned int top2 = (i + 1) * 2;
		unsigned int bottom2 = (i + 1) * 2 + 1;

		indices.push_back(top1); indices.push_back(bottom1); indices.push_back(top2);
		indices.push_back(bottom1); indices.push_back(bottom2); indices.push_back(top2);
	}

	// 3. TAPA SUPERIOR
	unsigned int centerTopIndex = vertices.size() / 3;
	vertices.push_back(0.0f); vertices.push_back(semiH); vertices.push_back(0.0f); // Centro arriba

	for (int i = 0; i < res; i++) {
		indices.push_back(centerTopIndex);
		indices.push_back((i + 1) * 2);
		indices.push_back(i * 2);
	}

	// 4. TAPA INFERIOR 
	unsigned int centerBottomIndex = vertices.size() / 3;
	vertices.push_back(0.0f); vertices.push_back(-semiH); vertices.push_back(0.0f); // Centro abajo

	for (int i = 0; i < res; i++) {
		indices.push_back(centerBottomIndex);
		indices.push_back(i * 2 + 1);
		indices.push_back((i + 1) * 2 + 1);
	}

	Mesh* cilindro = new Mesh();
	cilindro->CreateMeshGeometry(vertices, indices, vertices.size(), indices.size());
	meshList.push_back(cilindro);
}

//función para crear un cono
void CrearCono(int res, float R) {

	//constantes utilizadas en los ciclos for
	int n, i;
	//cálculo del paso interno en la circunferencia y variables que almacenarán cada coordenada de cada vértice
	GLfloat dt = 2 * PI / res, x, z, y = -0.5f;

	vector<GLfloat> vertices;
	vector<unsigned int> indices;

	//caso inicial para crear el cono
	vertices.push_back(0.0);
	vertices.push_back(0.5);
	vertices.push_back(0.0);

	//ciclo for para crear los vértices de la circunferencia del cono
	for (n = 0; n <= (res); n++) {
		x = R * cos((n)*dt);
		z = R * sin((n)*dt);
		for (i = 0; i < 3; i++) {
			switch (i) {
			case 0:
				vertices.push_back(x);
				break;
			case 1:
				vertices.push_back(y);
				break;
			case 2:
				vertices.push_back(z);
				break;
			}
		}
	}
	vertices.push_back(R * cos(0) * dt);
	vertices.push_back(-0.5);
	vertices.push_back(R * sin(0) * dt);


	for (i = 0; i < res + 2; i++) indices.push_back(i);

	//se genera el mesh del cono
	Mesh* cono = new Mesh();
	cono->CreateMeshGeometry(vertices, indices, vertices.size(), res + 2);
	meshList.push_back(cono);
}

//función para crear pirámide cuadrangular unitaria
void CrearPiramideCuadrangular()
{
	vector<unsigned int> piramidecuadrangular_indices = {
		0,3,4,
		3,2,4,
		2,1,4,
		1,0,4,
		0,1,2,
		0,2,4

	};
	vector<GLfloat> piramidecuadrangular_vertices = {
		0.5f,-0.5f,0.5f,
		0.5f,-0.5f,-0.5f,
		-0.5f,-0.5f,-0.5f,
		-0.5f,-0.5f,0.5f,
		0.0f,0.5f,0.0f,
	};
	Mesh* piramide = new Mesh();
	piramide->CreateMeshGeometry(piramidecuadrangular_vertices, piramidecuadrangular_indices, 15, 18);
	meshList.push_back(piramide);
}



void CreateShaders()
{
	Shader* shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);

}


int main()
{
	mainWindow = Window(800, 600);
	mainWindow.Initialise();

	CrearCubo();//índice 0 en MeshList
	CrearPiramideTriangular();//índice 1 en MeshList
	CrearCilindro(30, 1.0f);//índice 2 en MeshList
	CrearCono(25, 2.0f);//índice 3 en MeshList
	CrearPiramideCuadrangular();//índice 4 en MeshList
	CreateShaders();



	/*Cámara se usa el comando: glm::lookAt(vector de posición, vector de orientación, vector up));
	En la clase Camera se reciben 5 datos:
	glm::vec3 vector de posición,
	glm::vec3 vector up,
	GlFloat yaw rotación para girar hacia la derecha e izquierda
	GlFloat pitch rotación para inclinar hacia arriba y abajo
	GlFloat velocidad de desplazamiento,
	GlFloat velocidad de vuelta o de giro
	Se usa el Mouse y las teclas WASD y su posición inicial está en 0,0,1 y ve hacia 0,0,-1.
	*/
	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 0.2f, 0.2f);
	GLuint uniformProjection = 0;
	GLuint uniformModel = 0;
	GLuint uniformView = 0;
	GLuint uniformColor = 0;
	glm::mat4 projection = glm::perspective(glm::radians(60.0f), mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 100.0f);
	

	//Loop mientras no se cierra la ventana
	sp.init(); //inicializar esfera
	sp.load();//enviar la esfera al shader

	glm::mat4 model(1.0);
	glm::mat4 modelaux(1.0);
	glm::mat4 modelaux_brazo(1.0);
	glm::mat4 modelaux_pecho(1.0);
	glm::mat4 modelaux_cabeza(1.0);
	glm::mat4 modelaux_columna(1.0);
	glm::mat4 modelaux_garra(1.0);

	glm::vec3 color = glm::vec3(0.0f, 0.0f, 0.0f);

	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;
		glfwPollEvents();
		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		shaderList[0].useShader();
		uniformModel = shaderList[0].getModelLocation();
		uniformProjection = shaderList[0].getProjectLocation();
		uniformView = shaderList[0].getViewLocation();
		uniformColor = shaderList[0].getColorLocation();

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));

		// Colores definidos
		glm::vec3 verdeOscuro = glm::vec3(0.0f, 0.2f, 0.05f);
		glm::vec3 verdeClaro = glm::vec3(0.2f, 0.6f, 0.2f);
		glm::vec3 amarilloGarras = glm::vec3(1.0f, 0.9f, 0.0f);
		glm::vec3 blanco = glm::vec3(1.0f, 1.0f, 1.0f);

		// ==============================================================================
		// 1. TORSO ALINEADO - TECLA F 
		// ==============================================================================
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, -1.0f, -10.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion1()), glm::vec3(0.0f, 1.0f, 0.0f));
		modelaux = model;

		// --- 1a. CINTURA / CADERA ---
		model = glm::scale(model, glm::vec3(2.2f, 0.8f, 1.6f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(verdeOscuro));
		meshList[0]->RenderMesh();

		// --- 1b. TORSO CILÍNDRICO ---
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, 1.1f, 0.0f));
		modelaux_columna = model;
		model = glm::scale(model, glm::vec3(1.2f, 1.4f, 1.2f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(verdeClaro));
		meshList[2]->RenderMesh();

		// ==============================================================================
		// 2. CABEZA OVALADA Y OJOS
		// ==============================================================================
		model = modelaux_columna;
		model = glm::translate(model, glm::vec3(0.0f, 1.0f, 0.5f));
		modelaux_cabeza = model;

		// Cabeza ovalada
		model = glm::scale(model, glm::vec3(1.1f, 1.0f, 1.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(verdeOscuro));
		sp.render();

		// OJOS
		glm::vec3 posOjos[] = { glm::vec3(0.3f, 0.4f, 1.2f), glm::vec3(-0.3f, 0.4f, 1.2f) };

		for (int i = 0; i < 2; i++) {
			model = modelaux_cabeza;
			model = glm::translate(model, posOjos[i]);
			model = glm::scale(model, glm::vec3(0.15f, 0.15f, 0.15f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			glUniform3fv(uniformColor, 1, glm::value_ptr(blanco));
			sp.render();
		}

		// ==============================================================================
		// 3. BRAZOS - HOMBRO (J) Y CODO/MANOS (K)
		// ==============================================================================
		glm::vec3 posBrazos[] = { glm::vec3(0.8f, 0.2f, 0.6f), glm::vec3(-0.8f, 0.2f, 0.6f) };

		for (int i = 0; i < 2; i++) {
			model = modelaux_columna;
			model = glm::translate(model, posBrazos[i]);
			model = glm::rotate(model, glm::radians(mainWindow.getarticulacion4()), glm::vec3(1.0f, 0.0f, 0.0f));
			modelaux_brazo = model;

			// Húmero
			glm::mat4 model_humero = glm::translate(model, glm::vec3(0.0f, -0.25f, 0.0f));
			model_humero = glm::scale(model_humero, glm::vec3(0.3f, 0.5f, 0.3f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model_humero));
			glUniform3fv(uniformColor, 1, glm::value_ptr(verdeOscuro));
			meshList[2]->RenderMesh();

			// Antebrazo y MANOS (Controlados por Tecla K)
			model = modelaux_brazo;
			model = glm::translate(model, glm::vec3(0.0f, -0.5f, 0.0f));
			model = glm::rotate(model, glm::radians(mainWindow.getarticulacion5()), glm::vec3(1.0f, 0.0f, 0.0f));
			modelaux_garra = model;

			glm::mat4 model_antebrazo = glm::translate(model, glm::vec3(0.0f, -0.2f, 0.1f));
			model_antebrazo = glm::scale(model_antebrazo, glm::vec3(0.25f, 0.4f, 0.25f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model_antebrazo));
			meshList[2]->RenderMesh();

			// Garras
			glm::vec3 offsetG[] = { glm::vec3(0.1f, -0.4f, 0.2f), glm::vec3(0.0f, -0.4f, 0.35f), glm::vec3(-0.1f, -0.4f, 0.2f) };
			for (int g = 0; g < 3; g++) {
				model = modelaux_garra;
				model = glm::translate(model, offsetG[g]);
				model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
				model = glm::scale(model, glm::vec3(0.1f, 0.4f, 0.1f));
				glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
				glUniform3fv(uniformColor, 1, glm::value_ptr(amarilloGarras));
				meshList[1]->RenderMesh();
			}
		}

		// ==============================================================================
		// 4. PIERNAS - MUSLO (G) Y PANTORRILLA (H)
		// ==============================================================================
		glm::vec3 posPiernas[] = { glm::vec3(0.6f, -0.4f, 0.0f), glm::vec3(-0.6f, -0.4f, 0.0f) };

		for (int i = 0; i < 2; i++) {
			model = modelaux;
			model = glm::translate(model, posPiernas[i]);

			// Muslo (Gira adelante/atrás con G)
			model = glm::rotate(model, glm::radians(mainWindow.getarticulacion2()), glm::vec3(1.0f, 0.0f, 0.0f));
			modelaux_brazo = model; // Pivote Muslo

			glm::mat4 model_muslo = glm::translate(model, glm::vec3(0.0f, -0.5f, 0.0f));
			model_muslo = glm::scale(model_muslo, glm::vec3(0.6f, 1.0f, 0.6f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model_muslo));
			glUniform3fv(uniformColor, 1, glm::value_ptr(verdeOscuro));
			meshList[2]->RenderMesh();

			// Pantorrilla (Gira adelante/atrás con H)
			model = modelaux_brazo;
			model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0f));
			model = glm::rotate(model, glm::radians(mainWindow.getarticulacion3()), glm::vec3(1.0f, 0.0f, 0.0f));
			modelaux_garra = model; // Pivote para pies

			glm::mat4 model_panto = glm::translate(model, glm::vec3(0.0f, -0.5f, 0.0f));
			model_panto = glm::scale(model_panto, glm::vec3(0.5f, 1.0f, 0.5f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model_panto));
			meshList[2]->RenderMesh();

			// Garras del pie
			glm::vec3 offsetDedos[] = { glm::vec3(0.2f, -1.0f, 0.2f), glm::vec3(0.0f, -1.0f, 0.4f), glm::vec3(-0.2f, -1.0f, 0.2f) };
			for (int d = 0; d < 3; d++) {
				model = modelaux_garra;
				model = glm::translate(model, offsetDedos[d]);
				model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
				model = glm::scale(model, glm::vec3(0.2f, 0.5f, 0.2f));
				glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
				glUniform3fv(uniformColor, 1, glm::value_ptr(amarilloGarras));
				meshList[1]->RenderMesh();
			}
		}

		// ==============================================================================
		// 5. COLA ARTICULADA (L ) - MOVIMIENTO(Arriba/Abajo y Lados)
		// ==============================================================================
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, -0.8f));

		// Rotación Horizontal (Izquierda/Derecha) 
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion6()), glm::vec3(0.0f, 1.0f, 0.0f));

		// FUNCION ADICIONAL: Rotación Vertical (Arriba/Abajo) 
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion6() * 0.5f), glm::vec3(1.0f, 0.0f, 0.0f));

		for (int s = 0; s < 3; s++) {
			model = glm::rotate(model, glm::radians(20.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			model = glm::translate(model, glm::vec3(0.0f, 0.0f, -0.5f));

			glm::mat4 model_segmento = model;
			float factorEscala = 1.0f - (s * 0.2f);
			model_segmento = glm::scale(model_segmento, glm::vec3(0.5f * factorEscala, 0.3f * factorEscala, 0.8f));

			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model_segmento));
			glUniform3fv(uniformColor, 1, glm::value_ptr(verdeOscuro));
			meshList[0]->RenderMesh();
		}

		glUseProgram(0);
		mainWindow.swapBuffers();
	}
	return 0;
}
