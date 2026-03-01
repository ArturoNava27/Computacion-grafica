#include <stdio.h>
#include <string.h>
#include <glew.h>
#include <glfw3.h>
#include <stdlib.h> // Para rand y srand
#include <time.h>   // Para time

//Dimensiones de la ventana
const int WIDTH = 800, HEIGHT = 800;
GLuint VAO, VBO, shader;

//LENGUAJE DE SHADER (SOMBRAS) GLSL
//Vertex Shader (convierte datos a xyz)
//recibir color, salida Vcolor
static const char* vShader = "						\n\
#version 330										\n\
layout (location =0) in vec3 pos;					\n\
void main()											\n\
{													\n\
gl_Position=vec4(pos.x,pos.y,pos.z,1.0f); 			\n\
}";

//Fragment Shader (procesar color)
//recibir Vcolor y dar de salida color
static const char* fShader = "						\n\
#version 330										\n\
out vec4 color;										\n\
uniform vec3 uColor;								\n\
void main()											\n\
{													\n\
	color = vec4(uColor, 1.0f);				 		\n\
}";


void CrearTriangulo()
{
	GLfloat vertices[] = {
		// =====================================================
		// LETRA A (Puntas unidas fue el mejor que logré)
		// =====================================================
		// Lado izquierdo (Diagonal externa e interna)
		-0.95f, -0.90f, 0.0f,  -0.80f, -0.50f, 0.0f,  -0.85f, -0.90f, 0.0f,
		-0.95f, -0.90f, 0.0f,  -0.85f, -0.50f, 0.0f,  -0.80f, -0.50f, 0.0f,

		// Lado derecho (Diagonal externa e interna)
		-0.65f, -0.90f, 0.0f,  -0.80f, -0.50f, 0.0f,  -0.75f, -0.90f, 0.0f,
		-0.65f, -0.90f, 0.0f,  -0.75f, -0.50f, 0.0f,  -0.80f, -0.50f, 0.0f,

		// Barra central (El puente horizontal de la A)
		-0.88f, -0.75f, 0.0f,  -0.72f, -0.75f, 0.0f,  -0.72f, -0.70f, 0.0f,
		-0.88f, -0.75f, 0.0f,  -0.88f, -0.70f, 0.0f,  -0.72f, -0.70f, 0.0f,

		// =====================================================
		// LETRA D (Construcción por bloques)
		// =====================================================
		// Poste vertical izquierdo (Rectángulo principal)
		-0.15f, -0.25f, 0.0f, -0.05f, -0.25f, 0.0f, -0.05f, 0.25f, 0.0f,
		-0.15f, -0.25f, 0.0f, -0.15f,  0.25f, 0.0f, -0.05f, 0.25f, 0.0f,

		// Travesaño superior (Horizontal arriba)
		-0.05f,  0.25f, 0.0f,  0.10f,  0.25f, 0.0f,  0.10f, 0.15f, 0.0f,
		-0.05f,  0.25f, 0.0f, -0.05f,  0.15f, 0.0f,  0.10f, 0.15f, 0.0f,

		// Travesaño inferior (Horizontal abajo)
		-0.05f, -0.25f, 0.0f,  0.10f, -0.25f, 0.0f,  0.10f, -0.15f, 0.0f,
		-0.05f, -0.25f, 0.0f, -0.05f, -0.15f, 0.0f,  0.10f, -0.15f, 0.0f,

		// Curva/Cierre derecho (Triángulos que dan forma de D)
		0.10f,  0.25f, 0.0f,  0.20f,  0.10f, 0.0f,  0.10f, -0.25f, 0.0f,
		0.20f,  0.10f, 0.0f,  0.20f, -0.10f, 0.0f,  0.10f, -0.25f, 0.0f,

		// =====================================================
		// LETRA N (Tres columnas unidas)
		// =====================================================
		// Poste vertical izquierdo
		0.55f,  0.35f, 0.0f,  0.65f,  0.35f, 0.0f,  0.65f,  0.85f, 0.0f,
		0.55f,  0.35f, 0.0f,  0.55f,  0.85f, 0.0f,  0.65f,  0.85f, 0.0f,

		// Poste vertical derecho
		0.80f,  0.35f, 0.0f,  0.90f,  0.35f, 0.0f,  0.90f,  0.85f, 0.0f,
		0.80f,  0.35f, 0.0f,  0.80f,  0.85f, 0.0f,  0.90f,  0.85f, 0.0f,

		// Diagonal central (Une la parte superior izq con inferior der)
		0.55f,  0.85f, 0.0f,  0.65f,  0.85f, 0.0f,  0.90f,  0.35f, 0.0f,
		0.55f,  0.85f, 0.0f,  0.90f,  0.35f, 0.0f,  0.80f,  0.35f, 0.0f
	};
	glGenVertexArrays(1, &VAO); //generar 1 VAO
	glBindVertexArray(VAO);//asignar VAO

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); //pasarle los datos al VBO asignando tamano, los datos y en este caso es estático pues no se modificarán los valores

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT), (GLvoid*)0);//Stride en caso de haber datos de color por ejemplo, es saltar cierta cantidad de datos
	glEnableVertexAttribArray(0);
	//agregar valores a vèrtices y luego declarar un nuevo vertexAttribPointer
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

}
void AddShader(GLuint theProgram, const char* shaderCode, GLenum shaderType) //Función para agregar los shaders a la tarjeta gráfica

