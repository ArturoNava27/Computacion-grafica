/*
Pr�ctica 5: Optimizaci�n y Carga de Modelos
*/
//para cargar imagen
#define STB_IMAGE_IMPLEMENTATION

#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>
#include <math.h>

#include <glew.h>
#include <glfw3.h>

#include <glm.hpp>
#include <gtc\matrix_transform.hpp>
#include <gtc\type_ptr.hpp>
#include<assimp/Importer.hpp>

#include "Window.h"
#include "Mesh.h"
#include "Shader_m.h"
#include "Camera.h"
#include "Sphere.h"
#include"Model.h"
#include "Skybox.h"

const float toRadians = 3.14159265f / 180.0f;
//float angulocola = 0.0f;
Window mainWindow;
std::vector<Mesh> meshList;
std::vector<Shader> shaderList;

Camera camera;

// Piezas del Carro
Model carroceria_M;
Model cofre_M;
Model llanta_del_der_M;
Model llanta_del_izq_M;
Model llanta_tra_der_M;
Model llanta_tra_izq_M;

Skybox* skybox; 

Sphere cabeza = Sphere(0.5, 20, 20);
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;


// Vertex Shader
static const char* vShader = "shaders/shader_m.vert";

// Fragment Shader
static const char* fShader = "shaders/shader_m.frag";



void CreateObjects()
{
	unsigned int indices[] = {
		0, 3, 1,
		1, 3, 2,
		2, 3, 0,
		0, 1, 2
	};

	GLfloat vertices[] = {
		//	x      y      z			u	  v			nx	  ny    nz
			-1.0f, -1.0f, -0.6f,	0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, -1.0f, 1.0f,		0.5f, 0.0f,		0.0f, 0.0f, 0.0f,
			1.0f, -1.0f, -0.6f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f,		0.5f, 1.0f,		0.0f, 0.0f, 0.0f
	};

	unsigned int floorIndices[] = {
		0, 2, 1,
		1, 2, 3
	};

	GLfloat floorVertices[] = {
		-10.0f, 0.0f, -10.0f,	0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, -10.0f,	10.0f, 0.0f,	0.0f, -1.0f, 0.0f,
		-10.0f, 0.0f, 10.0f,	0.0f, 10.0f,	0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, 10.0f,		10.0f, 10.0f,	0.0f, -1.0f, 0.0f
	};

	Mesh obj1;
	obj1.CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj1);

	Mesh obj2;
	obj2.CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj2);

	Mesh obj3;
	obj3.CreateMesh(floorVertices, floorIndices, 32, 6);
	meshList.push_back(obj3);

}


void CreateShaders()
{
	Shader* shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}

