

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


#include "Window.h"
#include "Mesh.h"
#include "Shader_light.h"
#include "Camera.h"
#include "Texture.h"
#include "Sphere.h"
#include"Model.h"
#include "Skybox.h"


#include "CommonValues.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "Material.h"
const float toRadians = 3.14159265f / 180.0f;


float movCoche;
float movOffset;
float rotllanta;
float rotllantaOffset;
float rotCocheY;
float giroCocheOffset;
int estadoCoche;
float posCocheZ;
float anguloVueltaU;
float direccionLlantas;


float movNave;
float posNaveY;
float rotNaveY;
float rotAla;
float naveOffset;
float naveYOffset;
float giroNaveOffset;
float alaOffset;
int estadoNave;
int dirNaveY;
int dirAla;
bool avanza;
float toffsetflechau = 0.0f;
float toffsetflechav = 0.0f;
float toffsetnumerou = 0.0f;
float toffsetnumerov = 0.0f;
float toffsetnumerocambiau = 0.0;
float angulovaria = 0.0f;


float rotMallaX;
float rotMallaY;
float rotMallaZ;
float rotMallaXOffset;
float rotMallaYOffset;
float rotMallaZOffset;


float timeOfDay;
float dayNightSpeed;
float intensidadDia;
float intensidadNoche;


struct EstadoGloboSuave
{
	glm::vec3 posicion;
	float escala;
	float rotY;
};


glm::vec3 PuntoRutaGloboSuave(float angulo, glm::vec3 centro, float radioX, float radioZ,
	float alturaBase, float amplitudAltura, float ondaX, float ondaZ)
{
	glm::vec3 p;
	p.x = centro.x + (cos(angulo) * radioX) + (sin(angulo * 2.0f) * ondaX);
	p.z = centro.z + (sin(angulo) * radioZ) + (cos(angulo * 1.5f) * ondaZ);
	p.y = alturaBase + (sin(angulo * 1.7f) * amplitudAltura);
	return p;
}


EstadoGloboSuave CalcularEstadoGloboSuave(float tiempoAnimacion, glm::vec3 centroRuta,
	float radioX, float radioZ, float alturaBase, float escalaBase, float velocidad,
	float desfase, bool direccionOpuesta, float amplitudAltura, float ondaX, float ondaZ)
{
	EstadoGloboSuave estado;

	float sentido = direccionOpuesta ? -1.0f : 1.0f;
	float angulo = (tiempoAnimacion * velocidad * sentido) + desfase;

	glm::vec3 posicionActual = PuntoRutaGloboSuave(angulo, centroRuta, radioX, radioZ,
		alturaBase, amplitudAltura, ondaX, ondaZ);

	glm::vec3 posicionSiguiente = PuntoRutaGloboSuave(angulo + (0.025f * sentido), centroRuta,
		radioX, radioZ, alturaBase, amplitudAltura, ondaX, ondaZ);

	glm::vec3 direccion = posicionSiguiente - posicionActual;

	estado.posicion = posicionActual;
	estado.escala = escalaBase * (1.0f + (0.018f * sin(angulo * 1.3f)));

	if (glm::length(direccion) > 0.001f)
		estado.rotY = (atan2(direccion.x, direccion.z) / toRadians) + 180.0f;
	else
		estado.rotY = 0.0f;

	return estado;
}

Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;

Camera camera;

Texture brickTexture;
Texture dirtTexture;
Texture plainTexture;
Texture pisoTexture;
Texture AgaveTexture;
Texture FlechaTexture;
Texture NumerosTexture;
Texture Numero1Texture;
Texture Numero2Texture;


Model BaseYBolaAzul_M;
Model MallaCircular_M;
Model Faro_M;
Model Pueblo_M;
Model PuenteProgreso_M;
Model Estacion_M;
Model Globo_M;
Model Nave2_M;
Model Maquinarr_M;
Model BaseAro_M;
Model Aro1_M;
Model Aro2_M;
Model Aro3_M;
Model Aro4_M;

Skybox skybox;


Material Material_brillante;
Material Material_opaco;


GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;


DirectionalLight mainLight;

PointLight pointLights[MAX_POINT_LIGHTS];
SpotLight spotLights[MAX_SPOT_LIGHTS];


static const char* vShader = "shaders/shader_light.vert";


static const char* fShader = "shaders/shader_light.frag";


void calcAverageNormals(unsigned int* indices, unsigned int indiceCount, GLfloat* vertices, unsigned int verticeCount,
	unsigned int vLength, unsigned int normalOffset)
{
	for (size_t i = 0; i < indiceCount; i += 3)
	{
		unsigned int in0 = indices[i] * vLength;
		unsigned int in1 = indices[i + 1] * vLength;
		unsigned int in2 = indices[i + 2] * vLength;
		glm::vec3 v1(vertices[in1] - vertices[in0], vertices[in1 + 1] - vertices[in0 + 1], vertices[in1 + 2] - vertices[in0 + 2]);
		glm::vec3 v2(vertices[in2] - vertices[in0], vertices[in2 + 1] - vertices[in0 + 1], vertices[in2 + 2] - vertices[in0 + 2]);
		glm::vec3 normal = glm::cross(v1, v2);
		normal = glm::normalize(normal);

		in0 += normalOffset; in1 += normalOffset; in2 += normalOffset;
		vertices[in0] += normal.x; vertices[in0 + 1] += normal.y; vertices[in0 + 2] += normal.z;
		vertices[in1] += normal.x; vertices[in1 + 1] += normal.y; vertices[in1 + 2] += normal.z;
		vertices[in2] += normal.x; vertices[in2 + 1] += normal.y; vertices[in2 + 2] += normal.z;
	}

	for (size_t i = 0; i < verticeCount / vLength; i++)
	{
		unsigned int nOffset = i * vLength + normalOffset;
		glm::vec3 vec(vertices[nOffset], vertices[nOffset + 1], vertices[nOffset + 2]);
		vec = glm::normalize(vec);
		vertices[nOffset] = vec.x; vertices[nOffset + 1] = vec.y; vertices[nOffset + 2] = vec.z;
	}
}


float Clamp01(float valor)
{
	if (valor < 0.0f)
		return 0.0f;
	if (valor > 1.0f)
		return 1.0f;
	return valor;
}

float SuavizarAnimacion(float t)
{
	t = Clamp01(t);
	return t * t * (3.0f - (2.0f * t));
}

void CreateObjects()
{
	unsigned int indices[] = {
		0, 3, 1,
		1, 3, 2,
		2, 3, 0,
		0, 1, 2
	};

	GLfloat vertices[] = {

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
		10.0f, 0.0f, -10.0f,	1.0f, 0.0f,	0.0f, -1.0f, 0.0f,
		-10.0f, 0.0f, 10.0f,	0.0f, 1.0f,	0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, 10.0f,		1.0f, 1.0f,	0.0f, -1.0f, 0.0f
	};
	unsigned int vegetacionIndices[] = {
	   0, 1, 2,
	   0, 2, 3,
	   4,5,6,
	   4,6,7
	};

	GLfloat vegetacionVertices[] = {
		-0.5f, -0.5f, 0.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.5f, -0.5f, 0.0f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.5f, 0.5f, 0.0f,		1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		-0.5f, 0.5f, 0.0f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f,

		0.0f, -0.5f, -0.5f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.5f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, 0.5f,		1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, -0.5f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f,


	};


	unsigned int flechaIndices[] = {
	   0, 1, 2,
	   0, 2, 3,
	};

	GLfloat flechaVertices[] = {
		-0.5f, 0.0f, 0.5f,		0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, 0.5f,		1.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, -0.5f,		1.0f, 1.0f,		0.0f, -1.0f, 0.0f,
		-0.5f, 0.0f, -0.5f,		0.0f, 1.0f,		0.0f, -1.0f, 0.0f,

	};

	unsigned int scoreIndices[] = {
	   0, 1, 2,
	   0, 2, 3,
	};

	GLfloat scoreVertices[] = {
		-0.5f, 0.0f, 0.5f,		0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, 0.5f,		1.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, -0.5f,		1.0f, 1.0f,		0.0f, -1.0f, 0.0f,
		-0.5f, 0.0f, -0.5f,		0.0f, 1.0f,		0.0f, -1.0f, 0.0f,

	};

	unsigned int numeroIndices[] = {
	   0, 1, 2,
	   0, 2, 3,
	};

	GLfloat numeroVertices[] = {
		-0.5f, 0.0f, 0.5f,		0.0f, 0.67f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, 0.5f,		0.25f, 0.67f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, -0.5f,		0.25f, 1.0f,		0.0f, -1.0f, 0.0f,
		-0.5f, 0.0f, -0.5f,		0.0f, 1.0f,		0.0f, -1.0f, 0.0f,

	};

	Mesh* obj1 = new Mesh();
	obj1->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj1);

	Mesh* obj2 = new Mesh();
	obj2->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj2);

	Mesh* obj3 = new Mesh();
	obj3->CreateMesh(floorVertices, floorIndices, 32, 6);
	meshList.push_back(obj3);


	Mesh* obj4 = new Mesh();
	obj4->CreateMesh(vegetacionVertices, vegetacionIndices, 64, 12);
	meshList.push_back(obj4);

	Mesh* obj5 = new Mesh();
	obj5->CreateMesh(flechaVertices, flechaIndices, 32, 6);
	meshList.push_back(obj5);

	Mesh* obj6 = new Mesh();
	obj6->CreateMesh(scoreVertices, scoreIndices, 32, 6);
	meshList.push_back(obj6);

	Mesh* obj7 = new Mesh();
	obj7->CreateMesh(numeroVertices, numeroIndices, 32, 6);
	meshList.push_back(obj7);

}


void CreateShaders()
{
	Shader* shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}


