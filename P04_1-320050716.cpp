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
		float theta = (2.0f * PI * i)/(float) res ;
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
	//Cilindro y cono reciben resolución (slices, rebanadas) y Radio de circunferencia de la base y tapa

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
	//glm::mat4 projection = glm::ortho(-1, 1, -1, 1, 1, 10);

	//Loop mientras no se cierra la ventana
	sp.init(); //inicializar esfera
	sp.load();//enviar la esfera al shader

	glm::mat4 model(1.0);
	glm::mat4 modelaux(1.0);

	glm::vec3 color = glm::vec3(0.0f, 0.0f, 0.0f); 

	while (!mainWindow.getShouldClose())
	{

		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;
		//Recibir eventos del usuario
		glfwPollEvents();
		//Cámara
		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

		//Limpiar la ventana
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //Se agrega limpiar el buffer de profundidad
		shaderList[0].useShader();
		uniformModel = shaderList[0].getModelLocation();
		uniformProjection = shaderList[0].getProjectLocation();
		uniformView = shaderList[0].getViewLocation();
		uniformColor = shaderList[0].getColorLocation();

		// ==============================================================================
		// 1. LA CABINA (BASE/PADRE)
		// ==============================================================================
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 6.0f, -4.0f));
		modelaux = model;

		// --- DIBUJO DEL CUBO (Cuerpo de la cabina) ---
		glm::mat4 modelCuerpo = model;
		modelCuerpo = glm::scale(modelCuerpo, glm::vec3(8.0f, 4.0f, 4.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelCuerpo));
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		color = glm::vec3(1.0f, 0.0f, 1.0f); // Rosa
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[0]->RenderMesh();

		
		modelCuerpo = model;
		modelCuerpo = glm::translate(modelCuerpo, glm::vec3(-3.8f, 0.5f, 0.0f));
		modelCuerpo = glm::scale(modelCuerpo, glm::vec3(0.8f, 1.5f, 1.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelCuerpo));
		color = glm::vec3(0.0f, 1.0f, 1.0f); // Cian
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		sp.render();

		// ==============================================================================
		// 1b. LA BASE (PIRÁMIDE CUADRANGULAR)
		// ==============================================================================
		model = modelaux; 
		model = glm::translate(model, glm::vec3(0.0f, -3.0f, 0.0f)); 
		glm::mat4 modelBaseLlantasRaw = model; 

		// --- DIBUJO DE LA PIRÁMIDE (Gris) ---
		glm::mat4 modelPiramideDibujo = glm::scale(model, glm::vec3(10.0f, 2.0f, 10.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelPiramideDibujo));
		color = glm::vec3(0.3f, 0.3f, 0.3f); 
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[4]->RenderMesh(); // Pirámide

		// ==============================================================================
		// 1c. LAS 4 LLANTAS (CORREGIDAS PARA APUNTAR HACIA EL EJE Z)
		// ==============================================================================
		color = glm::vec3(0.1f, 0.1f, 0.1f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));

		float pXZ = 5.0f;
		float pY = -1.0f;
		float coordsX[] = { pXZ, -pXZ, pXZ, -pXZ };
		float coordsZ[] = { pXZ, pXZ, -pXZ, -pXZ };

		float angulos[] = {
			mainWindow.getrotLlanta1(),
			mainWindow.getrotLlanta2(),
			mainWindow.getrotLlanta3(),
			mainWindow.getrotLlanta4()
		};

			for (int i = 0; i < 4; i++) {
				model = modelBaseLlantasRaw;
				model = glm::translate(model, glm::vec3(coordsX[i], pY, coordsZ[i]));

				// 1. GIRO DE ACELERACIÓN: Usamos el eje Y porque es el eje natural del cilindro (sus tapas).
				// Al girar aquí, la llanta "rueda" sobre su circunferencia.
				model = glm::rotate(model, glm::radians(angulos[i]), glm::vec3(0.0f, 0.0f, 1.0f));

				// 2. ORIENTACIÓN: Rotamos 90 grados en X para "acostar" el cilindro hacia los lados.
				// Esto hace que el frente de rodado coincida con el frente de la cabina (Eje Z).
				model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));

				model = glm::scale(model, glm::vec3(2.5f, 1.2f, 2.5f));

				glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
				meshList[2]->RenderMesh();
			}

		// ==============================================================================
		// 2. ARTICULACIÓN 1 Y PRIMER BRAZO
		// ==============================================================================
		model = modelaux;
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion1()), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::rotate(model, glm::radians(135.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::translate(model, glm::vec3(2.5f, 0.0f, 0.0f));
		modelaux = model;

		// --- DIBUJO DEL BRAZO ---
		model = glm::scale(model, glm::vec3(5.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		// CAMBIO DE COLOR (Para diferenciar):
		color = glm::vec3(1.0f, 0.0f, 0.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));

		meshList[0]->RenderMesh();

		// 3. ARTICULACIÓN 2 (ESFERA AMARILLA)
		model = modelaux;
		model = glm::translate(model, glm::vec3(2.5f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion2()), glm::vec3(0.0f, 0.0f, 1.0f));
		modelaux = model;
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 1.0f, 0.0f); // Amarillo
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		sp.render();

		// 4. SEGUNDO BRAZO (VERDE)
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, -2.5f, 0.0f));
		modelaux = model;
		model = glm::scale(model, glm::vec3(1.0f, 5.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.0f, 1.0f, 0.0f); // Verde
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[0]->RenderMesh();

		// 5. ARTICULACIÓN 3 (UNE VERDE CON AZUL)
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, -2.5f, 0.0f)); 
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion3()), glm::vec3(0.0f, 0.0f, 1.0f));
		modelaux = model; 
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.0f, 0.0f, 1.0f); // Azul
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		sp.render();

		// 6. TERCER BRAZO (CIAN / AZUL CLARO)
		model = modelaux;
		model = glm::rotate(model, glm::radians(135.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::translate(model, glm::vec3(0.0f, -2.5f, 0.0f));
		modelaux = model; 
		model = glm::scale(model, glm::vec3(1.0f, 5.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.0f, 1.0f, 1.0f); // Cian
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[0]->RenderMesh();

		// ==============================================================================
		// 7. ARTICULACIÓN 4 (MAGENTA) - EL PIVOTE
		// ==============================================================================
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, -2.5f, 0.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion4()), glm::vec3(1.0f, 0.0f, 0.0f));
		glm::mat4 modelArticulacion4 = model;

		// Dibujo de la esfera Magenta (última articulación)
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 0.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		sp.render();

		// ==============================================================================
		// 8.CANASTA GRIS
		// ==============================================================================
		model = modelArticulacion4;
		model = glm::translate(model, glm::vec3(-1.55f, -0.75f, 0.0f));
		model = glm::scale(model, glm::vec3(4.0f, 1.5f, 3.0f));

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.8f, 0.8f, 0.8f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[0]->RenderMesh();
		glUseProgram(0);
		mainWindow.swapBuffers();
	}
	return 0;
}
