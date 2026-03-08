// Pyraminx con 9 pirámides por cara
#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>
#include <glew.h>
#include <glfw3.h>

// GLM
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

// Clases del proyecto
#include "Mesh.h"
#include "Shader.h"
#include "Window.h"
#include "Camera.h"

using std::vector;

const float toRadians = 3.14159265f / 180.0;

GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;

Camera camera;
Window mainWindow;

vector<Mesh*> meshList;
vector<Shader> shaderList;

static const char* vShader = "shaders/shader.vert";
static const char* fShader = "shaders/shader.frag";


//---------------------------------------------------------
// PIRÁMIDE TRIANGULAR
//---------------------------------------------------------

void CrearPiramideTriangular()
{
	unsigned int indices[] =
	{
		0,1,2,
		1,3,2,
		3,0,2,
		1,0,3
	};

	GLfloat vertices[] =
	{
		-0.5f,-0.5f,0.0f,
		 0.5f,-0.5f,0.0f,
		 0.0f,0.5f,-0.25f,
		 0.0f,-0.5f,-0.5f
	};

	Mesh* obj = new Mesh();
	obj->CreateMesh(vertices, indices, 12, 12);
	meshList.push_back(obj);
}

//---------------------------------------------------------

void CreateShaders()
{
	Shader* shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}

//---------------------------------------------------------
// DIBUJAR UNA PIEZA DE LA PYRAMINX
//---------------------------------------------------------

void DibujarPieza(glm::mat4 globalRotation,
	GLuint uniformModel,
	GLuint uniformColor,
	glm::vec3 pos,
	glm::vec3 color,
	bool invertida)
{
	glm::mat4 model(1.0f);

	model = glm::translate(model, pos);
	model = model * globalRotation;
	model = glm::scale(model, glm::vec3(0.35f));

	if (invertida)
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(1, 0, 0));

	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	glUniform3f(uniformColor, color.x, color.y, color.z);

	meshList[0]->RenderMesh();
}