int main()
{
	mainWindow = Window(1366, 768);
	mainWindow.Initialise();

	CreateObjects();
	CreateShaders();

	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 0.5f, 0.5f);

	brickTexture = Texture("Textures/brick.png");
	brickTexture.LoadTextureA();
	dirtTexture = Texture("Textures/dirt.png");
	dirtTexture.LoadTextureA();
	plainTexture = Texture("Textures/plain.png");
	plainTexture.LoadTextureA();
	pisoTexture = Texture("Textures/piso.png");
	pisoTexture.LoadTextureA();
	AgaveTexture = Texture("Textures/Agave.tga");
	AgaveTexture.LoadTextureA();
	FlechaTexture = Texture("Textures/flechas.tga");
	FlechaTexture.LoadTextureA();
	NumerosTexture = Texture("Textures/numerosbase.tga");
	NumerosTexture.LoadTextureA();
	Numero1Texture = Texture("Textures/numero1.tga");
	Numero1Texture.LoadTextureA();
	Numero2Texture = Texture("Textures/numero2.tga");
	Numero2Texture.LoadTextureA();


	BaseYBolaAzul_M = Model();
	BaseYBolaAzul_M.LoadModel("Models/baseybolaazul.obj");

	MallaCircular_M = Model();
	MallaCircular_M.LoadModel("Models/mallacircular.obj");

	Faro_M = Model();
	Faro_M.LoadModel("Models/faro.obj");

	Pueblo_M = Model();
	Pueblo_M.LoadModel("Models/pueblo.obj");

	PuenteProgreso_M = Model();
	PuenteProgreso_M.LoadModel("Models/puenteprogreso.obj");

	Estacion_M = Model();
	Estacion_M.LoadModel("Models/estacion.obj");

	Globo_M = Model();
	Globo_M.LoadModel("Models/globo.obj");

	Nave2_M = Model();
	Nave2_M.LoadModel("Models/nave2.obj");

	Maquinarr_M = Model();
	Maquinarr_M.LoadModel("Models/maquinarr.obj");

	BaseAro_M = Model();
	BaseAro_M.LoadModel("Models/basearo.obj");

	Aro1_M = Model();
	Aro1_M.LoadModel("Models/aro1.obj");

	Aro2_M = Model();
	Aro2_M.LoadModel("Models/aro2.obj");

	Aro3_M = Model();
	Aro3_M.LoadModel("Models/aro3.obj");

	Aro4_M = Model();
	Aro4_M.LoadModel("Models/aro4.obj");


	std::vector<std::string> skyboxFaces;
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_rt.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_lf.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_dn.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_up.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_bk.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_ft.tga");

	skybox = Skybox(skyboxFaces);

	Material_brillante = Material(4.0f, 256);
	Material_opaco = Material(0.3f, 4);


	mainLight = DirectionalLight(1.0f, 1.0f, 1.0f,
		0.5f, 0.5f,
		0.0f, -1.0f, -1.0f);

	unsigned int pointLightCount = 0;

	// Se reservan 3 point lights para las luces que sí usa el proyecto:
	// [0] globo azul fijo, [1] Incognium, [2] pueblo/faros.
	// Arrancan apagadas para evitar luces rojas/azules sobrantes cerca del origen.
	pointLights[0] = PointLight(0.0f, 0.55f, 1.0f,
		0.0f, 0.0f,
		0.0f, 0.0f, 0.0f,
		1.0f, 0.0010f, 0.000006f);
	pointLightCount++;

	pointLights[1] = PointLight(0.0f, 0.45f, 1.0f,
		0.0f, 0.0f,
		0.0f, 0.0f, 0.0f,
		1.0f, 0.010f, 0.0010f);
	pointLightCount++;

	pointLights[2] = PointLight(1.0f, 0.86f, 0.55f,
		0.0f, 0.0f,
		0.0f, 0.0f, 0.0f,
		1.0f, 0.00025f, 0.0000025f);
	pointLightCount++;

	unsigned int spotLightCount = 0;

	// Spotlight conservado:
	// [0] luz/linterna que sigue a la cámara.
	spotLights[0] = SpotLight(1.0f, 1.0f, 1.0f,
		0.0f, 2.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		5.0f);
	spotLightCount++;


	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0, uniformTextureOffset = 0;
	GLuint uniformColor = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 1000.0f);

	movCoche = 0.0f;
	movOffset = 0.1f;
	rotllanta = 0.0f;
	rotllantaOffset = 10.0f;
	rotCocheY = 0.0f;
	giroCocheOffset = 1.15f;
	estadoCoche = 0;
	posCocheZ = -2.0f;
	anguloVueltaU = 0.0f;
	direccionLlantas = 0.0f;

	movNave = 0.0f;
	posNaveY = 0.0f;
	rotNaveY = 0.0f;
	rotAla = 0.0f;
	naveOffset = 0.06f;
	naveYOffset = 0.05f;
	giroNaveOffset = 1.0f;
	alaOffset = 2.5f;
	estadoNave = 0;
	dirNaveY = 1;
	dirAla = 1;

	rotMallaX = 0.0f;
	rotMallaY = 0.0f;
	rotMallaZ = 0.0f;
	rotMallaXOffset = 0.9f;
	rotMallaYOffset = 1.3f;
	rotMallaZOffset = 0.7f;

	timeOfDay = 8.0f;
	dayNightSpeed = 0.015f;
	intensidadDia = 1.0f;
	intensidadNoche = 0.0f;


	bool luzIncogniumActiva = true;
	bool luzFarosActiva = true;
	bool teclaILiberada = true;
	bool teclaFLiberada = true;

	glm::vec3 lowerLight(0.0f, 0.0f, 0.0f);

	glm::mat4 model(1.0);
	glm::mat4 modelaux(1.0);
	glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
	glm::vec2 toffset = glm::vec2(0.0f, 0.0f);
	bool datosInicialesGrupoMostrados = false;

	// Ajuste final de la jerarquia escena completa.
	// Se deja fijo y sin menu de teclado.
	glm::vec3 ajusteJerarquiaEscena = glm::vec3(159.97f, 0.00f, 185.73f);
	float escalaJerarquiaEscena = 1.66f;


	glm::vec3 posicionGrupoPueblo = glm::vec3(-162.25f, -0.38f, -218.39f);
	float rotacionGrupoPuebloY = -2611.83f;
	float escalaGrupoPueblo = 1.0f;


	float velocidadMoverGrupoPueblo = 2.5f;
	float velocidadAlturaGrupoPueblo = 1.5f;
	float velocidadEscalaGrupoPueblo = 0.25f;
	float escalaMinimaGrupoPueblo = 0.20f;
	float escalaMaximaGrupoPueblo = 8.0f;
	float velocidadRotacionGrupoPueblo = 35.0f;
	float tiempoImpresionGrupoPueblo = 0.0f;


	glm::vec3 posicionGrupoIncognium = glm::vec3(-0.04f, -1.47f, 0.14f);
	float escalaGrupoIncognium = 81.74f;
	glm::vec3 pivoteMalla = glm::vec3(-0.000776f, 0.12575f, 0.0f);


	float velocidadMoverIncognium = 1.2f;
	float velocidadAlturaIncognium = 1.0f;
	float velocidadEscalaIncognium = 8.0f;
	float escalaMinimaIncognium = 5.0f;
	float escalaMaximaIncognium = 120.0f;
	float tiempoImpresionIncognium = 0.0f;


	glm::vec3 posicionFaro = glm::vec3(-26.37f, -2.00f, 26.95f);
	glm::vec3 posicionFaros[4] = {
		glm::vec3(-26.37f, -2.00f, 26.95f),
		glm::vec3(26.37f, -2.00f, 26.95f),
		glm::vec3(-26.37f, -2.00f, -26.95f),
		glm::vec3(26.37f, -2.00f, -26.95f)
	};
	float escalaFaro = 1.17f;
	glm::vec3 posicionLocalLuzFaro = glm::vec3(0.0f, 9.15f, 0.0f);

	glm::vec3 posicionLocalCristalFaro = glm::vec3(0.0f, 7.55f, 0.0f);


	glm::vec3 posicionLocalAuraFaro = glm::vec3(0.0f, 7.55f, 0.0f);


	float velocidadMoverFaro = 1.2f;
	float velocidadAlturaFaro = 1.0f;
	float velocidadEscalaFaro = 0.8f;
	float escalaMinimaFaro = 0.2f;
	float escalaMaximaFaro = 20.0f;
	float tiempoImpresionFaro = 0.0f;


	glm::vec3 posicionPuenteProgreso = glm::vec3(3.07f, 5.74f, 114.52f);
	float rotacionPuenteProgresoY = 0.0f;
	float escalaPuenteProgreso = 53.37f;


	float velocidadMoverPuente = 2.5f;
	float velocidadAlturaPuente = 1.5f;
	float velocidadEscalaPuente = 18.0f;
	float escalaMinimaPuente = 1.0f;
	float escalaMaximaPuente = 250.0f;
	float tiempoImpresionPuente = 0.0f;


	glm::vec3 posicionEstacion = glm::vec3(71.51f, 3.07f, -79.38f);
	float rotacionEstacionY = -1433.31f;
	float escalaEstacion = 39.63f;


	float velocidadMoverEstacion = 2.5f;
	float velocidadAlturaEstacion = 1.5f;
	float velocidadEscalaEstacion = 10.0f;
	float escalaMinimaEstacion = 1.0f;
	float escalaMaximaEstacion = 200.0f;
	float velocidadRotacionEstacion = 35.0f;
	float tiempoImpresionEstacion = 0.0f;


	// Ajuste fijo de la jerarquia de la estacion.
	// Se aplica como padre de toda la estacion para que se muevan juntos:
	// estacion + 3 globos + luz/aura del globo.
	glm::vec3 ajusteJerarquiaEstacionFija = glm::vec3(-131.18f, 1.49f, 5.30f);

	glm::vec3 posicionGlobo = glm::vec3(78.02f, 45.49f, -46.34f);
	float rotacionGloboY = -785.62f;
	float escalaGlobo = 9.19f;


	glm::vec3 posicionGloboArriba = posicionGlobo;
	float rotacionGloboArribaY = rotacionGloboY;
	float escalaGloboArriba = escalaGlobo;

	glm::vec3 posicionGloboAbajo = posicionGlobo;
	float rotacionGloboAbajoY = rotacionGloboY;
	float escalaGloboAbajo = escalaGlobo;


	float velocidadMoverGlobo = 2.5f;
	float velocidadAlturaGlobo = 1.5f;
	float velocidadEscalaGlobo = 8.0f;
	float escalaMinimaGlobo = 1.0f;
	float escalaMaximaGlobo = 200.0f;
	float velocidadRotacionGlobo = 35.0f;
	float tiempoImpresionGlobo = 0.0f;


	float tiempoAnimacionGlobo = 0.0f;
	bool animacionGloboActiva = true;


	float factorLocalEstacion = 1.0f / escalaEstacion;
	glm::mat4 matrizLocalEstacionInicial = glm::mat4(1.0f);
	matrizLocalEstacionInicial = glm::translate(matrizLocalEstacionInicial, posicionEstacion);
	matrizLocalEstacionInicial = glm::rotate(matrizLocalEstacionInicial, rotacionEstacionY * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	matrizLocalEstacionInicial = glm::scale(matrizLocalEstacionInicial, glm::vec3(escalaEstacion, escalaEstacion, escalaEstacion));
	glm::mat4 inversaLocalEstacionInicial = glm::inverse(matrizLocalEstacionInicial);


	glm::vec3 centroRutaGloboPrincipal = glm::vec3(inversaLocalEstacionInicial * glm::vec4(80.0f, 45.49f, -103.0f, 1.0f));
	glm::vec3 centroRutaGloboArriba = glm::vec3(inversaLocalEstacionInicial * glm::vec4(78.0f, 67.49f, -78.0f, 1.0f));
	glm::vec3 centroRutaGloboAbajo = glm::vec3(inversaLocalEstacionInicial * glm::vec4(80.0f, 28.49f, -101.0f, 1.0f));


	float velocidadGloboPrincipal = 0.115f;
	float velocidadGloboArriba = 0.095f;
	float velocidadGloboAbajo = 0.128f;

	float desfaseGloboPrincipal = 4.05f;
	float desfaseGloboArriba = 1.35f;
	float desfaseGloboAbajo = 4.55f;

	float escalaGloboLocalBase = 9.19f * factorLocalEstacion;


	EstadoGloboSuave estadoInicialGlobo = CalcularEstadoGloboSuave(tiempoAnimacionGlobo, centroRutaGloboPrincipal,
		39.0f * factorLocalEstacion, 48.0f * factorLocalEstacion, centroRutaGloboPrincipal.y, escalaGloboLocalBase, velocidadGloboPrincipal, desfaseGloboPrincipal,
		false, 2.25f * factorLocalEstacion, 3.5f * factorLocalEstacion, 2.5f * factorLocalEstacion);
	posicionGlobo = estadoInicialGlobo.posicion;
	escalaGlobo = estadoInicialGlobo.escala;
	rotacionGloboY = estadoInicialGlobo.rotY;

	EstadoGloboSuave estadoInicialGloboArriba = CalcularEstadoGloboSuave(tiempoAnimacionGlobo, centroRutaGloboArriba,
		45.0f * factorLocalEstacion, 41.0f * factorLocalEstacion, centroRutaGloboArriba.y, escalaGloboLocalBase, velocidadGloboArriba, desfaseGloboArriba,
		true, 2.8f * factorLocalEstacion, 4.0f * factorLocalEstacion, 3.0f * factorLocalEstacion);
	posicionGloboArriba = estadoInicialGloboArriba.posicion;
	escalaGloboArriba = estadoInicialGloboArriba.escala;
	rotacionGloboArribaY = estadoInicialGloboArriba.rotY;

	EstadoGloboSuave estadoInicialGloboAbajo = CalcularEstadoGloboSuave(tiempoAnimacionGlobo, centroRutaGloboAbajo,
		28.0f * factorLocalEstacion, 34.0f * factorLocalEstacion, centroRutaGloboAbajo.y, escalaGloboLocalBase, velocidadGloboAbajo, desfaseGloboAbajo,
		false, 1.85f * factorLocalEstacion, 2.2f * factorLocalEstacion, 1.7f * factorLocalEstacion);
	posicionGloboAbajo = estadoInicialGloboAbajo.posicion;
	escalaGloboAbajo = estadoInicialGloboAbajo.escala;
	rotacionGloboAbajoY = estadoInicialGloboAbajo.rotY;


	glm::vec3 posicionGrupoAros = glm::vec3(-250.12f, 0.00f, -124.16f);
	float rotacionGrupoArosY = -20251.06f;
	float escalaGrupoAros = 0.21f;


	glm::vec3 nave2Inicio = glm::vec3(-276.43f, 7.39f, -124.17f);
	glm::vec3 nave2Media = glm::vec3(-297.98f, 7.39f, -124.17f);
	glm::vec3 nave2Final = glm::vec3(-312.53f, 7.39f, -124.17f);
	glm::vec3 nave2Desaparicion = glm::vec3(-385.00f, 7.39f, -124.17f);
	float nave2RotYInicio = -1979.40f;
	float nave2RotYMedia = -1979.40f;
	float nave2RotYFinal = -1979.40f;
	float nave2EscalaInicio = 12.10f;
	float nave2EscalaMedia = 12.10f;
	float nave2EscalaFinal = 12.10f;
	float nave2EscalaDesaparicion = 0.05f;

	glm::vec3 posicionNave2 = nave2Inicio;
	float rotacionNave2Y = nave2RotYInicio;
	float escalaNave2 = nave2EscalaInicio;

	bool animacionNave2Activa = false;
	bool nave2Visible = true;
	bool flashNave2Activo = false;
	bool teclaNLiberada = true;
	bool tecla2LiberadaCopia = true;
	int faseAnimacionNave2 = 0;
	float tiempoFaseNave2 = 0.0f;
	float duracionTramoNave2_1 = 2.55f;
	float duracionTramoNave2_2 = 2.05f;
	float duracionTramoNave2_3 = 2.20f;
	float duracionFlashNave2 = 1.20f;
	float intensidadFlashNave2 = 0.0f;
	float escalaFlashNave2 = 0.0f;


	glm::vec3 separacionLateralNave2Copia = glm::vec3(0.0f, 0.0f, 38.0f);


	glm::vec3 posicionGrupoArosCopia = glm::vec3(-250.06f, 0.00f, -86.16f);
	float rotacionGrupoArosCopiaY = -20250.14f;
	float escalaGrupoArosCopia = 0.21f;


	glm::vec3 posicionMaquinarr = glm::vec3(-247.53f, 0.00f, -170.49f);
	float rotacionMaquinarrY = 90.10f;
	float escalaMaquinarr = 13.74f;


	glm::vec3 posicionMaquinarrBaseConversion = glm::vec3(-255.35f, 0.00f, -105.41f);
	float rotacionMaquinarrBaseConversionY = 0.0f;
	float escalaMaquinarrBaseConversion = 12.52f;


	glm::mat4 jerarquiaMaquinarrInicial = glm::mat4(1.0f);
	jerarquiaMaquinarrInicial = glm::translate(jerarquiaMaquinarrInicial, posicionMaquinarrBaseConversion);
	jerarquiaMaquinarrInicial = glm::rotate(jerarquiaMaquinarrInicial, rotacionMaquinarrBaseConversionY * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	jerarquiaMaquinarrInicial = glm::scale(jerarquiaMaquinarrInicial, glm::vec3(escalaMaquinarrBaseConversion, escalaMaquinarrBaseConversion, escalaMaquinarrBaseConversion));
	glm::mat4 inversaJerarquiaMaquinarrInicial = glm::inverse(jerarquiaMaquinarrInicial);

	auto convertirAMaquinarrLocal = [&](glm::vec3 posicionMundo)
		{
			return glm::vec3(inversaJerarquiaMaquinarrInicial * glm::vec4(posicionMundo, 1.0f));
		};

	posicionGrupoAros = convertirAMaquinarrLocal(posicionGrupoAros);
	posicionGrupoArosCopia = convertirAMaquinarrLocal(posicionGrupoArosCopia);
	nave2Inicio = convertirAMaquinarrLocal(nave2Inicio);
	nave2Media = convertirAMaquinarrLocal(nave2Media);
	nave2Final = convertirAMaquinarrLocal(nave2Final);
	nave2Desaparicion = convertirAMaquinarrLocal(nave2Desaparicion);
	separacionLateralNave2Copia = glm::vec3(inversaJerarquiaMaquinarrInicial * glm::vec4(separacionLateralNave2Copia, 0.0f));
	posicionNave2 = nave2Inicio;

	glm::vec3 nave2CopiaAparicion = nave2Desaparicion + separacionLateralNave2Copia;
	glm::vec3 nave2CopiaTramo1 = nave2Final + separacionLateralNave2Copia;
	glm::vec3 nave2CopiaTramo2 = nave2Media + separacionLateralNave2Copia;
	glm::vec3 nave2CopiaFinal = nave2Inicio + separacionLateralNave2Copia;
	float nave2CopiaRotY = nave2RotYInicio + 180.0f;
	float nave2CopiaRotYFinal = nave2CopiaRotY + 180.0f;
	float nave2CopiaEscalaCompleta = nave2EscalaInicio;
	float nave2CopiaEscalaInicial = 0.05f;

	glm::vec3 posicionNave2Copia = nave2CopiaAparicion;
	float rotacionNave2CopiaY = nave2CopiaRotY;
	float escalaNave2Copia = nave2CopiaEscalaInicial;
	bool nave2CopiaVisible = false;
	bool flashNave2CopiaActivo = false;
	bool animacionNave2CopiaActiva = false;
	int faseAnimacionNave2Copia = 0;
	float tiempoFaseNave2Copia = 0.0f;
	float duracionFlashNave2Copia = 1.20f;
	float duracionAparicionNave2Copia = 1.25f;
	float duracionCopiaTramo1 = 2.20f;
	float duracionCopiaTramo2 = 2.05f;
	float duracionCopiaTramo3 = 2.55f;
	float duracionGiroFinalNave2Copia = 1.25f;
	float intensidadFlashNave2Copia = 0.0f;
	float escalaFlashNave2Copia = 0.0f;


	float tiempoAnimacionAros = 0.0f;
	float duracionCicloArosNormal = 4.20f;
	float duracionCicloArosRapida = 1.05f;
	float duracionCicloArosActual = duracionCicloArosNormal;
	int cicloArosActual = 0;
	bool animacionArosActiva = false;
	bool tecla1LiberadaAros = true;
	bool animacionUnidaArosNave2Activa = false;
	bool iniciarNave2DespuesAros = false;


	glm::vec3 ejeSalidaAros = glm::vec3(0.0f, 0.0f, 1.0f);


	float separacionOriginalAro1 = 0.00f;
	float separacionOriginalAro2 = 3.50f;
	float separacionOriginalAro3 = 7.00f;
	float separacionOriginalAro4 = 10.50f;


	float retrocesoAro1 = -8.00f;
	float retrocesoAro2 = -12.00f;
	float retrocesoAro3 = -16.00f;
	float retrocesoAro4 = -20.00f;


	float frenteAro1 = 8.00f;
	float frenteAro2 = 20.00f;
	float frenteAro3 = 34.00f;
	float frenteAro4 = 50.00f;


	float offsetAro1 = -separacionOriginalAro1 + frenteAro1;
	float offsetAro2 = -separacionOriginalAro2 + frenteAro2;
	float offsetAro3 = -separacionOriginalAro3 + frenteAro3;
	float offsetAro4 = -separacionOriginalAro4 + frenteAro4;


	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime();
		GLfloat frameTime = now - lastTime;
		deltaTime = frameTime;
		deltaTime += frameTime / limitFPS;
		lastTime = now;

		angulovaria += 0.5f * deltaTime;

		if (!datosInicialesGrupoMostrados)
		{
			printf("Grupo completo inicial -> pos = glm::vec3(%.2ff, %.2ff, %.2ff); | escala = %.2ff | rotY = %.2ff\n",
				posicionGrupoPueblo.x, posicionGrupoPueblo.y, posicionGrupoPueblo.z, escalaGrupoPueblo, rotacionGrupoPuebloY);
			printf("Puente fijo -> pos = glm::vec3(%.2ff, %.2ff, %.2ff); | escala = %.2ff | rotY = %.2ff\n",
				posicionPuenteProgreso.x, posicionPuenteProgreso.y, posicionPuenteProgreso.z, escalaPuenteProgreso, rotacionPuenteProgresoY);
			printf("Estacion base -> pos = glm::vec3(%.2ff, %.2ff, %.2ff); | ajuste = glm::vec3(%.2ff, %.2ff, %.2ff); | escala = %.2ff | rotY = %.2ff\n",
				posicionEstacion.x, posicionEstacion.y, posicionEstacion.z,
				ajusteJerarquiaEstacionFija.x, ajusteJerarquiaEstacionFija.y, ajusteJerarquiaEstacionFija.z,
				escalaEstacion, rotacionEstacionY);
			printf("Globo inicial -> pos = glm::vec3(%.2ff, %.2ff, %.2ff); | escala = %.2ff | rotY = %.2ff\n",
				posicionGlobo.x, posicionGlobo.y, posicionGlobo.z, escalaGlobo, rotacionGloboY);
			datosInicialesGrupoMostrados = true;
		}


		timeOfDay += dayNightSpeed * deltaTime;
		if (timeOfDay >= 24.0f)
			timeOfDay = 0.0f;


		float cicloDia = sin(((timeOfDay / 24.0f) * 360.0f - 90.0f) * toRadians);
		intensidadDia = (cicloDia + 1.0f) / 2.0f;
		if (intensidadDia < 0.08f)
			intensidadDia = 0.08f;
		if (intensidadDia > 1.0f)
			intensidadDia = 1.0f;
		intensidadNoche = 1.0f - intensidadDia;

		float rojoSol = 0.12f + (0.88f * intensidadDia);
		float verdeSol = 0.16f + (0.84f * intensidadDia);
		float azulSol = 0.35f + (0.65f * intensidadDia);
		float ambienteSol = 0.05f + (0.45f * intensidadDia);
		float difusaSol = 0.10f + (0.75f * intensidadDia);


		mainLight = DirectionalLight(rojoSol, verdeSol, azulSol,
			ambienteSol, difusaSol,
			0.0f, -1.0f, -1.0f);


		rotMallaX += rotMallaXOffset * deltaTime;
		rotMallaY += rotMallaYOffset * deltaTime;
		rotMallaZ += rotMallaZOffset * deltaTime;

		if (rotMallaX >= 360.0f)
			rotMallaX = 0.0f;
		if (rotMallaY >= 360.0f)
			rotMallaY = 0.0f;
		if (rotMallaZ >= 360.0f)
			rotMallaZ = 0.0f;


		float limiteAvanceCoche = -195.0f;
		float radioVueltaU = 25.0f;
		float zInicialCoche = -2.0f;
		float zCarrilRegreso = zInicialCoche + (radioVueltaU * 2.0f);
		float anguloDireccionLlanta = 28.0f;

		if (estadoCoche == 0)
		{
			rotCocheY = 0.0f;
			posCocheZ = zInicialCoche;
			direccionLlantas = 0.0f;

			if (movCoche > limiteAvanceCoche)
			{
				movCoche -= movOffset * deltaTime;
				rotllanta += rotllantaOffset * deltaTime;
			}
			else
			{
				movCoche = limiteAvanceCoche;
				anguloVueltaU = 0.0f;
				estadoCoche = 1;
			}
		}
		else if (estadoCoche == 1)
		{
			anguloVueltaU += giroCocheOffset * deltaTime;
			if (anguloVueltaU > 180.0f)
				anguloVueltaU = 180.0f;

			float anguloRad = anguloVueltaU * toRadians;
			rotCocheY = anguloVueltaU;
			movCoche = limiteAvanceCoche - (radioVueltaU * sin(anguloRad));
			posCocheZ = zInicialCoche + (radioVueltaU * (1.0f - cos(anguloRad)));
			direccionLlantas = anguloDireccionLlanta;
			rotllanta += rotllantaOffset * deltaTime;

			if (anguloVueltaU >= 180.0f)
			{
				movCoche = limiteAvanceCoche;
				posCocheZ = zCarrilRegreso;
				rotCocheY = 180.0f;
				direccionLlantas = 0.0f;
				estadoCoche = 2;
			}
		}
		else if (estadoCoche == 2)
		{
			rotCocheY = 180.0f;
			posCocheZ = zCarrilRegreso;
			direccionLlantas = 0.0f;

			if (movCoche < 0.0f)
			{
				movCoche += movOffset * deltaTime;
				rotllanta += rotllantaOffset * deltaTime;
			}
			else
			{
				movCoche = 0.0f;
				anguloVueltaU = 0.0f;
				estadoCoche = 3;
			}
		}
		else if (estadoCoche == 3)
		{
			anguloVueltaU += giroCocheOffset * deltaTime;
			if (anguloVueltaU > 180.0f)
				anguloVueltaU = 180.0f;

			float anguloRad = anguloVueltaU * toRadians;
			rotCocheY = 180.0f + anguloVueltaU;
			movCoche = radioVueltaU * sin(anguloRad);
			posCocheZ = zCarrilRegreso - (radioVueltaU * (1.0f - cos(anguloRad)));
			direccionLlantas = anguloDireccionLlanta;
			rotllanta += rotllantaOffset * deltaTime;

			if (anguloVueltaU >= 180.0f)
			{
				movCoche = 0.0f;
				posCocheZ = zInicialCoche;
				rotCocheY = 0.0f;
				direccionLlantas = 0.0f;
				estadoCoche = 0;
			}
		}


		float limiteNave = 20.0f;
		float limiteNaveYSuperior = 2.0f;
		float limiteNaveYInferior = -2.0f;
		float limiteAlaSuperior = 35.0f;
		float limiteAlaInferior = -35.0f;

		posNaveY += naveYOffset * deltaTime * dirNaveY;
		if (posNaveY >= limiteNaveYSuperior)
		{
			posNaveY = limiteNaveYSuperior;
			dirNaveY = -1;
		}
		else if (posNaveY <= limiteNaveYInferior)
		{
			posNaveY = limiteNaveYInferior;
			dirNaveY = 1;
		}

		rotAla += alaOffset * deltaTime * dirAla;
		if (rotAla >= limiteAlaSuperior)
		{
			rotAla = limiteAlaSuperior;
			dirAla = -1;
		}
		else if (rotAla <= limiteAlaInferior)
		{
			rotAla = limiteAlaInferior;
			dirAla = 1;
		}

		if (estadoNave == 0)
		{
			rotNaveY = 0.0f;
			if (movNave < limiteNave)
				movNave += naveOffset * deltaTime;
			else
			{
				movNave = limiteNave;
				estadoNave = 1;
			}
		}
		else if (estadoNave == 1)
		{
			rotNaveY += giroNaveOffset * deltaTime;
			if (rotNaveY >= 180.0f)
			{
				rotNaveY = 180.0f;
				estadoNave = 2;
			}
		}
		else if (estadoNave == 2)
		{
			rotNaveY = 180.0f;
			if (movNave > 0.0f)
				movNave -= naveOffset * deltaTime;
			else
			{
				movNave = 0.0f;
				estadoNave = 3;
			}
		}
		else if (estadoNave == 3)
		{
			rotNaveY -= giroNaveOffset * deltaTime;
			if (rotNaveY <= 0.0f)
			{
				rotNaveY = 0.0f;
				estadoNave = 0;
			}
		}


		bool* keysAros = mainWindow.getsKeys();
		if (keysAros[GLFW_KEY_1])
		{
			if (tecla1LiberadaAros && !animacionUnidaArosNave2Activa && !animacionArosActiva && !animacionNave2Activa)
			{


				animacionUnidaArosNave2Activa = true;
				iniciarNave2DespuesAros = true;
				animacionArosActiva = true;
				tiempoAnimacionAros = 0.0f;
				cicloArosActual = 0;
				duracionCicloArosActual = duracionCicloArosNormal;


				posicionNave2 = nave2Inicio;
				rotacionNave2Y = nave2RotYInicio;
				escalaNave2 = nave2EscalaInicio;
				nave2Visible = true;
				flashNave2Activo = false;
				intensidadFlashNave2 = 0.0f;
				escalaFlashNave2 = 0.0f;
				faseAnimacionNave2 = 0;
				tiempoFaseNave2 = 0.0f;
				animacionNave2Activa = false;

				printf("Animacion unida: aros originales -> Nave 2 original.\n");
				tecla1LiberadaAros = false;
			}
		}
		else
		{
			tecla1LiberadaAros = true;
		}


		float poseAro1 = frenteAro1;
		float poseAro2 = frenteAro2;
		float poseAro3 = frenteAro3;
		float poseAro4 = frenteAro4;

		if (animacionArosActiva)
		{
			tiempoAnimacionAros += frameTime;
			if (tiempoAnimacionAros >= duracionCicloArosActual)
			{
				if (cicloArosActual == 0)
				{

					cicloArosActual = 1;
					duracionCicloArosActual = duracionCicloArosRapida;
					tiempoAnimacionAros = 0.0f;
				}
				else
				{
					tiempoAnimacionAros = 0.0f;
					duracionCicloArosActual = duracionCicloArosNormal;
					cicloArosActual = 0;
					animacionArosActiva = false;

					if (iniciarNave2DespuesAros)
					{

						posicionNave2 = nave2Inicio;
						rotacionNave2Y = nave2RotYInicio;
						escalaNave2 = nave2EscalaInicio;
						nave2Visible = true;
						flashNave2Activo = false;
						intensidadFlashNave2 = 0.0f;
						escalaFlashNave2 = 0.0f;
						faseAnimacionNave2 = 1;
						tiempoFaseNave2 = 0.0f;
						animacionNave2Activa = true;
						iniciarNave2DespuesAros = false;
						printf("Aros terminados. Inicia Nave 2 original.\n");
					}
				}
			}

			float progresoAros = tiempoAnimacionAros / duracionCicloArosActual;

			auto limitarAros = [](float t)
				{
					if (t < 0.0f) t = 0.0f;
					if (t > 1.0f) t = 1.0f;
					return t;
				};

			auto suavizarAros = [&](float t)
				{
					t = limitarAros(t);
					return t * t * (3.0f - 2.0f * t);
				};

			auto mezclarAros = [&](float a, float b, float t)
				{
					return a + ((b - a) * suavizarAros(t));
				};

			if (progresoAros < 0.12f)
			{

				poseAro1 = frenteAro1;
				poseAro2 = frenteAro2;
				poseAro3 = frenteAro3;
				poseAro4 = frenteAro4;
			}
			else if (progresoAros < 0.32f)
			{

				float t = (progresoAros - 0.12f) / 0.20f;
				poseAro1 = mezclarAros(frenteAro1, 0.0f, t);
				poseAro2 = mezclarAros(frenteAro2, 0.0f, t);
				poseAro3 = mezclarAros(frenteAro3, 0.0f, t);
				poseAro4 = mezclarAros(frenteAro4, 0.0f, t);
			}
			else if (progresoAros < 0.45f)
			{

				poseAro1 = 0.0f;
				poseAro2 = 0.0f;
				poseAro3 = 0.0f;
				poseAro4 = 0.0f;
			}
			else if (progresoAros < 0.58f)
			{

				float t = (progresoAros - 0.45f) / 0.13f;
				poseAro1 = mezclarAros(0.0f, retrocesoAro1, t);
				poseAro2 = mezclarAros(0.0f, retrocesoAro2, t);
				poseAro3 = mezclarAros(0.0f, retrocesoAro3, t);
				poseAro4 = mezclarAros(0.0f, retrocesoAro4, t);
			}
			else if (progresoAros < 0.68f)
			{

				float t = (progresoAros - 0.58f) / 0.10f;
				poseAro1 = mezclarAros(retrocesoAro1, frenteAro1, t);
				poseAro2 = mezclarAros(retrocesoAro2, frenteAro2, t);
				poseAro3 = mezclarAros(retrocesoAro3, frenteAro3, t);
				poseAro4 = mezclarAros(retrocesoAro4, frenteAro4, t);
			}
			else if (progresoAros < 0.82f)
			{

				float t = (progresoAros - 0.68f) / 0.14f;
				float rebote = sin(suavizarAros(t) * 3.14159265f);
				poseAro1 = frenteAro1 + (0.35f * rebote);
				poseAro2 = frenteAro2 + (0.65f * rebote);
				poseAro3 = frenteAro3 + (0.95f * rebote);
				poseAro4 = frenteAro4 + (1.25f * rebote);
			}
			else
			{

				poseAro1 = frenteAro1;
				poseAro2 = frenteAro2;
				poseAro3 = frenteAro3;
				poseAro4 = frenteAro4;
			}
		}


		offsetAro1 = -separacionOriginalAro1 + poseAro1;
		offsetAro2 = -separacionOriginalAro2 + poseAro2;
		offsetAro3 = -separacionOriginalAro3 + poseAro3;
		offsetAro4 = -separacionOriginalAro4 + poseAro4;


		glfwPollEvents();


		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());


		bool* keysLuces = mainWindow.getsKeys();
		if (keysLuces[GLFW_KEY_I])
		{
			if (teclaILiberada)
			{
				luzIncogniumActiva = !luzIncogniumActiva;
				printf("Luz Incognium: %s\n", luzIncogniumActiva ? "ENCENDIDA" : "APAGADA");
				teclaILiberada = false;
			}
		}
		else
		{
			teclaILiberada = true;
		}

		if (keysLuces[GLFW_KEY_F])
		{
			if (teclaFLiberada)
			{
				luzFarosActiva = !luzFarosActiva;
				printf("Luz faros y globo: %s\n", luzFarosActiva ? "ENCENDIDA" : "APAGADA");
				teclaFLiberada = false;
			}
		}
		else
		{
			teclaFLiberada = true;
		}



		bool* keysNave2 = mainWindow.getsKeys();

		if (false && keysNave2[GLFW_KEY_N])
		{
			if (teclaNLiberada)
			{
				posicionNave2 = nave2Inicio;
				rotacionNave2Y = nave2RotYInicio;
				escalaNave2 = nave2EscalaInicio;
				nave2Visible = true;
				flashNave2Activo = false;
				intensidadFlashNave2 = 0.0f;
				escalaFlashNave2 = 0.0f;
				faseAnimacionNave2 = 1;
				tiempoFaseNave2 = 0.0f;
				animacionNave2Activa = true;
				printf("Animacion Nave 2 original iniciada.\n");
				teclaNLiberada = false;
			}
		}
		else
		{
			teclaNLiberada = true;
		}

		if (keysNave2[GLFW_KEY_2])
		{
			if (tecla2LiberadaCopia)
			{
				posicionNave2Copia = nave2CopiaAparicion;
				rotacionNave2CopiaY = nave2CopiaRotY;
				escalaNave2Copia = nave2CopiaEscalaInicial;
				nave2CopiaVisible = false;
				flashNave2CopiaActivo = true;
				intensidadFlashNave2Copia = 1.0f;
				escalaFlashNave2Copia = 1.60f;
				faseAnimacionNave2Copia = 1;
				tiempoFaseNave2Copia = 0.0f;
				animacionNave2CopiaActiva = true;
				printf("Animacion copia Nave 2 iniciada con tecla 2.\n");
				tecla2LiberadaCopia = false;
			}
		}
		else
		{
			tecla2LiberadaCopia = true;
		}

		if (animacionNave2Activa)
		{

			tiempoFaseNave2 += frameTime;

			if (faseAnimacionNave2 == 1)
			{
				float t = SuavizarAnimacion(tiempoFaseNave2 / duracionTramoNave2_1);
				posicionNave2 = glm::mix(nave2Inicio, nave2Media, t);
				rotacionNave2Y = nave2RotYInicio + ((nave2RotYMedia - nave2RotYInicio) * t);

				escalaNave2 = nave2EscalaInicio;

				if (t >= 1.0f)
				{
					posicionNave2 = nave2Media;
					rotacionNave2Y = nave2RotYMedia;
					escalaNave2 = nave2EscalaMedia;
					faseAnimacionNave2 = 2;
					tiempoFaseNave2 = 0.0f;
				}
			}
			else if (faseAnimacionNave2 == 2)
			{
				float t = SuavizarAnimacion(tiempoFaseNave2 / duracionTramoNave2_2);
				posicionNave2 = glm::mix(nave2Media, nave2Final, t);
				rotacionNave2Y = nave2RotYMedia + ((nave2RotYFinal - nave2RotYMedia) * t);

				escalaNave2 = nave2EscalaFinal;

				if (t >= 1.0f)
				{
					posicionNave2 = nave2Final;
					rotacionNave2Y = nave2RotYFinal;
					escalaNave2 = nave2EscalaFinal;
					faseAnimacionNave2 = 3;
					tiempoFaseNave2 = 0.0f;
				}
			}
			else if (faseAnimacionNave2 == 3)
			{
				float t = SuavizarAnimacion(tiempoFaseNave2 / duracionTramoNave2_3);
				posicionNave2 = glm::mix(nave2Final, nave2Desaparicion, t);
				rotacionNave2Y = nave2RotYFinal;


				float tAchicar = SuavizarAnimacion((t - 0.78f) / 0.22f);
				escalaNave2 = nave2EscalaFinal + ((nave2EscalaDesaparicion - nave2EscalaFinal) * tAchicar);

				if (t >= 1.0f)
				{
					posicionNave2 = nave2Desaparicion;
					escalaNave2 = nave2EscalaDesaparicion;
					nave2Visible = false;
					flashNave2Activo = true;
					intensidadFlashNave2 = 1.0f;
					escalaFlashNave2 = 1.60f;
					faseAnimacionNave2 = 4;
					tiempoFaseNave2 = 0.0f;
				}
			}
			else if (faseAnimacionNave2 == 4)
			{
				float t = Clamp01(tiempoFaseNave2 / duracionFlashNave2);
				intensidadFlashNave2 = pow(1.0f - t, 0.85f);
				escalaFlashNave2 = 1.60f + (1.20f * t);

				if (t >= 1.0f)
				{
					intensidadFlashNave2 = 0.0f;
					escalaFlashNave2 = 0.0f;
					flashNave2Activo = false;
					faseAnimacionNave2 = 0;
					animacionNave2Activa = false;
					animacionUnidaArosNave2Activa = false;
				}
			}
		}

		if (animacionNave2CopiaActiva)
		{

			tiempoFaseNave2Copia += frameTime;

			if (faseAnimacionNave2Copia == 1)
			{
				float t = Clamp01(tiempoFaseNave2Copia / duracionFlashNave2Copia);
				intensidadFlashNave2Copia = pow(1.0f - t, 0.85f);
				escalaFlashNave2Copia = 1.60f + (1.20f * t);

				if (t >= 1.0f)
				{
					flashNave2CopiaActivo = false;
					nave2CopiaVisible = true;
					escalaNave2Copia = nave2CopiaEscalaInicial;
					faseAnimacionNave2Copia = 2;
					tiempoFaseNave2Copia = 0.0f;
				}
			}
			else if (faseAnimacionNave2Copia == 2)
			{
				float t = SuavizarAnimacion(tiempoFaseNave2Copia / duracionAparicionNave2Copia);
				posicionNave2Copia = nave2CopiaAparicion;
				rotacionNave2CopiaY = nave2CopiaRotY;
				escalaNave2Copia = nave2CopiaEscalaInicial + ((nave2CopiaEscalaCompleta - nave2CopiaEscalaInicial) * t);

				if (t >= 1.0f)
				{
					escalaNave2Copia = nave2CopiaEscalaCompleta;
					faseAnimacionNave2Copia = 3;
					tiempoFaseNave2Copia = 0.0f;
				}
			}
			else if (faseAnimacionNave2Copia == 3)
			{
				float t = SuavizarAnimacion(tiempoFaseNave2Copia / duracionCopiaTramo1);
				posicionNave2Copia = glm::mix(nave2CopiaAparicion, nave2CopiaTramo1, t);
				rotacionNave2CopiaY = nave2CopiaRotY;
				escalaNave2Copia = nave2CopiaEscalaCompleta;

				if (t >= 1.0f)
				{
					posicionNave2Copia = nave2CopiaTramo1;
					faseAnimacionNave2Copia = 4;
					tiempoFaseNave2Copia = 0.0f;
				}
			}
			else if (faseAnimacionNave2Copia == 4)
			{
				float t = SuavizarAnimacion(tiempoFaseNave2Copia / duracionCopiaTramo2);
				posicionNave2Copia = glm::mix(nave2CopiaTramo1, nave2CopiaTramo2, t);
				rotacionNave2CopiaY = nave2CopiaRotY;
				escalaNave2Copia = nave2CopiaEscalaCompleta;

				if (t >= 1.0f)
				{
					posicionNave2Copia = nave2CopiaTramo2;
					faseAnimacionNave2Copia = 5;
					tiempoFaseNave2Copia = 0.0f;
				}
			}
			else if (faseAnimacionNave2Copia == 5)
			{
				float t = SuavizarAnimacion(tiempoFaseNave2Copia / duracionCopiaTramo3);
				posicionNave2Copia = glm::mix(nave2CopiaTramo2, nave2CopiaFinal, t);
				rotacionNave2CopiaY = nave2CopiaRotY;
				escalaNave2Copia = nave2CopiaEscalaCompleta;

				if (t >= 1.0f)
				{
					posicionNave2Copia = nave2CopiaFinal;
					rotacionNave2CopiaY = nave2CopiaRotY;
					faseAnimacionNave2Copia = 6;
					tiempoFaseNave2Copia = 0.0f;
				}
			}
			else if (faseAnimacionNave2Copia == 6)
			{

				float t = SuavizarAnimacion(tiempoFaseNave2Copia / duracionGiroFinalNave2Copia);
				posicionNave2Copia = nave2CopiaFinal;
				escalaNave2Copia = nave2CopiaEscalaCompleta;
				rotacionNave2CopiaY = nave2CopiaRotY + ((nave2CopiaRotYFinal - nave2CopiaRotY) * t);

				if (t >= 1.0f)
				{
					rotacionNave2CopiaY = nave2CopiaRotYFinal;
					faseAnimacionNave2Copia = 0;
					animacionNave2CopiaActiva = false;
				}
			}
		}


		if (animacionGloboActiva)
		{
			tiempoAnimacionGlobo += frameTime;

			EstadoGloboSuave estadoGloboPrincipal = CalcularEstadoGloboSuave(tiempoAnimacionGlobo, centroRutaGloboPrincipal,
				39.0f * factorLocalEstacion, 48.0f * factorLocalEstacion, centroRutaGloboPrincipal.y, escalaGloboLocalBase, velocidadGloboPrincipal, desfaseGloboPrincipal,
				false, 2.25f * factorLocalEstacion, 3.5f * factorLocalEstacion, 2.5f * factorLocalEstacion);
			posicionGlobo = estadoGloboPrincipal.posicion;
			escalaGlobo = estadoGloboPrincipal.escala;
			rotacionGloboY = estadoGloboPrincipal.rotY;

			EstadoGloboSuave estadoGloboArriba = CalcularEstadoGloboSuave(tiempoAnimacionGlobo, centroRutaGloboArriba,
				45.0f * factorLocalEstacion, 41.0f * factorLocalEstacion, centroRutaGloboArriba.y, escalaGloboLocalBase, velocidadGloboArriba, desfaseGloboArriba,
				true, 2.8f * factorLocalEstacion, 4.0f * factorLocalEstacion, 3.0f * factorLocalEstacion);
			posicionGloboArriba = estadoGloboArriba.posicion;
			escalaGloboArriba = estadoGloboArriba.escala;
			rotacionGloboArribaY = estadoGloboArriba.rotY;

			EstadoGloboSuave estadoGloboAbajo = CalcularEstadoGloboSuave(tiempoAnimacionGlobo, centroRutaGloboAbajo,
				28.0f * factorLocalEstacion, 34.0f * factorLocalEstacion, centroRutaGloboAbajo.y, escalaGloboLocalBase, velocidadGloboAbajo, desfaseGloboAbajo,
				false, 1.85f * factorLocalEstacion, 2.2f * factorLocalEstacion, 1.7f * factorLocalEstacion);
			posicionGloboAbajo = estadoGloboAbajo.posicion;
			escalaGloboAbajo = estadoGloboAbajo.escala;
			rotacionGloboAbajoY = estadoGloboAbajo.rotY;
		}


		// Raiz general del escenario: todas las jerarquias principales salen de aqui.
		glm::mat4 jerarquiaEscena = glm::mat4(1.0f);
		jerarquiaEscena = glm::translate(jerarquiaEscena, ajusteJerarquiaEscena);
		jerarquiaEscena = glm::scale(jerarquiaEscena, glm::vec3(escalaJerarquiaEscena, escalaJerarquiaEscena, escalaJerarquiaEscena));

		glm::mat4 jerarquiaPueblo = jerarquiaEscena;
		jerarquiaPueblo = glm::translate(jerarquiaPueblo, posicionGrupoPueblo);
		jerarquiaPueblo = glm::rotate(jerarquiaPueblo, rotacionGrupoPuebloY * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		jerarquiaPueblo = glm::scale(jerarquiaPueblo, glm::vec3(escalaGrupoPueblo, escalaGrupoPueblo, escalaGrupoPueblo));


		glm::mat4 jerarquiaEstacion = jerarquiaPueblo;
		jerarquiaEstacion = glm::translate(jerarquiaEstacion, posicionEstacion + ajusteJerarquiaEstacionFija);
		jerarquiaEstacion = glm::rotate(jerarquiaEstacion, rotacionEstacionY * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		jerarquiaEstacion = glm::scale(jerarquiaEstacion, glm::vec3(escalaEstacion, escalaEstacion, escalaEstacion));


		glm::mat4 jerarquiaIncognium = jerarquiaPueblo;
		jerarquiaIncognium = glm::translate(jerarquiaIncognium, posicionGrupoIncognium);
		jerarquiaIncognium = glm::scale(jerarquiaIncognium, glm::vec3(escalaGrupoIncognium, escalaGrupoIncognium, escalaGrupoIncognium));

		glm::vec3 posicionLuzIncognium = glm::vec3(jerarquiaIncognium * glm::vec4(pivoteMalla, 1.0f));
		float intensidadIncogniumManual = luzIncogniumActiva ? 1.0f : 0.0f;
		pointLights[1] = PointLight(0.0f, 0.45f, 1.0f,
			0.35f * intensidadIncogniumManual, 1.4f * intensidadIncogniumManual,
			posicionLuzIncognium.x, posicionLuzIncognium.y, posicionLuzIncognium.z,
			1.0f, 0.010f, 0.0010f);


		// Luz azul del globo: unida mediante jerarquia al globo principal.
		// Se controla con la misma tecla F de las luces del pueblo.
		glm::vec3 posicionLuzGloboLocal = glm::vec3(0.0f, 0.0f, 0.0f);
		glm::mat4 jerarquiaGloboPrincipal = jerarquiaEstacion;
		jerarquiaGloboPrincipal = glm::translate(jerarquiaGloboPrincipal, posicionGlobo);
		jerarquiaGloboPrincipal = glm::rotate(jerarquiaGloboPrincipal, rotacionGloboY * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		jerarquiaGloboPrincipal = glm::scale(jerarquiaGloboPrincipal, glm::vec3(escalaGlobo, escalaGlobo, escalaGlobo));

		glm::mat4 jerarquiaGloboArriba = jerarquiaEstacion;
		jerarquiaGloboArriba = glm::translate(jerarquiaGloboArriba, posicionGloboArriba);
		jerarquiaGloboArriba = glm::rotate(jerarquiaGloboArriba, rotacionGloboArribaY * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		jerarquiaGloboArriba = glm::scale(jerarquiaGloboArriba, glm::vec3(escalaGloboArriba, escalaGloboArriba, escalaGloboArriba));

		glm::mat4 jerarquiaGloboAbajo = jerarquiaEstacion;
		jerarquiaGloboAbajo = glm::translate(jerarquiaGloboAbajo, posicionGloboAbajo);
		jerarquiaGloboAbajo = glm::rotate(jerarquiaGloboAbajo, rotacionGloboAbajoY * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		jerarquiaGloboAbajo = glm::scale(jerarquiaGloboAbajo, glm::vec3(escalaGloboAbajo, escalaGloboAbajo, escalaGloboAbajo));

		glm::vec3 posicionLuzGlobo = glm::vec3(jerarquiaGloboPrincipal * glm::vec4(posicionLuzGloboLocal, 1.0f));

		float intensidadLuzGlobo = luzFarosActiva ? 1.0f : 0.0f;

		// Relleno azul difuso alrededor del globo, sin spotlight circular ni luces raras en el origen.
		pointLights[0] = PointLight(0.0f, 0.55f, 1.0f,
			0.08f * intensidadLuzGlobo, 1.15f * intensidadLuzGlobo,
			posicionLuzGlobo.x, posicionLuzGlobo.y, posicionLuzGlobo.z,
			1.0f, 0.0010f, 0.000006f);


		glm::mat4 jerarquiaFaros[4];
		for (int i = 0; i < 4; i++)
		{
			jerarquiaFaros[i] = jerarquiaPueblo;
			jerarquiaFaros[i] = glm::translate(jerarquiaFaros[i], posicionFaros[i]);
			jerarquiaFaros[i] = glm::scale(jerarquiaFaros[i], glm::vec3(escalaFaro, escalaFaro, escalaFaro));
		}


		glm::vec3 posicionLuzFarosLocal = glm::vec3(
			(posicionFaros[0].x + posicionFaros[1].x + posicionFaros[2].x + posicionFaros[3].x) / 4.0f,
			45.0f,
			(posicionFaros[0].z + posicionFaros[1].z + posicionFaros[2].z + posicionFaros[3].z) / 4.0f
		);
		glm::vec3 posicionLuzFaros = glm::vec3(jerarquiaPueblo * glm::vec4(posicionLuzFarosLocal, 1.0f));

		float intensidadFaro = luzFarosActiva ? intensidadNoche : 0.0f;
		if (intensidadFaro < 0.12f)
			intensidadFaro = 0.0f;


		pointLights[2] = PointLight(1.0f, 0.86f, 0.55f,
			0.055f * intensidadFaro, 1.05f * intensidadFaro,
			posicionLuzFaros.x, posicionLuzFaros.y, posicionLuzFaros.z,
			1.0f, 0.00025f, 0.0000025f);


		glClearColor(0.02f * intensidadDia, 0.04f * intensidadDia, 0.08f + (0.22f * intensidadDia), 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		float intensidadSkybox = 0.55f + (0.88f * intensidadDia);
		glm::vec3 tinteSkybox = glm::vec3(
			0.18f + (0.82f * intensidadDia),
			0.22f + (0.78f * intensidadDia),
			0.38f + (0.62f * intensidadDia)
		);
		skybox.DrawSkybox(camera.calculateViewMatrix(), projection, intensidadSkybox, tinteSkybox);
		shaderList[0].UseShader();
		uniformModel = shaderList[0].GetModelLocation();
		uniformProjection = shaderList[0].GetProjectionLocation();
		uniformView = shaderList[0].GetViewLocation();
		uniformEyePosition = shaderList[0].GetEyePositionLocation();
		uniformColor = shaderList[0].getColorLocation();
		uniformTextureOffset = shaderList[0].getOffsetLocation();


		uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
		uniformShininess = shaderList[0].GetShininessLocation();

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);


		lowerLight = camera.getCameraPosition();
		lowerLight.y -= 0.3f;
		spotLights[0].SetFlash(lowerLight, camera.getCameraDirection());


		shaderList[0].SetDirectionalLight(&mainLight);
		shaderList[0].SetPointLights(pointLights, pointLightCount);
		shaderList[0].SetSpotLights(spotLights, spotLightCount);


		model = glm::mat4(1.0);
		modelaux = glm::mat4(1.0);
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		toffset = glm::vec2(0.0f, 0.0f);
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
		model = glm::scale(model, glm::vec3(30.0f, 1.0f, 30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		pisoTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[2]->RenderMesh();


		model = jerarquiaPueblo;
		model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
		model = glm::rotate(model, 0.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		toffset = glm::vec2(0.0f, 0.0f);
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Pueblo_M.RenderModel();


		model = jerarquiaEstacion;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		toffset = glm::vec2(0.0f, 0.0f);
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Estacion_M.RenderModel();


		glm::mat4 jerarquiaMaquinarr = jerarquiaEscena;
		jerarquiaMaquinarr = glm::translate(jerarquiaMaquinarr, posicionMaquinarr);
		jerarquiaMaquinarr = glm::rotate(jerarquiaMaquinarr, rotacionMaquinarrY * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		jerarquiaMaquinarr = glm::scale(jerarquiaMaquinarr, glm::vec3(escalaMaquinarr, escalaMaquinarr, escalaMaquinarr));

		float escalaCompensacionMaquinarr = 1.0f / escalaMaquinarr;


		model = jerarquiaMaquinarr;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		toffset = glm::vec2(0.0f, 0.0f);
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Maquinarr_M.RenderModel();


		glm::mat4 matrizAros = jerarquiaMaquinarr;
		matrizAros = glm::translate(matrizAros, posicionGrupoAros);
		matrizAros = glm::rotate(matrizAros, rotacionGrupoArosY * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		matrizAros = glm::scale(matrizAros, glm::vec3(escalaGrupoAros * escalaCompensacionMaquinarr, escalaGrupoAros * escalaCompensacionMaquinarr, escalaGrupoAros * escalaCompensacionMaquinarr));

		modelaux = matrizAros;


		model = modelaux;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		toffset = glm::vec2(0.0f, 0.0f);
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		BaseAro_M.RenderModel();


		model = modelaux;
		model = glm::translate(model, ejeSalidaAros * offsetAro1);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Aro1_M.RenderModel();

		model = modelaux;
		model = glm::translate(model, ejeSalidaAros * offsetAro2);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Aro2_M.RenderModel();

		model = modelaux;
		model = glm::translate(model, ejeSalidaAros * offsetAro3);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Aro3_M.RenderModel();

		model = modelaux;
		model = glm::translate(model, ejeSalidaAros * offsetAro4);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Aro4_M.RenderModel();


		float offsetAro1Copia = -separacionOriginalAro1 + frenteAro1;
		float offsetAro2Copia = -separacionOriginalAro2 + frenteAro2;
		float offsetAro3Copia = -separacionOriginalAro3 + frenteAro3;
		float offsetAro4Copia = -separacionOriginalAro4 + frenteAro4;

		glm::mat4 matrizArosCopia = jerarquiaMaquinarr;
		matrizArosCopia = glm::translate(matrizArosCopia, posicionGrupoArosCopia);
		matrizArosCopia = glm::rotate(matrizArosCopia, rotacionGrupoArosCopiaY * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		matrizArosCopia = glm::scale(matrizArosCopia, glm::vec3(escalaGrupoArosCopia * escalaCompensacionMaquinarr, escalaGrupoArosCopia * escalaCompensacionMaquinarr, escalaGrupoArosCopia * escalaCompensacionMaquinarr));

		modelaux = matrizArosCopia;

		model = modelaux;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		toffset = glm::vec2(0.0f, 0.0f);
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		BaseAro_M.RenderModel();

		model = modelaux;
		model = glm::translate(model, ejeSalidaAros * offsetAro1Copia);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Aro1_M.RenderModel();

		model = modelaux;
		model = glm::translate(model, ejeSalidaAros * offsetAro2Copia);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Aro2_M.RenderModel();

		model = modelaux;
		model = glm::translate(model, ejeSalidaAros * offsetAro3Copia);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Aro3_M.RenderModel();

		model = modelaux;
		model = glm::translate(model, ejeSalidaAros * offsetAro4Copia);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Aro4_M.RenderModel();


		if (nave2Visible)
		{
			model = jerarquiaMaquinarr;
			model = glm::translate(model, posicionNave2);
			model = glm::rotate(model, rotacionNave2Y * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
			model = glm::scale(model, glm::vec3(escalaNave2 * escalaCompensacionMaquinarr, escalaNave2 * escalaCompensacionMaquinarr, escalaNave2 * escalaCompensacionMaquinarr));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			color = glm::vec3(1.0f, 1.0f, 1.0f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			toffset = glm::vec2(0.0f, 0.0f);
			glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
			Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
			Nave2_M.RenderModel();
		}

		if (flashNave2Activo && intensidadFlashNave2 > 0.0f)
		{
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE);
			glDepthMask(GL_FALSE);
			plainTexture.UseTexture();
			Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);


			glm::vec3 colorFlash = glm::vec3(2.4f, 2.1f, 0.75f) * intensidadFlashNave2;
			glUniform3fv(uniformColor, 1, glm::value_ptr(colorFlash));
			toffset = glm::vec2(0.0f, 0.0f);
			glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));

			model = jerarquiaMaquinarr;
			model = glm::translate(model, nave2Desaparicion);
			model = glm::scale(model, glm::vec3(escalaFlashNave2 * escalaCompensacionMaquinarr, escalaFlashNave2 * escalaCompensacionMaquinarr, escalaFlashNave2 * escalaCompensacionMaquinarr));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			meshList[0]->RenderMesh();

			model = jerarquiaMaquinarr;
			model = glm::translate(model, nave2Desaparicion);
			model = glm::rotate(model, 90.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
			model = glm::scale(model, glm::vec3(escalaFlashNave2 * 0.70f * escalaCompensacionMaquinarr, escalaFlashNave2 * 0.70f * escalaCompensacionMaquinarr, escalaFlashNave2 * 0.70f * escalaCompensacionMaquinarr));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			meshList[0]->RenderMesh();

			glDepthMask(GL_TRUE);
			glDisable(GL_BLEND);
		}


		if (nave2CopiaVisible)
		{
			model = jerarquiaMaquinarr;
			model = glm::translate(model, posicionNave2Copia);
			model = glm::rotate(model, rotacionNave2CopiaY * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
			model = glm::scale(model, glm::vec3(escalaNave2Copia * escalaCompensacionMaquinarr, escalaNave2Copia * escalaCompensacionMaquinarr, escalaNave2Copia * escalaCompensacionMaquinarr));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			color = glm::vec3(1.0f, 1.0f, 1.0f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			toffset = glm::vec2(0.0f, 0.0f);
			glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
			Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
			Nave2_M.RenderModel();
		}

		if (flashNave2CopiaActivo && intensidadFlashNave2Copia > 0.0f)
		{
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE);
			glDepthMask(GL_FALSE);
			plainTexture.UseTexture();
			Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);

			glm::vec3 colorFlashCopia = glm::vec3(2.4f, 2.1f, 0.75f) * intensidadFlashNave2Copia;
			glUniform3fv(uniformColor, 1, glm::value_ptr(colorFlashCopia));
			toffset = glm::vec2(0.0f, 0.0f);
			glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));

			model = jerarquiaMaquinarr;
			model = glm::translate(model, nave2CopiaAparicion);
			model = glm::scale(model, glm::vec3(escalaFlashNave2Copia * escalaCompensacionMaquinarr, escalaFlashNave2Copia * escalaCompensacionMaquinarr, escalaFlashNave2Copia * escalaCompensacionMaquinarr));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			meshList[0]->RenderMesh();

			model = jerarquiaMaquinarr;
			model = glm::translate(model, nave2CopiaAparicion);
			model = glm::rotate(model, 90.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
			model = glm::scale(model, glm::vec3(escalaFlashNave2Copia * 0.70f * escalaCompensacionMaquinarr, escalaFlashNave2Copia * 0.70f * escalaCompensacionMaquinarr, escalaFlashNave2Copia * 0.70f * escalaCompensacionMaquinarr));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			meshList[0]->RenderMesh();

			glDepthMask(GL_TRUE);
			glDisable(GL_BLEND);
		}


		model = jerarquiaGloboPrincipal;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		toffset = glm::vec2(0.0f, 0.0f);
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Globo_M.RenderModel();


		model = jerarquiaGloboArriba;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		toffset = glm::vec2(0.0f, 0.0f);
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Globo_M.RenderModel();


		model = jerarquiaGloboAbajo;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		toffset = glm::vec2(0.0f, 0.0f);
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Globo_M.RenderModel();


		if (intensidadLuzGlobo > 0.0f)
		{
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE);
			glDepthMask(GL_FALSE);
			plainTexture.UseTexture();
			Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);

			model = jerarquiaGloboPrincipal;
			model = glm::translate(model, posicionLuzGloboLocal);
			model = glm::scale(model, glm::vec3(0.18f, 1.0f, 0.18f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			color = glm::vec3(0.0f, 1.25f, 3.0f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			toffset = glm::vec2(0.0f, 0.0f);
			glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
			meshList[5]->RenderMesh();

			model = jerarquiaGloboPrincipal;
			model = glm::translate(model, posicionLuzGloboLocal - glm::vec3(0.0f, 0.015f, 0.0f));
			model = glm::scale(model, glm::vec3(0.28f, 1.0f, 0.28f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			color = glm::vec3(0.0f, 0.45f, 1.35f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
			meshList[5]->RenderMesh();

			glDepthMask(GL_TRUE);
			glDisable(GL_BLEND);
		}


		model = jerarquiaPueblo;
		model = glm::translate(model, posicionPuenteProgreso);
		model = glm::rotate(model, rotacionPuenteProgresoY * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(escalaPuenteProgreso, escalaPuenteProgreso, escalaPuenteProgreso));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		toffset = glm::vec2(0.0f, 0.0f);
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		PuenteProgreso_M.RenderModel();


		modelaux = jerarquiaIncognium;


		model = modelaux;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		toffset = glm::vec2(0.0f, 0.0f);
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		BaseYBolaAzul_M.RenderModel();


		model = modelaux;
		model = glm::translate(model, pivoteMalla);
		model = glm::rotate(model, rotMallaX * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, rotMallaY * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, rotMallaZ * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::translate(model, -pivoteMalla);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		toffset = glm::vec2(0.0f, 0.0f);
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		MallaCircular_M.RenderModel();


		for (int i = 0; i < 4; i++)
		{
			model = jerarquiaFaros[i];
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			color = glm::vec3(1.0f, 1.0f, 1.0f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			toffset = glm::vec2(0.0f, 0.0f);
			glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
			Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
			Faro_M.RenderModel();
		}


		if (intensidadFaro > 0.0f)
		{
			for (int i = 0; i < 4; i++)
			{

				model = jerarquiaFaros[i];
				model = glm::translate(model, posicionLocalAuraFaro);
				model = glm::rotate(model, 90.0f * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
				model = glm::scale(model, glm::vec3(1.10f, 1.0f, 1.10f));
				glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
				color = glm::vec3(2.4f, 2.0f, 0.55f);
				glUniform3fv(uniformColor, 1, glm::value_ptr(color));
				toffset = glm::vec2(0.0f, 0.0f);
				glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
				plainTexture.UseTexture();
				Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
				meshList[5]->RenderMesh();


				model = jerarquiaFaros[i];
				model = glm::translate(model, posicionLocalAuraFaro);
				model = glm::rotate(model, 90.0f * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
				model = glm::rotate(model, 90.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
				model = glm::scale(model, glm::vec3(1.10f, 1.0f, 1.10f));
				glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
				color = glm::vec3(2.6f, 2.15f, 0.70f);
				glUniform3fv(uniformColor, 1, glm::value_ptr(color));
				toffset = glm::vec2(0.0f, 0.0f);
				glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
				plainTexture.UseTexture();
				Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
				meshList[5]->RenderMesh();


				model = jerarquiaFaros[i];
				model = glm::translate(model, posicionLocalCristalFaro);
				model = glm::rotate(model, 90.0f * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
				model = glm::scale(model, glm::vec3(0.58f, 1.0f, 0.58f));
				glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
				color = glm::vec3(6.0f, 5.2f, 1.55f);
				glUniform3fv(uniformColor, 1, glm::value_ptr(color));
				toffset = glm::vec2(0.0f, 0.0f);
				glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
				plainTexture.UseTexture();
				Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
				meshList[5]->RenderMesh();


				model = jerarquiaFaros[i];
				model = glm::translate(model, posicionLocalCristalFaro);
				model = glm::rotate(model, 90.0f * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
				model = glm::rotate(model, 90.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
				model = glm::scale(model, glm::vec3(0.58f, 1.0f, 0.58f));
				glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
				color = glm::vec3(6.5f, 5.7f, 1.85f);
				glUniform3fv(uniformColor, 1, glm::value_ptr(color));
				toffset = glm::vec2(0.0f, 0.0f);
				glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
				plainTexture.UseTexture();
				Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
				meshList[5]->RenderMesh();


				model = jerarquiaFaros[i];
				model = glm::translate(model, posicionLocalCristalFaro);
				model = glm::scale(model, glm::vec3(0.55f, 1.0f, 0.55f));
				glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
				color = glm::vec3(7.0f, 6.4f, 2.15f);
				glUniform3fv(uniformColor, 1, glm::value_ptr(color));
				toffset = glm::vec2(0.0f, 0.0f);
				glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
				plainTexture.UseTexture();
				Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
				meshList[5]->RenderMesh();
			}
		}
		glUseProgram(0);

		mainWindow.swapBuffers();
	}

	return 0;
}
