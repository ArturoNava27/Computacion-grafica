
#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>
#include <glew.h>
#include <glfw3.h>
#include <stdlib.h>
#include <time.h>

#include <glm.hpp>
#include <gtc\matrix_transform.hpp>
#include <gtc\type_ptr.hpp>

#include "Mesh.h"
#include "Shader.h"
#include "Window.h"

// Variables para la Casa
Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;

// Variables para las Letras (manuales)
GLuint letraVAO, letraVBO, shaderLetras;

// Shaders de las Letras (En línea para evitar depender de archivos externos adicionales)
static const char* vSourceLetras = "#version 330\n layout (location = 0) in vec3 pos; void main() { gl_Position = vec4(pos, 1.0); }";
static const char* fSourceLetras = "#version 330\n out vec4 color; uniform vec3 uColor; void main() { color = vec4(uColor, 1.0); }";

void CrearLetras() {
    GLfloat vertices[] = {
        // LETRA A 
        -0.95f, -0.90f, 0.0f,  -0.80f, -0.50f, 0.0f,  -0.85f, -0.90f, 0.0f,
        -0.95f, -0.90f, 0.0f,  -0.85f, -0.50f, 0.0f,  -0.80f, -0.50f, 0.0f,
        -0.65f, -0.90f, 0.0f,  -0.80f, -0.50f, 0.0f,  -0.75f, -0.90f, 0.0f,
        -0.65f, -0.90f, 0.0f,  -0.75f, -0.50f, 0.0f,  -0.80f, -0.50f, 0.0f,
        -0.88f, -0.75f, 0.0f,  -0.72f, -0.75f, 0.0f,  -0.72f, -0.70f, 0.0f,
        -0.88f, -0.75f, 0.0f,  -0.88f, -0.70f, 0.0f,  -0.72f, -0.70f, 0.0f,

        // LETRA D
        -0.15f, -0.25f, 0.0f, -0.05f, -0.25f, 0.0f, -0.05f, 0.25f, 0.0f,
        -0.15f, -0.25f, 0.0f, -0.15f,  0.25f, 0.0f, -0.05f, 0.25f, 0.0f,
        -0.05f,  0.25f, 0.0f,  0.10f,  0.25f, 0.0f,  0.10f, 0.15f, 0.0f,
        -0.05f,  0.25f, 0.0f, -0.05f,  0.15f, 0.0f,  0.10f, 0.15f, 0.0f,
        -0.05f, -0.25f, 0.0f,  0.10f, -0.25f, 0.0f,  0.10f, -0.15f, 0.0f,
        -0.05f, -0.25f, 0.0f, -0.05f, -0.15f, 0.0f,  0.10f, -0.15f, 0.0f,
        0.10f,  0.25f, 0.0f,  0.20f,  0.10f, 0.0f,  0.10f, -0.25f, 0.0f,
        0.20f,  0.10f, 0.0f,  0.20f, -0.10f, 0.0f,  0.10f, -0.25f, 0.0f,

        // LETRA N
        0.55f,  0.35f, 0.0f,  0.65f,  0.35f, 0.0f,  0.65f,  0.85f, 0.0f,
        0.55f,  0.35f, 0.0f,  0.55f,  0.85f, 0.0f,  0.65f,  0.85f, 0.0f,
        0.80f,  0.35f, 0.0f,  0.90f,  0.35f, 0.0f,  0.90f,  0.85f, 0.0f,
        0.80f,  0.35f, 0.0f,  0.80f,  0.85f, 0.0f,  0.90f,  0.85f, 0.0f,
        0.55f,  0.85f, 0.0f,  0.65f,  0.85f, 0.0f,  0.90f,  0.35f, 0.0f,
        0.55f,  0.85f, 0.0f,  0.90f,  0.35f, 0.0f,  0.80f,  0.35f, 0.0f
    };

    glGenVertexArrays(1, &letraVAO);
    glBindVertexArray(letraVAO);
    glGenBuffers(1, &letraVBO);
    glBindBuffer(GL_ARRAY_BUFFER, letraVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void CompileShaderLetras() {
    shaderLetras = glCreateProgram();
    GLuint v = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(v, 1, &vSourceLetras, NULL);
    glCompileShader(v);
    GLuint f = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(f, 1, &fSourceLetras, NULL);
    glCompileShader(f);
    glAttachShader(shaderLetras, v);
    glAttachShader(shaderLetras, f);
    glLinkProgram(shaderLetras);
}

void CreaPiramide() {
    unsigned int indices[] = { 0,1,2, 1,3,2, 3,0,2, 1,0,3 };
    GLfloat vertices[] = { -0.5f,-0.5f,0.0f, 0.5f,-0.5f,0.0f, 0.0f,0.5f,-0.25f, 0.0f,-0.5f,-0.5f };
    Mesh* obj1 = new Mesh();
    obj1->CreateMesh(vertices, indices, 12, 12);
    meshList.push_back(obj1);
}

void CrearCubo() {
    unsigned int cubo_indices[] = { 0,1,2, 2,3,0, 1,5,6, 6,2,1, 7,6,5, 5,4,7, 4,0,3, 3,7,4, 4,5,1, 1,0,4, 3,2,6, 6,7,3 };
    GLfloat cubo_vertices[] = { -0.5f,-0.5f,0.5f, 0.5f,-0.5f,0.5f, 0.5f,0.5f,0.5f, -0.5f,0.5f,0.5f, -0.5f,-0.5f,-0.5f, 0.5f,-0.5f,-0.5f, 0.5f,0.5f,-0.5f, -0.5f,0.5f,-0.5f };
    Mesh* cubo = new Mesh();
    cubo->CreateMesh(cubo_vertices, cubo_indices, 24, 36);
    meshList.push_back(cubo);
}

int main() {
    mainWindow = Window(800, 800);
    mainWindow.Initialise();
    glEnable(GL_DEPTH_TEST);

    CreaPiramide(); // index 0
    CrearCubo();     // index 1
    CrearLetras();
    CompileShaderLetras();

    Shader* solidShader = new Shader();
    solidShader->CreateFromFiles("shaders/shadersolid.vert", "shaders/shadersolid.frag");
    shaderList.push_back(*solidShader);

    glm::mat4 projection = glm::ortho(-1.8f, 1.8f, -1.8f, 1.8f, 0.1f, 100.0f);
    int ordenColores[3] = { 0, 1, 2 };
    int indiceColor = 0;
    srand(time(NULL));

    while (!mainWindow.getShouldClose()) {
        glfwPollEvents();

        static double ultimoCambio = -2.0;
        double tiempo = glfwGetTime();
        if (tiempo - ultimoCambio >= 2.0) {
            int estado = ordenColores[indiceColor];
            if (estado == 0) glClearColor(1.0f, 0.85f, 0.85f, 1.0f);
            else if (estado == 1) glClearColor(0.85f, 1.0f, 0.85f, 1.0f);
            else glClearColor(0.85f, 0.85f, 1.0f, 1.0f);
            indiceColor = (indiceColor + 1) % 3;
            ultimoCambio = tiempo;
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // --- 1. RENDERIZADO DE LA CASA ---
        shaderList[0].useShader();
        GLuint uniformModel = shaderList[0].getModelLocation();
        GLuint uniformProjection = shaderList[0].getProjectLocation();
        GLuint locColor = shaderList[0].getColorLocation();
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));

        // Cuerpo Rojo
        glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.4f, -1.5f));
        model = glm::scale(model, glm::vec3(1.8f, 1.4f, 0.7f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniform3f(locColor, 1.0f, 0.0f, 0.0f);
        meshList[1]->RenderMesh();

        // Techo Azul
        model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.9f, -1.5f));
        model = glm::scale(model, glm::vec3(2.2f, 1.2f, 0.8f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniform3f(locColor, 0.0f, 0.0f, 1.0f);
        meshList[0]->RenderMesh();

        // Ventanas (Verdes)
        model = glm::translate(glm::mat4(1.0f), glm::vec3(-0.5f, -0.1f, -1.0f));
        model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.1f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniform3f(locColor, 0.0f, 1.0f, 0.0f);
        meshList[1]->RenderMesh();

        model = glm::translate(glm::mat4(1.0f), glm::vec3(0.5f, -0.1f, -1.0f));
        model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.1f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        meshList[1]->RenderMesh();

        // Puerta
        model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.85f, -1.0f));
        model = glm::scale(model, glm::vec3(0.35f, 0.5f, 0.1f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        meshList[1]->RenderMesh();

        //  ÁRBOLES

         // Árbol Izquierdo
         // Tronco
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-1.3f, -0.92f, -1.5f));
        // Mantenemos el tronco ancho (0.25f) para estabilidad
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

        // --- 2. RENDERIZADO DE LAS LETRAS (Sin Depth Test para quedar al frente) ---
        glDisable(GL_DEPTH_TEST);
        glUseProgram(shaderLetras);
        GLuint uColorLetra = glGetUniformLocation(shaderLetras, "uColor");
        glBindVertexArray(letraVAO);

        glUniform3f(uColorLetra, 0.0f, 0.0f, 0.0f); // A - Negro
        glDrawArrays(GL_TRIANGLES, 0, 18);

        glUniform3f(uColorLetra, 0.5f, 0.0f, 0.5f); // D - Morado
        glDrawArrays(GL_TRIANGLES, 18, 24);

        glUniform3f(uColorLetra, 1.0f, 0.5f, 0.0f); // N - Naranja
        glDrawArrays(GL_TRIANGLES, 42, 18);

        glBindVertexArray(0);
        glUseProgram(0);
        glEnable(GL_DEPTH_TEST);

        mainWindow.swapBuffers();
    }
    return 0;
}