void DibujarCaraModular(glm::mat4 baseFace, GLuint uniformModel, GLuint uniformColor, glm::vec3 color) {
	float size = 0.62f; // Un poco más grandes para cubrir el negro
	float sep = 0.65f;
	float h = sep * 0.866f;

	// Ajustamos este valor para subir o bajar el bloque de 9 triángulos dentro de la cara
	float verticalAdj = -0.45f;

	// Piezas normales
	for (int fila = 0; fila < 3; fila++) {
		for (int col = 0; col <= fila; col++) {
			glm::mat4 model = baseFace;
			float x = (col - fila * 0.5f) * sep;
			float y = (1.0f - fila) * h + verticalAdj;

			model = glm::translate(model, glm::vec3(x, y, 0.01f));
			model = glm::scale(model, glm::vec3(size, size, 0.02f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			glUniform3f(uniformColor, color.x, color.y, color.z);
			meshList[0]->RenderMesh();
		}
	}
	// 2. DIBUJAR LOS 3 TRIÁNGULOS INVERTIDOS (Rellenan los huecos)
	for (int fila = 0; fila < 2; fila++) { // Solo hay en los huecos de fila 0-1 y 1-2
		for (int col = 0; col <= fila; col++) {
			glm::mat4 model = baseFace;

			// X es el punto medio entre dos triángulos normales
			float x = (col - fila * 0.5f) * sep;
			// Y debe estar desplazado media altura respecto a los normales para encajar
			float y = (0.66f - fila) * h - 0.22f;

			model = glm::translate(model, glm::vec3(x, y, 0.01f));
			// Rotación de 180 grados
			model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0, 0, 1));
			model = glm::scale(model, glm::vec3(size, size, 0.02f));

			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			glUniform3f(uniformColor, color.x, color.y, color.z);
			meshList[0]->RenderMesh();
		}
	}
}
//
// PYRAMINX COMPLETO
//---------------------------------------------------------
void DibujarPyraminxCompleto(glm::mat4 globalRotation, GLuint uniformModel, GLuint uniformColor, glm::vec3 centro)
{
	// ---------- MARCO NEGRO ----------
	glm::mat4 marcoModel(1.0f);
	marcoModel = glm::translate(marcoModel, centro);
	marcoModel = marcoModel * globalRotation;
	marcoModel = glm::scale(marcoModel, glm::vec3(1.6f)); // tamaño del tetraedro

	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(marcoModel));
	glUniform3f(uniformColor, 0, 0, 0);
	meshList[0]->RenderMesh();


	// ---------- CONFIGURACIÓN DE CARAS ----------
	struct Cara { float rotY; glm::vec3 color; };

	Cara caras[] =
	{
		{0.0f,   glm::vec3(0,0,1)},   // azul
		{120.0f, glm::vec3(0,1,0)},   // verde
		{240.0f, glm::vec3(1,0,0)}    // rojo
	};

	float inclinacion = 19.47f; // inclinación correcta del tetraedro
	float dist = 0.55f;         // distancia desde el centro


	// ---------- CARAS LATERALES ----------
	for (int i = 0; i < 3; i++)
	{
		glm::mat4 baseFace(1.0f);

		baseFace = glm::translate(baseFace, centro);

		// rotación global del objeto
		baseFace = baseFace * globalRotation;

		// orientación de cada cara
		baseFace = glm::rotate(baseFace, glm::radians(caras[i].rotY), glm::vec3(0, 1, 0));

		// inclinación del tetraedro
		baseFace = glm::rotate(baseFace, glm::radians(inclinacion), glm::vec3(1, 0, 0));

		// separación desde el centro
		baseFace = glm::translate(baseFace, glm::vec3(0, 0, dist));

		DibujarCaraModular(baseFace, uniformModel, uniformColor, caras[i].color);
	}


	// ---------- CARA INFERIOR ----------
	glm::mat4 baseAbajo(1.0f);

	baseAbajo = glm::translate(baseAbajo, centro);
	baseAbajo = baseAbajo * globalRotation;

	// orienta la cara hacia abajo
	baseAbajo = glm::rotate(baseAbajo, glm::radians(-90.0f), glm::vec3(1, 0, 0));

	// distancia desde el centro
	baseAbajo = glm::translate(baseAbajo, glm::vec3(0, 0, -0.85f));

	DibujarCaraModular(baseAbajo, uniformModel, uniformColor, glm::vec3(1, 1, 0)); // amarillo
}
int main()
{
	mainWindow = Window(800, 600);
	mainWindow.Initialise();

	// Dentro del main, después de mainWindow.Initialise();
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	

	CrearPiramideTriangular();
	CreateShaders();

	camera = Camera(glm::vec3(0, 0, 5),
		glm::vec3(0, 1, 0),
		-90,
		0,
		0.5,
		0.5);

	GLuint uniformProjection = 0;
	GLuint uniformModel = 0;
	GLuint uniformView = 0;
	GLuint uniformColor = 0;

	glm::mat4 projection =
		glm::perspective(glm::radians(60.0f),
			mainWindow.getBufferWidth() /
			mainWindow.getBufferHeight(),
			0.1f,
			100.0f);

	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		lastTime = now;

		glfwPollEvents();

		// Dentro del while, cambia el color de limpieza a blanco
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());


		shaderList[0].useShader();

		uniformModel = shaderList[0].getModelLocation();
		uniformProjection = shaderList[0].getProjectLocation();
		uniformView = shaderList[0].getViewLocation();
		uniformColor = shaderList[0].getColorLocation();

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));

		glm::mat4 globalRotation(1.0f);

		globalRotation = glm::rotate(globalRotation,
			glm::radians(mainWindow.getrotax()),
			glm::vec3(1, 0, 0));

		globalRotation = glm::rotate(globalRotation,
			glm::radians(mainWindow.getrotay()),
			glm::vec3(0, 1, 0));

		globalRotation = glm::rotate(globalRotation,
			glm::radians(mainWindow.getrotaz()),
			glm::vec3(0, 0, 1));

		// Dentro del while:
		glm::vec3 centroPyraminx = glm::vec3(0.0f, 0.0f, -2.0f); // Definir la posición
		DibujarPyraminxCompleto(globalRotation, uniformModel, uniformColor, centroPyraminx); // Pasar los 4 argumentos

		glUseProgram(0);
		mainWindow.swapBuffers();
	}

	return 0;
}