//the Program recibe los datos de theShader

{
	GLuint theShader = glCreateShader(shaderType);//theShader es un shader que se crea de acuerdo al tipo de shader: vertex o fragment
	const GLchar* theCode[1];
	theCode[0] = shaderCode;//shaderCode es el texto que se le pasa a theCode
	GLint codeLength[1];
	codeLength[0] = strlen(shaderCode);//longitud del texto
	glShaderSource(theShader, 1, theCode, codeLength);//Se le asigna al shader el código
	glCompileShader(theShader);//Se comila el shader
	GLint result = 0;
	GLchar eLog[1024] = { 0 };
	//verificaciones y prevención de errores
	glGetShaderiv(theShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetProgramInfoLog(shader, sizeof(eLog), NULL, eLog);
		printf("EL error al compilar el shader %d es: %s \n", shaderType, eLog);
		return;
	}
	glAttachShader(theProgram, theShader);//Si no hubo problemas se asigna el shader a theProgram el cual asigna el código a la tarjeta gráfica
}

void CompileShaders() {
	shader = glCreateProgram(); //se crea un programa
	if (!shader)
	{
		printf("Error creando el shader");
		return;
	}
	AddShader(shader, vShader, GL_VERTEX_SHADER);//Agregar vertex shader
	AddShader(shader, fShader, GL_FRAGMENT_SHADER);//Agregar fragment shader
	//Para terminar de linkear el programa y ver que no tengamos errores
	GLint result = 0;
	GLchar eLog[1024] = { 0 };
	glLinkProgram(shader);//se linkean los shaders a la tarjeta gráfica
	//verificaciones y prevención de errores
	glGetProgramiv(shader, GL_LINK_STATUS, &result);
	if (!result)
	{
		glGetProgramInfoLog(shader, sizeof(eLog), NULL, eLog);
		printf("EL error al linkear es: %s \n", eLog);
		return;
	}
	glValidateProgram(shader);
	glGetProgramiv(shader, GL_VALIDATE_STATUS, &result);
	if (!result)
	{
		glGetProgramInfoLog(shader, sizeof(eLog), NULL, eLog);
		printf("EL error al validar es: %s \n", eLog);
		return;
	}



}
int ordenColores[3] = { 0,1,2 }; // 0 = rojo, 1 = verde, 2 = azul
int indiceColor = 0; // Variable para rastrear el índice actual del color
int main()
{
	//Inicialización de GLFW
	if (!glfwInit())
	{
		printf("Falló inicializar GLFW");
		glfwTerminate();
		return 1;
	}

	//****  LAS SIGUIENTES 4 LÍNEAS SE COMENTAN EN DADO CASO DE QUE AL USUARIO NO LE FUNCIONE LA VENTANA Y PUEDA CONOCER LA VERSIÓN DE OPENGL QUE TIENE ****/

	//Asignando variables de GLFW y propiedades de ventana
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	//para solo usar el core profile de OpenGL y no tener retrocompatibilidad
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	//CREAR VENTANA
	GLFWwindow* mainWindow = glfwCreateWindow(WIDTH, HEIGHT, "Primer ventana", NULL, NULL);

	if (!mainWindow)
	{
		printf("Fallo en crearse la ventana con GLFW");
		glfwTerminate();
		return 1;
	}
	//Obtener tamaño de Buffer
	int BufferWidth, BufferHeight;
	glfwGetFramebufferSize(mainWindow, &BufferWidth, &BufferHeight);

	//asignar el contexto
	glfwMakeContextCurrent(mainWindow);

	//permitir nuevas extensiones
	glewExperimental = GL_TRUE;

	if (glewInit() != GLEW_OK)
	{
		printf("Falló inicialización de GLEW");
		glfwDestroyWindow(mainWindow);
		glfwTerminate();
		return 1;
	}

	// Asignar valores de la ventana y coordenadas
	//Asignar Viewport
	glViewport(0, 0, BufferWidth, BufferHeight);

	//Llamada a las funciones creadas antes del main
	CrearTriangulo();
	CompileShaders();

	srand(time(NULL)); // Inicializa semilla aleatoria
	// Mezclar los 3 colores (Fisher-Yates simplificado)
	for (int i = 2; i > 0; i--) {
		int j = rand() % (i + 1);
		int temp = ordenColores[i];
		ordenColores[i] = ordenColores[j];
		ordenColores[j] = temp;
	}


	//Loop mientras no se cierra la ventana
	while (!glfwWindowShouldClose(mainWindow))
	{
		glfwPollEvents();

		// --- LÓGICA DE CAMBIO DE COLOR ---
		static double ultimoCambio = -2.0; //(Agregué esta modificación para que el primer cambio de color ocurra inmediatamente al iniciar el programa)
		double tiempo = glfwGetTime();

		if (tiempo - ultimoCambio >= 2.0) {

			int estado = ordenColores[indiceColor];

			if (estado == 0) {
				glClearColor(1.0f, 0.0f, 0.0f, 1.0f); // Rojo
			}
			else if (estado == 1) {
				glClearColor(0.0f, 1.0f, 0.0f, 1.0f); // Verde
			}
			else {
				glClearColor(0.0f, 0.0f, 1.0f, 1.0f); // Azul
			}

			indiceColor = (indiceColor + 1) % 3; // Ciclo infinito
			ultimoCambio = tiempo;
		}
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(shader);
		// Obtener la ubicación de la variable uColor en el shader
		GLuint colorLoc = glGetUniformLocation(shader, "uColor");
		glBindVertexArray(VAO);

		// --- DIBUJAR LETRA A (NEGRO) ---
		// Los primeros 18 vértices (6 triángulos) corresponden a la A
		glUniform3f(colorLoc, 0.0f, 0.0f, 0.0f); // Negro
		glDrawArrays(GL_TRIANGLES, 0, 18);

		// --- DIBUJAR LETRA D (MORADO) ---
		// Los siguientes 24 vértices (8 triángulos) corresponden a la D
		glUniform3f(colorLoc, 0.5f, 0.0f, 0.5f); // Morado
		glDrawArrays(GL_TRIANGLES, 18, 24);

		// --- DIBUJAR LETRA N (NARANJA) ---
		// Los últimos 18 vértices corresponden a la N
		glUniform3f(colorLoc, 1.0f, 0.5f, 0.0f); // Naranja
		glDrawArrays(GL_TRIANGLES, 42, 18);

		glBindVertexArray(0);
		glUseProgram(0);

		glfwSwapBuffers(mainWindow);
	}

	return 0;
}