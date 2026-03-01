//Práctica 2: índices, mesh, proyecciones, transformaciones geométricas
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
//clases para dar orden y limpieza al código
#include"Mesh.h"
#include"Shader.h"
#include"Window.h"

//Dimensiones de la ventana
const float toRadians = 3.14159265f / 180.0;
Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<MeshColor*> meshColorList;
std::vector<Shader> shaderList;

//Vertex Shader
static const char* vShader = "shaders/shader.vert";
static const char* fShader = "shaders/shader.frag";
static const char* vShaderColor = "shaders/shadercolor.vert";
static const char* fShaderColor = "shaders/shadercolor.frag";

float angulo = 0.0f;

//Pirámide triangular regular
void CreaPiramide()
{
	unsigned int indices[] = {
		0,1,2,
		1,3,2,
		3,0,2,
		1,0,3
	};
	GLfloat vertices[] = {
		-0.5f, -0.5f, 0.0f,	//0
		 0.5f, -0.5f, 0.0f,	//1
		 0.0f,  0.5f, -0.25f,	//2
		 0.0f, -0.5f, -0.5f,	//3
	};
	Mesh* obj1 = new Mesh();
	obj1->CreateMesh(vertices, indices, 12, 12);
	meshList.push_back(obj1);
}

//Vértices de un cubo
void CrearCubo()
{
	unsigned int cubo_indices[] = {
		0, 1, 2, 2, 3, 0, // front
		1, 5, 6, 6, 2, 1, // right
		7, 6, 5, 5, 4, 7, // back
		4, 0, 3, 3, 7, 4, // left
		4, 5, 1, 1, 0, 4, // bottom
		3, 2, 6, 6, 7, 3  // top
	};

	GLfloat cubo_vertices[] = {
		-0.5f, -0.5f,  0.5f,
		 0.5f, -0.5f,  0.5f,
		 0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,
		-0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f,
		 0.5f,  0.5f, -0.5f,
		-0.5f,  0.5f, -0.5f
	};
	Mesh* cubo = new Mesh();
	cubo->CreateMesh(cubo_vertices, cubo_indices, 24, 36);
	meshList.push_back(cubo);
}

// (Omitimos CrearLetrasyFiguras por brevedad, pero mantenla en tu código)

void CreateShaders()
{
	Shader* shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);

	Shader* shader2 = new Shader();
	shader2->CreateFromFiles(vShaderColor, fShaderColor);
	shaderList.push_back(*shader2);

	Shader* solidShader = new Shader();
	solidShader->CreateFromFiles("shaders/shadersolid.vert", "shaders/shadersolid.frag");
	shaderList.push_back(*solidShader);
}

int main()
{
	mainWindow = Window(800, 800);
	mainWindow.Initialise();
	glEnable(GL_DEPTH_TEST);

	CreaPiramide(); // Mesh index 0
	CrearCubo();     // Mesh index 1
	CreateShaders();

	GLuint uniformProjection = 0;
	GLuint uniformModel = 0;
	GLuint locColor = 0;

	// --- 1. CONFIGURACIÓN DE PROYECCIÓN (UNA SOLA VEZ) ---
	glm::mat4 projection = glm::ortho(-1.8f, 1.8f, -1.8f, 1.8f, 0.1f, 100.0f);
	glm::mat4 model(1.0f);

	//Loop mientras no se cierra la ventana
	while (!mainWindow.getShouldClose())
	{
		glfwPollEvents();
		// --- CAMBIO DE FONDO A BLANCO ---
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // R=1, G=1, B=1 es Blanco
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shaderList[2].useShader();
		uniformModel = shaderList[2].getModelLocation();
		uniformProjection = shaderList[2].getProjectLocation();
		locColor = shaderList[2].getColorLocation();

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));

		// --- 1. CUERPO DE LA CASA (ROJO) 
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, -0.4f, -1.5f));
		model = glm::scale(model, glm::vec3(1.8f, 1.4f, 0.7f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3f(locColor, 1.0f, 0.0f, 0.0f);
		meshList[1]->RenderMesh();

		// --- 2. TECHO (AZUL )
		model = glm::mat4(1.0f);
		// Subimos a 0.9f para que el borde inferior del triángulo toque exactamente el tope del rojo
		model = glm::translate(model, glm::vec3(0.0f, 0.9f, -1.5f));
		model = glm::scale(model, glm::vec3(2.2f, 1.2f, 0.8f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3f(locColor, 0.0f, 0.0f, 1.0f);
		meshList[0]->RenderMesh();

		// --- 3. VENTANAS (VERDES )
		// Ventana Izquierda
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-0.5f, -0.1f, -1.0f)); // Bajamos a -0.1f
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.1f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3f(locColor, 0.0f, 1.0f, 0.0f);
		meshList[1]->RenderMesh();

		// Ventana Derecha
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.5f, -0.1f, -1.0f)); // Bajamos a -0.1f
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.1f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3f(locColor, 0.0f, 1.0f, 0.0f);
		meshList[1]->RenderMesh();

		// --- 4. PUERTA ---
		model = glm::mat4(1.0f);
		// Bajamos a -0.85f para eliminar ese espacio milimétrico con el borde
		model = glm::translate(model, glm::vec3(0.0f, -0.85f, -1.0f));
		// Escala estilizada: un poco más angosta y baja
		model = glm::scale(model, glm::vec3(0.35f, 0.5f, 0.1f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3f(locColor, 0.0f, 1.0f, 0.0f);
		meshList[1]->RenderMesh();

		// --- 5. ÁRBOLES FRONDOSOS ---

		// Árbol Izquierdo
		// Tronco
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-1.3f, -0.92f, -1.5f));
		model = glm::scale(model, glm::vec3(0.25f, 0.35f, 0.1f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3f(locColor, 0.478f, 0.255f, 0.067f);
		meshList[1]->RenderMesh();

		// Copa
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-1.3f, -0.35f, -1.2f));
		// Aumentamos escala X a 0.6f 
		model = glm::scale(model, glm::vec3(0.6f, 0.8f, 0.4f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3f(locColor, 0.0f, 0.5f, 0.0f);
		meshList[0]->RenderMesh();

		// Árbol Derecho
		// Tronco
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(1.3f, -0.92f, -1.5f));
		model = glm::scale(model, glm::vec3(0.25f, 0.35f, 0.1f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3f(locColor, 0.478f, 0.255f, 0.067f);
		meshList[1]->RenderMesh();

		// Copa
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(1.3f, -0.35f, -1.2f));
		// Aumentamos escala X a 0.6f
		model = glm::scale(model, glm::vec3(0.6f, 0.8f, 0.4f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3f(locColor, 0.0f, 0.5f, 0.0f);
		meshList[0]->RenderMesh();

		

		glUseProgram(0);
		mainWindow.swapBuffers();
	
	}
	return 0;
}