int main() {
    mainWindow = Window(1366, 768);
    mainWindow.Initialise();
    glEnable(GL_DEPTH_TEST);

    CreateObjects();
    CreateShaders();

    camera = Camera(glm::vec3(0.0f, 0.5f, 7.0f),
        glm::vec3(0.0f, 1.0f, 0.0f),
        -60.0f, 0.0f, 0.3f, 1.0f);

    // CARGA DE MODELOS
    carroceria_M.LoadModel("carroceria.obj");
    cofre_M.LoadModel("cofre.obj");
    llanta_del_der_M.LoadModel("llanta_del_der.obj");
    llanta_del_izq_M.LoadModel("llanta_del_izq.obj");
    llanta_tra_der_M.LoadModel("llanta_tra_der.obj");
    llanta_tra_izq_M.LoadModel("llanta_tra_izq.obj");

  
    GLuint uniformProjection, uniformModel, uniformView, uniformColor;
    glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 1000.0f);

    // VARIABLES DE CONTROL 
    float posicionCocheZ = -3.0f;
    float rotacionLlanta = 0.0f;
    float anguloCofre = 0.0f;
    float velocidadCoche = 5.0f;
    const float limiteCofre = 15.0f; // Límite reducido a consideración propia para notar su funcion real de cofre.
    float velocidadApertura = 5.0f; // Velocidad más lenta para notar el tope

    glm::mat4 model;
    glm::mat4 modelaux;

    while (!mainWindow.getShouldClose())
    {
        GLfloat now = (GLfloat)glfwGetTime();
        deltaTime = now - lastTime;
        lastTime = now;

        glfwPollEvents();

		//  CONTROL DE MOVIMIENTO (Teclas arriba y abajo)
        if (mainWindow.getsKeys()[GLFW_KEY_UP]) {
            posicionCocheZ -= velocidadCoche * deltaTime;
            rotacionLlanta += (velocidadCoche * 100.0f) * deltaTime;
        }
        if (mainWindow.getsKeys()[GLFW_KEY_DOWN]) {
            posicionCocheZ += velocidadCoche * deltaTime;
            rotacionLlanta -= (velocidadCoche * 100.0f) * deltaTime;
        }
        // ROTACIÓN INDEPENDIENTE DE LLANTAS (Tecla R adelante, Tecla T atrás)
        if (mainWindow.getsKeys()[GLFW_KEY_R]) {
            rotacionLlanta += 500.0f * deltaTime;
        }
        if (mainWindow.getsKeys()[GLFW_KEY_T]) {
            rotacionLlanta -= 500.0f * deltaTime;
        }
        // CONTROL DE COFRE (Tecla G)

        if (mainWindow.getsKeys()[GLFW_KEY_G]) {
            anguloCofre += velocidadApertura * deltaTime;
        }
        else {
            anguloCofre -= velocidadApertura * deltaTime;
        }

        if (anguloCofre > limiteCofre) anguloCofre = limiteCofre;
        if (anguloCofre < 0.0f) anguloCofre = 0.0f;

        camera.keyControl(mainWindow.getsKeys(), deltaTime);
        camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (skybox) skybox->DrawSkybox(camera.calculateViewMatrix(), projection);

        shaderList[0].UseShader();
        uniformModel = shaderList[0].GetModelLocation();
        uniformProjection = shaderList[0].GetProjectionLocation();
        uniformView = shaderList[0].GetViewLocation();
        uniformColor = shaderList[0].getColorLocation();

        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));

        // CARROCERÍA (Padre)
        glUniform3fv(uniformColor, 1, glm::value_ptr(glm::vec3(0.5f, 0.5f, 0.5f)));
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, -1.0f, posicionCocheZ));
        model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
        glm::mat4 matrizPadre = model;

        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(matrizPadre));
        carroceria_M.RenderModel();

        // COFRE 
        glUniform3fv(uniformColor, 1, glm::value_ptr(glm::vec3(0.05f, 0.05f, 0.05f)));
        modelaux = matrizPadre;

        modelaux = glm::translate(modelaux, glm::vec3(0.0f, 1.7f, 1.56f));
        modelaux = glm::rotate(modelaux, glm::radians(-anguloCofre), glm::vec3(1.0f, 0.0f, 0.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelaux));
        cofre_M.RenderModel();

        // LLANTAS 
        glUniform3fv(uniformColor, 1, glm::value_ptr(glm::vec3(0.15f, 0.15f, 0.15f)));
        Model* modelosLlantas[] = { &llanta_del_der_M, &llanta_del_izq_M, &llanta_tra_der_M, &llanta_tra_izq_M };

        glm::vec3 posLlantas[] = {
            glm::vec3(-2.1f, 0.7f,  2.5f),   // Delantera Derecha
            glm::vec3(2.1f, 0.7f,  2.5f),   // Delantera Izquierda
            glm::vec3(-2.1f, 0.7f, -3.2f),   // Trasera Derecha
            glm::vec3(2.1f, 0.7f, -3.2f)    // Trasera Izquierda
        };

        for (int i = 0; i < 4; i++) {
            modelaux = matrizPadre;
            modelaux = glm::translate(modelaux, posLlantas[i]);
            modelaux = glm::rotate(modelaux, glm::radians(rotacionLlanta), glm::vec3(1.0f, 0.0f, 0.0f));

            glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelaux));
            modelosLlantas[i]->RenderModel();
        }

        // PISO
        glUniform3fv(uniformColor, 1, glm::value_ptr(glm::vec3(0.2f, 0.5f, 0.2f)));
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        if (meshList.size() > 2) meshList[2].RenderMesh();

        glUseProgram(0);
        mainWindow.swapBuffers();
    } 

    return 0;
} 