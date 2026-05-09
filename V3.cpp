/*
Animación:
- Simple o básica:Por banderas y condicionales (más de 1 transformación geométrica se ve modificada)
-Compleja: Por medio de funciones y algoritmos.
-Textura Animada
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
//para probar el importer
//#include<assimp/Importer.hpp>

#include "Window.h"
#include "Mesh.h"
#include "Shader_light.h"
#include "Camera.h"
#include "Texture.h"
#include "Sphere.h"
#include"Model.h"
#include "Skybox.h"

//para iluminación
#include "CommonValues.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "Material.h"
const float toRadians = 3.14159265f / 180.0f;

//variables para animación
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

//variables para animación de la nave
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

//variables para animación de la malla circular
float rotMallaX;
float rotMallaY;
float rotMallaZ;
float rotMallaXOffset;
float rotMallaYOffset;
float rotMallaZOffset;

//variables para sistema de día y noche
float timeOfDay;
float dayNightSpeed;
float intensidadDia;
float intensidadNoche;

// Estado visual de cada globo durante la animacion continua.
// La trayectoria se calcula con funciones seno/coseno para que no dependa
// de puntos guardados ni cambios bruscos entre posiciones.
struct EstadoGloboSuave
{
	glm::vec3 posicion;
	float escala;
	float rotY;
};

// Punto de una ruta ovalada con una pequena variacion senoidal.
// Esto hace que la nave no se vea robotica ni como si siguiera puntos fijos.
glm::vec3 PuntoRutaGloboSuave(float angulo, glm::vec3 centro, float radioX, float radioZ,
	float alturaBase, float amplitudAltura, float ondaX, float ondaZ)
{
	glm::vec3 p;
	p.x = centro.x + (cos(angulo) * radioX) + (sin(angulo * 2.0f) * ondaX);
	p.z = centro.z + (sin(angulo) * radioZ) + (cos(angulo * 1.5f) * ondaZ);
	p.y = alturaBase + (sin(angulo * 1.7f) * amplitudAltura);
	return p;
}

// Calcula posicion, escala y orientacion con una ruta continua.
// La orientacion sale de un punto ligeramente adelantado en la misma curva,
// por eso la nave siempre apunta hacia donde va.
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

// Modelos de planetas.
Model PlanetaMorado001_M;
Model PlanetaAmarillo_M;
Model PlanetaAzul_M;
Model PlanetaGris_M;
Model PlanetaMorado_M;
Model PlanetaVerde_M;

// Modelo rosa independiente para posicionarlo con menu.
Model Rosa_M;

// Piezas separadas del Principito.
// Cada OBJ se renderiza como hijo de jerarquiaPrincipito.
Model PrincipitoBrazoDer_M;
Model PrincipitoBrazoIzq_M;
Model PrincipitoCabeza_M;
Model PrincipitoPiernaDer_M;
Model PrincipitoPiernaIzq_M;
Model PrincipitoTorso_M;

// Piezas separadas de Blitzcrank.
// Cada OBJ debe tener su origen/pivote correcto desde Blender.
Model BlitzBrazoDer_M;
Model BlitzBrazoIzq_M;
Model BlitzHombroIzq_M;
Model BlitzHombroDer_M;
Model BlitzPieDer_M;
Model BlitzPieIzq_M;
Model BlitzPiernaDer_M;
Model BlitzPiernaIzq_M;
Model BlitzTorso_M;

Skybox skybox;

//materiales
Material Material_brillante;
Material Material_opaco;


//Sphere cabeza = Sphere(0.5, 20, 20);
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;

// luz direccional
DirectionalLight mainLight;
//para declarar varias luces de tipo pointlight
PointLight pointLights[MAX_POINT_LIGHTS];
SpotLight spotLights[MAX_SPOT_LIGHTS];

// Vertex Shader
static const char* vShader = "shaders/shader_light.vert";

// Fragment Shader
static const char* fShader = "shaders/shader_light.frag";



//cálculo del promedio de las normales para sombreado de Phong
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
	meshList.push_back(obj6); // todos los números

	Mesh* obj7 = new Mesh();
	obj7->CreateMesh(numeroVertices, numeroIndices, 32, 6);
	meshList.push_back(obj7); // solo un número

}


void CreateShaders()
{
	Shader* shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}




int main()
{
	mainWindow = Window(1366, 768); // 1280, 1024 or 1024, 768
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

	// Planetas.
	// Estos modelos se agrupan abajo en jerarquiaPlanetas para moverlos juntos con el menu de teclas.
	PlanetaMorado001_M = Model();
	PlanetaMorado001_M.LoadModel("Models/morado_001.obj");

	PlanetaAmarillo_M = Model();
	PlanetaAmarillo_M.LoadModel("Models/amarillo.obj");

	PlanetaAzul_M = Model();
	PlanetaAzul_M.LoadModel("Models/azul.obj");

	PlanetaGris_M = Model();
	PlanetaGris_M.LoadModel("Models/gris.obj");

	PlanetaMorado_M = Model();
	PlanetaMorado_M.LoadModel("Models/morado.obj");


	PlanetaVerde_M = Model();
	PlanetaVerde_M.LoadModel("Models/verde.obj");

	// Rosa independiente.
	// Usa el mismo archivo rosa.obj, pero con su propia jerarquia para acomodarla aparte.
	Rosa_M = Model();
	Rosa_M.LoadModel("Models/rosa.obj");

	// Principito separado por piezas.
	// Coloca los .obj y .mtl en la carpeta Models con estos mismos nombres.
	PrincipitoBrazoDer_M = Model();
	PrincipitoBrazoDer_M.LoadModel("Models/brazoderprinc.obj");

	PrincipitoBrazoIzq_M = Model();
	PrincipitoBrazoIzq_M.LoadModel("Models/brazoizqder.obj");

	PrincipitoCabeza_M = Model();
	PrincipitoCabeza_M.LoadModel("Models/cabezaprinc.obj");

	PrincipitoPiernaDer_M = Model();
	PrincipitoPiernaDer_M.LoadModel("Models/piernaderprinc.obj");

	PrincipitoPiernaIzq_M = Model();
	PrincipitoPiernaIzq_M.LoadModel("Models/piernaizqprinc.obj");

	PrincipitoTorso_M = Model();
	PrincipitoTorso_M.LoadModel("Models/torsoprincipito.obj");

	// Blitzcrank separado por piezas.
	// Coloca los .obj y .mtl en la carpeta Models con estos mismos nombres.
	BlitzBrazoDer_M = Model();
	BlitzBrazoDer_M.LoadModel("Models/bderblitz.obj");

	BlitzBrazoIzq_M = Model();
	BlitzBrazoIzq_M.LoadModel("Models/bizblitz.obj");

	BlitzHombroIzq_M = Model();
	BlitzHombroIzq_M.LoadModel("Models/hombizqblitz.obj");

	BlitzHombroDer_M = Model();
	BlitzHombroDer_M.LoadModel("Models/homderblitz.obj");

	BlitzPieDer_M = Model();
	BlitzPieDer_M.LoadModel("Models/piederblitz.obj");

	BlitzPieIzq_M = Model();
	BlitzPieIzq_M.LoadModel("Models/pieizqblitz.obj");

	BlitzPiernaDer_M = Model();
	BlitzPiernaDer_M.LoadModel("Models/piernderblitz.obj");

	BlitzPiernaIzq_M = Model();
	BlitzPiernaIzq_M.LoadModel("Models/piernizqblitz.obj");

	BlitzTorso_M = Model();
	BlitzTorso_M.LoadModel("Models/torsoblitz.obj");


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


	//luz direccional, sólo 1 y siempre debe de existir
	mainLight = DirectionalLight(1.0f, 1.0f, 1.0f,
		0.5f, 0.5f,
		0.0f, -1.0f, -1.0f);
	//contador de luces puntuales
	unsigned int pointLightCount = 0;
	// Luz azul de la estacion del aire.
	// Se declara aqui, pero su posicion real se actualiza dentro del ciclo
	// usando jerarquiaEstacion para que se vaya junto con la estacion.
	pointLights[0] = PointLight(0.0f, 0.45f, 1.0f,
		0.0f, 0.0f,
		0.0f, 0.0f, 0.0f,
		1.0f, 0.025f, 0.0035f);
	pointLightCount++;

	//Luz azul del Incognium de Zindelo
	//Se declara aquí, pero su posición se actualizará dentro del ciclo
	//usando la misma jerarquía de la malla circular.
	pointLights[1] = PointLight(0.0f, 0.35f, 1.0f,
		0.7f, 2.8f,
		0.0f, 0.0f, 0.0f,
		1.0f, 0.025f, 0.006f);
	pointLightCount++;

	//Luz del faro.
	//Se mantiene declarada y dentro del ciclo se actualiza para que
	//se encienda gradualmente cuando sea de noche.
	pointLights[2] = PointLight(1.0f, 0.82f, 0.45f,
		0.0f, 0.0f,
		0.0f, 0.0f, 0.0f,
		1.0f, 0.035f, 0.008f);
	pointLightCount++;

	unsigned int spotLightCount = 0;
	//linterna
	spotLights[0] = SpotLight(1.0f, 1.0f, 1.0f,
		0.0f, 2.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		5.0f);
	spotLightCount++;

	//luz fija
	spotLights[1] = SpotLight(0.0f, 0.0f, 1.0f,
		1.0f, 2.0f,
		5.0f, 10.0f, 0.0f,
		0.0f, -5.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		15.0f);
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

	// Control manual de luces.
	// I prende/apaga la luz azul del Incognium.
	// F prende/apaga la luz compartida de los cuatro faros.
	bool luzIncogniumActiva = true;
	bool luzFarosActiva = true;
	bool luzEstacionActiva = true;
	bool teclaILiberada = true;
	bool teclaFLiberada = true;

	// Luz azul de la estacion.
	// Es local a jerarquiaEstacion para que no se despegue si se mueve jerarquiaAireEstacion.
	glm::vec3 posicionLocalLuzEstacion = glm::vec3(0.0f, 2.610f, 0.0f);
	glm::vec3 posicionLocalAuraEstacion = glm::vec3(0.0f, 2.610f, 0.0f);
	float escalaAuraEstacion = 1.0f;
	float rotacionAuraEstacionY = -98.75f;
	float velocidadMoverAuraEstacion = 0.035f;
	float velocidadAlturaAuraEstacion = 0.035f;
	float velocidadEscalaAuraEstacion = 0.035f;
	float escalaMinimaAuraEstacion = 0.10f;
	float escalaMaximaAuraEstacion = 8.0f;
	float velocidadRotacionAuraEstacion = 2.5f;
	float tiempoImpresionAuraEstacion = 0.0f;

	glm::vec3 lowerLight(0.0f, 0.0f, 0.0f);

	glm::mat4 model(1.0);
	glm::mat4 modelaux(1.0);
	glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
	glm::vec2 toffset = glm::vec2(0.0f, 0.0f);
	bool datosInicialesGrupoMostrados = false;

	// Blitzcrank.
	// Ahora es hijo directo de jerarquiaArcane, no de jerarquiaPueblo.
	// Estos valores son locales a la tematica Arcane para conservar el acomodo visual original.
	glm::vec3 posicionBlitz = glm::vec3(24.97f, -0.80f, 25.00f);
	float rotacionBlitzY = 2791.83f;
	float escalaBlitz = 0.0125f;
	float velocidadMoverBlitz = 2.5f;
	float velocidadAlturaBlitz = 1.5f;
	float velocidadEscalaBlitz = 0.010f;
	float escalaMinimaBlitz = 0.001f;
	float escalaMaximaBlitz = 1.0f;
	float velocidadRotacionBlitz = 35.0f;
	float tiempoImpresionBlitz = 0.0f;

	// Movimiento visual simple de Blitz para que no se vea congelado.
	float velocidadPasoBlitz = 7.5f;
	float amplitudPasoBrazoBlitz = 8.0f;
	float amplitudPasoPiernaBlitz = 6.0f;

	// Jerarquia Arcane.
	// Es el padre general de la tematica Arcane: pueblo, Blitz y Maquinarr.
	// Se queda con la transformacion que antes tenia el grupo del pueblo para conservar la posicion visual.
	glm::vec3 posicionArcane = glm::vec3(-197.77f, -0.38f, 66.08f);
	float rotacionArcaneY = -360.35f;
	float escalaArcane = 2.09f;

	//Jerarquia general del pueblo.
	//Ahora solo contiene los elementos del pueblo: Pueblo_M, Incognium, 4 faros y Puente del Progreso.
	//Blitz y Maquinarr ya no son hijos de esta jerarquia.
	glm::vec3 posicionGrupoPueblo = glm::vec3(0.0f, 0.0f, 0.0f);
	float rotacionGrupoPuebloY = 0.0f;
	float escalaGrupoPueblo = 1.0f;

	// Modo editor para mover TODO el conjunto ya unido por jerarquia.
	// Flechas: izquierda/derecha mueven X, arriba/abajo mueven Z.
	// Q y E cambian la altura.
	// + y - cambian el tamaño general.
	// R y T rotan todo el conjunto en Y.
	float velocidadMoverGrupoPueblo = 2.5f;
	float velocidadAlturaGrupoPueblo = 1.5f;
	float velocidadEscalaGrupoPueblo = 0.25f;
	float escalaMinimaGrupoPueblo = 0.20f;
	float escalaMaximaGrupoPueblo = 8.0f;
	float velocidadRotacionGrupoPueblo = 35.0f;
	float tiempoImpresionGrupoPueblo = 0.0f;

	//Datos del grupo del Incognium.
	//Se dejan en variables para que la jerarquía y la luz usen la misma referencia.
	glm::vec3 posicionGrupoIncognium = glm::vec3(-0.04f, -1.47f, 0.14f);
	float escalaGrupoIncognium = 81.74f;
	glm::vec3 pivoteMalla = glm::vec3(-0.000776f, 0.12575f, 0.0f);

	// Modo editor para acomodar el Incognium con el teclado.
	// Flechas: izquierda/derecha mueven X, arriba/abajo mueven Z.
	// + y - cambian el tamaño.
	// Q y E cambian la altura.
	float velocidadMoverIncognium = 1.2f;
	float velocidadAlturaIncognium = 1.0f;
	float velocidadEscalaIncognium = 8.0f;
	float escalaMinimaIncognium = 5.0f;
	float escalaMaximaIncognium = 120.0f;
	float tiempoImpresionIncognium = 0.0f;

	//Faros del pueblo.
	//El primer faro es el que se acomodó manualmente.
	//Los otros tres se colocan como espejo en X y Z para formar un cuadrado alrededor del Incognium.
	glm::vec3 posicionFaro = glm::vec3(-26.37f, -2.00f, 26.95f);
	glm::vec3 posicionFaros[4] = {
		glm::vec3(-26.37f, -2.00f, 26.95f),
		glm::vec3(26.37f, -2.00f, 26.95f),
		glm::vec3(-26.37f, -2.00f, -26.95f),
		glm::vec3(26.37f, -2.00f, -26.95f)
	};
	float escalaFaro = 1.17f;
	glm::vec3 posicionLocalLuzFaro = glm::vec3(0.0f, 9.15f, 0.0f);
	// Cristal visual del faro: va mas abajo que la luz real para que quede dentro del cuerpo del farol.
	glm::vec3 posicionLocalCristalFaro = glm::vec3(0.0f, 7.55f, 0.0f);
	// Aura visual del faro: mismo punto del cristal, pero con planos mas grandes
	// para que parezca que el foco irradia luz alrededor sin crear mas PointLights.
	glm::vec3 posicionLocalAuraFaro = glm::vec3(0.0f, 7.55f, 0.0f);

	// Modo editor para acomodar el faro con el teclado.
	// Flechas: izquierda/derecha mueven X, arriba/abajo mueven Z.
	// + y - cambian el tamaño.
	// Q sube y E baja la altura.
	// La luz del faro usa la misma jerarquía del faro, por eso se mueve junto con el modelo.
	float velocidadMoverFaro = 1.2f;
	float velocidadAlturaFaro = 1.0f;
	float velocidadEscalaFaro = 0.8f;
	float escalaMinimaFaro = 0.2f;
	float escalaMaximaFaro = 20.0f;
	float tiempoImpresionFaro = 0.0f;

	//Puente del progreso.
	//Queda como hijo de la jerarquia general del pueblo para que se mueva junto con todo.
	//El puente ya queda fijo con las coordenadas que se obtuvieron con el editor.
	//Ahora el editor activo mueve la jerarquia general completa.
	glm::vec3 posicionPuenteProgreso = glm::vec3(3.07f, 5.74f, 114.52f);
	float rotacionPuenteProgresoY = 0.0f;
	float escalaPuenteProgreso = 53.37f;

	// Modo editor para acomodar el puente con el teclado.
	// Flechas: izquierda/derecha mueven X, arriba/abajo mueven Z.
	// + y - cambian el tamaño.
	// Q sube y E baja la altura.
	float velocidadMoverPuente = 2.5f;
	float velocidadAlturaPuente = 1.5f;
	float velocidadEscalaPuente = 18.0f;
	float escalaMinimaPuente = 1.0f;
	float escalaMaximaPuente = 250.0f;
	float tiempoImpresionPuente = 0.0f;

	// Estacion_M dentro de jerarquiaAireEstacion.
	// IMPORTANTE: estos valores son LOCALES al padre jerarquiaAireEstacion.
	// Se quedan en identidad para que Estacion_M sea el centro real del grupo.
	glm::vec3 posicionEstacion = glm::vec3(0.0f, 0.0f, 0.0f);
	float rotacionEstacionY = 0.0f;
	float escalaEstacion = 1.0f;

	// AireEstacion queda separada de jerarquiaPueblo.
	// Es una rama independiente del mundo y funciona como padre real del conjunto:
	// jerarquiaAireEstacion -> jerarquiaEstacion -> Estacion_M + 3 globos/naves.
	// Esta es la posicion final que ya habias obtenido con el menu.
	glm::vec3 posicionAireEstacion = glm::vec3(-2.70f, 43.24f, -0.24f);
	float rotacionAireEstacionY = -1433.31f;
	float escalaAireEstacion = 68.88f;

	// Referencia antigua SOLO para calcular las posiciones locales de las 3 naves.
	// Con esto las rutas ya no se quedan amarradas al mundo anterior;
	// se convierten a coordenadas locales y despues cuelgan de Estacion_M.
	glm::vec3 posicionEstacionBaseRuta = glm::vec3(71.51f, 3.07f, -79.38f);
	float rotacionEstacionBaseRutaY = -1433.31f;
	float escalaEstacionBaseRuta = 39.63f;

	// Modo editor para acomodar la estacion con el teclado.
	// Flechas: izquierda/derecha mueven X, arriba/abajo mueven Z.
	// + y - cambian el tamaño.
	// Q sube y E baja la altura.
	// R y T rotan la estacion en Y.
	float velocidadMoverEstacion = 2.5f;
	float velocidadAlturaEstacion = 1.5f;
	float velocidadEscalaEstacion = 10.0f;
	float escalaMinimaEstacion = 1.0f;
	float escalaMaximaEstacion = 200.0f;
	float velocidadRotacionEstacion = 35.0f;
	float tiempoImpresionEstacion = 0.0f;

	// Globo.
	// Se importa como hijo de la jerarquia general del pueblo.
	// El editor activo mueve solo este modelo para obtener sus coordenadas locales.
	glm::vec3 posicionGlobo = glm::vec3(78.02f, 45.49f, -46.34f);
	float rotacionGloboY = -785.62f;
	float escalaGlobo = 9.19f;

	// Naves extra para que la ruta se vea más viva.
	// Usan la misma trayectoria del globo principal, pero con altura/desfase diferente.
	glm::vec3 posicionGloboArriba = posicionGlobo;
	float rotacionGloboArribaY = rotacionGloboY;
	float escalaGloboArriba = escalaGlobo;

	glm::vec3 posicionGloboAbajo = posicionGlobo;
	float rotacionGloboAbajoY = rotacionGloboY;
	float escalaGloboAbajo = escalaGlobo;

	// Modo editor para acomodar el globo con el teclado.
	// Flechas: izquierda/derecha mueven X, arriba/abajo mueven Z.
	// + y - cambian el tamaño.
	// Q sube y E baja la altura.
	// R y T rotan el globo en Y.
	float velocidadMoverGlobo = 2.5f;
	float velocidadAlturaGlobo = 1.5f;
	float velocidadEscalaGlobo = 8.0f;
	float escalaMinimaGlobo = 1.0f;
	float escalaMaximaGlobo = 200.0f;
	float velocidadRotacionGlobo = 35.0f;
	float tiempoImpresionGlobo = 0.0f;

	// Animacion final de los globos.
// Las tres naves usan rutas continuas calculadas con senos y cosenos.
// Asi no dependen de puntos guardados y visualmente se sienten como vuelo normal.
	float tiempoAnimacionGlobo = 0.0f;
	bool animacionGloboActiva = true;

	// Las naves quedan unidas a la estacion mediante jerarquia.
	// Por eso sus coordenadas, radios y escalas se trabajan como valores locales de la estacion,
	// no como coordenadas directas del pueblo. Si la estacion se mueve, rota o escala, las naves se van con ella.
	float factorLocalEstacion = 1.0f / escalaEstacionBaseRuta;
	glm::mat4 matrizLocalEstacionInicial = glm::mat4(1.0f);
	matrizLocalEstacionInicial = glm::translate(matrizLocalEstacionInicial, posicionEstacionBaseRuta);
	matrizLocalEstacionInicial = glm::rotate(matrizLocalEstacionInicial, rotacionEstacionBaseRutaY * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	matrizLocalEstacionInicial = glm::scale(matrizLocalEstacionInicial, glm::vec3(escalaEstacionBaseRuta, escalaEstacionBaseRuta, escalaEstacionBaseRuta));
	glm::mat4 inversaLocalEstacionInicial = glm::inverse(matrizLocalEstacionInicial);

	// Centros de vuelo convertidos al espacio local de la estacion para conservar la zona visual original.
	glm::vec3 centroRutaGloboPrincipal = glm::vec3(inversaLocalEstacionInicial * glm::vec4(80.0f, 45.49f, -103.0f, 1.0f));
	glm::vec3 centroRutaGloboArriba = glm::vec3(inversaLocalEstacionInicial * glm::vec4(78.0f, 67.49f, -78.0f, 1.0f));
	glm::vec3 centroRutaGloboAbajo = glm::vec3(inversaLocalEstacionInicial * glm::vec4(80.0f, 28.49f, -101.0f, 1.0f));

	// Ajustes de vuelo: radios, velocidad y pequenas ondulaciones.
	// Al estar dentro de la estacion, los tamaños se dividen entre la escala de la estacion.
	float velocidadGloboPrincipal = 0.115f;
	float velocidadGloboArriba = 0.095f;
	float velocidadGloboAbajo = 0.128f;

	float desfaseGloboPrincipal = 4.05f;
	float desfaseGloboArriba = 1.35f;
	float desfaseGloboAbajo = 4.55f;

	float escalaGloboLocalBase = 9.19f * factorLocalEstacion;

	// Posiciones iniciales calculadas por funcion para evitar saltos al empezar.
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

	// Grupo de aros independiente.
	// NO es hijo de la estacion ni del pueblo.
	// La matriz padre mueve juntos basearo + aro1 + aro2 + aro3 + aro4.
	// La animacion interna solo modifica los aros como hijos de esta base.
	glm::vec3 posicionGrupoAros = glm::vec3(-250.12f, 0.00f, -124.16f);
	float rotacionGrupoArosY = -20251.06f;
	float escalaGrupoAros = 0.21f;

	// Nave 2 independiente.
	// Ya no usa menu de edicion para no volver a afectar la camara.
	// La animacion se activa con la tecla N y recorre tres posiciones:
	// inicio -> media -> final. Despues desaparece y deja un flash que se apaga.
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

	// Copia de Nave 2 colocada a un lado y girada 180 grados.
	// Hace la animacion contraria: primero aparece un flash y despues la nave nace desde ahi.
	glm::vec3 separacionLateralNave2Copia = glm::vec3(0.0f, 0.0f, 38.0f);

	// Copia estatica del grupo de aros para acompanar la copia de Nave 2.
	// NO toma la animacion de los aros originales.
	// Coordenadas finales obtenidas con el editor manual.
	glm::vec3 posicionGrupoArosCopia = glm::vec3(-250.06f, 0.00f, -86.16f);
	float rotacionGrupoArosCopiaY = -20250.14f;
	float escalaGrupoArosCopia = 0.21f;


	// Modelo Maquinarr fijo en la posicion final obtenida con el editor.
	// El editor de teclado ya fue retirado para dejar limpio el codigo.
	// Maquinarr ahora es hijo directo de jerarquiaArcane, no de jerarquiaPueblo.
	// Valores locales a la tematica Arcane para conservar el acomodo visual original.
	glm::vec3 posicionMaquinarr = glm::vec3(50.60f, 0.38f, 83.71f);
	float rotacionMaquinarrY = 2701.93f;
	float escalaMaquinarr = 13.74f;

	// Estos valores son solo la referencia con la que se calcularon las coordenadas locales
	// de los hijos antes de mover Maquinarr a su posicion final.
	glm::vec3 posicionMaquinarrBaseConversion = glm::vec3(-255.35f, 0.00f, -105.41f);
	float rotacionMaquinarrBaseConversionY = 0.0f;
	float escalaMaquinarrBaseConversion = 12.52f;

	// Jerarquia general del conjunto de transporte.
	// Maquinarr queda como padre; los aros originales, Nave 2 original,
	// la copia de aros y la copia de Nave 2 se manejan como hijos.
	// Las posiciones que ya habian quedado bien en mundo se convierten
	// una sola vez a coordenadas locales para conservar el acomodo visual.
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

	// Animacion de los aros tipo resortera:
	// 0) Base: todos los aros quedan al ras de la base.
	// 1) Retroceso: se jalan hacia atras en cono pequeno.
	// 2) Disparo: salen al otro lado formando un cono mas pronunciado.
	// 3) Regreso: vuelven a quedar al ras de la base para repetir.
	float tiempoAnimacionAros = 0.0f;
	float duracionCicloArosNormal = 4.20f;
	float duracionCicloArosRapida = 1.05f;
	float duracionCicloArosActual = duracionCicloArosNormal;
	int cicloArosActual = 0; // 0 = animacion normal, 1 = repeticion rapida
	bool animacionArosActiva = false;
	bool tecla1LiberadaAros = true;
	bool animacionUnidaArosNave2Activa = false;
	bool iniciarNave2DespuesAros = false;

	// Eje real de disparo de los aros.
	// Y en OpenGL era altura, por eso subian/bajaban.
	// X era lateral, por eso se iban de izquierda a derecha.
	// Z es el eje que atraviesa la basearo, como una resortera.
	// Si al probarlo dispara hacia el lado contrario, cambia 1.0f por -1.0f.
	glm::vec3 ejeSalidaAros = glm::vec3(0.0f, 0.0f, 1.0f);

	// Separacion original de los OBJ sobre el eje de salida.
	// Se ajusto para que la pose 0 quede como la imagen 2:
	// aros compactos, pegados al aro 1/basearo, no extendidos como la imagen 1.
	float separacionOriginalAro1 = 0.00f;
	float separacionOriginalAro2 = 3.50f;
	float separacionOriginalAro3 = 7.00f;
	float separacionOriginalAro4 = 10.50f;

	// Cono hacia atras mas marcado: se siente mas como liga/resortera cargandose.
	float retrocesoAro1 = -8.00f;
	float retrocesoAro2 = -12.00f;
	float retrocesoAro3 = -16.00f;
	float retrocesoAro4 = -20.00f;

	// Cono pronunciado hacia enfrente: aqui se nota el disparo de la resortera.
	float frenteAro1 = 8.00f;
	float frenteAro2 = 20.00f;
	float frenteAro3 = 34.00f;
	float frenteAro4 = 50.00f;

	// Estado base invertido: ahora el reposo visual es la pose de rebote/cono.
	// Por eso los offsets iniciales ya arrancan en frenteAro y no en 0.
	float offsetAro1 = -separacionOriginalAro1 + frenteAro1;
	float offsetAro2 = -separacionOriginalAro2 + frenteAro2;
	float offsetAro3 = -separacionOriginalAro3 + frenteAro3;
	float offsetAro4 = -separacionOriginalAro4 + frenteAro4;

	// Jerarquia de planetas.
	// Todos estos modelos cuelgan de jerarquiaPlanetas para poder posicionarlos juntos.
	glm::vec3 posicionPlanetas = glm::vec3(7.77f, 133.38f, -3.01f);
	float rotacionPlanetasY = -2321.02f;
	float escalaPlanetas = 6.88f;

	// Posiciones locales de cada planeta dentro de la jerarquia.
	glm::vec3 posicionPlanetaMorado001 = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 posicionPlanetaAmarillo = glm::vec3(12.0f, 0.0f, 0.0f);
	glm::vec3 posicionPlanetaAzul = glm::vec3(-12.0f, 0.0f, 0.0f);
	glm::vec3 posicionPlanetaGris = glm::vec3(0.0f, 0.0f, 12.0f);
	glm::vec3 posicionPlanetaMorado = glm::vec3(0.0f, 0.0f, -12.0f);
	glm::vec3 posicionPlanetaVerde = glm::vec3(-8.5f, 0.0f, -8.5f);
	float escalaPlanetaMorado001 = 1.0f;
	float escalaPlanetaAmarillo = 1.0f;
	float escalaPlanetaAzul = 1.0f;
	float escalaPlanetaGris = 1.0f;
	float escalaPlanetaMorado = 1.0f;
	float escalaPlanetaVerde = 1.0f;

	float velocidadMoverPlanetas = 7.5f;
	float velocidadAlturaPlanetas = 4.5f;
	float velocidadEscalaPlanetas = 1.5f;
	float escalaMinimaPlanetas = 0.05f;
	float escalaMaximaPlanetas = 80.0f;
	float velocidadRotacionPlanetas = 3.5f;
	float tiempoImpresionPlanetas = 0.0f;

	// Jerarquia del Principito.
	// Todas las piezas separadas se unen con esta matriz padre para moverlas juntas.
	glm::vec3 posicionPrincipito = glm::vec3(-2.68f, 274.28f, -333.72f);
	float rotacionPrincipitoY = -199.62f;
	float escalaPrincipito = 5.36f;

	// Al venir separado desde Blender, las piezas comparten el mismo origen de exportacion.
	// Por eso sus posiciones locales se dejan en cero para que embonen entre si.
	glm::vec3 posicionBrazoDerPrincipito = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 posicionBrazoIzqPrincipito = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 posicionCabezaPrincipito = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 posicionPiernaDerPrincipito = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 posicionPiernaIzqPrincipito = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 posicionTorsoPrincipito = glm::vec3(0.0f, 0.0f, 0.0f);

	float escalaBrazoDerPrincipito = 1.0f;
	float escalaBrazoIzqPrincipito = 1.0f;
	float escalaCabezaPrincipito = 1.0f;
	float escalaPiernaDerPrincipito = 1.0f;
	float escalaPiernaIzqPrincipito = 1.0f;
	float escalaTorsoPrincipito = 1.0f;

	float velocidadMoverPrincipito = 7.5f;
	float velocidadAlturaPrincipito = 4.5f;
	float velocidadEscalaPrincipito = 1.5f;
	float escalaMinimaPrincipito = 0.05f;
	float escalaMaximaPrincipito = 80.0f;
	float velocidadRotacionPrincipito = 3.5f;
	float tiempoImpresionPrincipito = 0.0f;

	// Rosa del planeta azul.
	// Es una sola pieza y ahora queda como hija directa del planeta azul,
	// por eso se mueve y vibra junto con la animacion del planeta.
	// Estos valores son LOCALES al planeta azul.
	glm::vec3 posicionRosa = glm::vec3(-16.94f, 10.89f, -30.57f);
	float rotacionRosaY = 2321.02f;
	float escalaRosa = 1.85f;
	float velocidadMoverRosa = 7.5f;
	float velocidadAlturaRosa = 4.5f;
	float velocidadEscalaRosa = 1.5f;
	float escalaMinimaRosa = 0.05f;
	float escalaMaximaRosa = 200.0f;
	float velocidadRotacionRosa = 3.5f;
	float tiempoImpresionRosa = 0.0f;

	////Loop mientras no se cierra la ventana
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
			printf("AireEstacion fija -> pos = glm::vec3(%.2ff, %.2ff, %.2ff); | escala = %.2ff | rotY = %.2ff\n",
				posicionAireEstacion.x, posicionAireEstacion.y, posicionAireEstacion.z, escalaAireEstacion, rotacionAireEstacionY);
			printf("Globo inicial -> pos = glm::vec3(%.2ff, %.2ff, %.2ff); | escala = %.2ff | rotY = %.2ff\n",
				posicionGlobo.x, posicionGlobo.y, posicionGlobo.z, escalaGlobo, rotacionGloboY);
			printf("Blitz inicial -> pos = glm::vec3(%.2ff, %.2ff, %.2ff); | escala = %.4ff | rotY = %.2ff\n",
				posicionBlitz.x, posicionBlitz.y, posicionBlitz.z, escalaBlitz, rotacionBlitzY);
			datosInicialesGrupoMostrados = true;
		}


		// Sistema de día y noche.
		// timeOfDay avanza de 0 a 24 y vuelve a empezar.
		timeOfDay += dayNightSpeed * deltaTime;
		if (timeOfDay >= 24.0f)
			timeOfDay = 0.0f;

		// Cálculo sencillo: al mediodía hay más intensidad, de noche baja.
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

		// Se actualiza la luz direccional para que funcione como sol/luna.
		mainLight = DirectionalLight(rojoSol, verdeSol, azulSol,
			ambienteSol, difusaSol,
			0.0f, -1.0f, -1.0f);

		// Animación de la malla circular:
		// se rota sobre sus propios ejes para simular aros de energía alrededor de la esfera.
		rotMallaX += rotMallaXOffset * deltaTime;
		rotMallaY += rotMallaYOffset * deltaTime;
		rotMallaZ += rotMallaZOffset * deltaTime;

		if (rotMallaX >= 360.0f)
			rotMallaX = 0.0f;
		if (rotMallaY >= 360.0f)
			rotMallaY = 0.0f;
		if (rotMallaZ >= 360.0f)
			rotMallaZ = 0.0f;

		// Animación cíclica del coche:
		// 0 = avanza hasta el límite original del piso
		// 1 = vuelta en U al final
		// 2 = regresa hacia el punto de partida
		// 3 = vuelta en U al inicio para repetir el ciclo
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


		// Animación cíclica de la nave por banderas y condicionales.
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

		// Animacion tipo resortera de los aros.
		// Se activa con la tecla 1.
		// Primero hace el ciclo normal y, al terminar, repite automaticamente
		// el mismo movimiento una segunda vez mucho mas rapido.
		// Cuando no esta activa, todos los anillos quedan al ras del aro 1.
		bool* keysAros = mainWindow.getsKeys();
		if (keysAros[GLFW_KEY_1])
		{
			if (tecla1LiberadaAros && !animacionUnidaArosNave2Activa && !animacionArosActiva && !animacionNave2Activa)
			{
				// Tecla 1: primero se activan los aros originales.
				// Al terminar los dos ciclos de aros, se inicia automaticamente la animacion de Nave 2 original.
				animacionUnidaArosNave2Activa = true;
				iniciarNave2DespuesAros = true;
				animacionArosActiva = true;
				tiempoAnimacionAros = 0.0f;
				cicloArosActual = 0;
				duracionCicloArosActual = duracionCicloArosNormal;

				// Se deja Nave 2 lista en su posicion inicial mientras terminan los aros.
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

		// Estado base invertido:
		// Cuando NO se presiona la tecla 1, los aros quedan en la pose de rebote/cono.
		// Al activar la animacion, los aros se pliegan a la base y despues regresan al rebote.
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
					// Al terminar la primera vuelta, se dispara una segunda vuelta mas rapida.
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
						// Segunda parte de la animacion unida: arranca Nave 2 original.
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
				// 0 - Pose base nueva: rebote/cono abierto.
				poseAro1 = frenteAro1;
				poseAro2 = frenteAro2;
				poseAro3 = frenteAro3;
				poseAro4 = frenteAro4;
			}
			else if (progresoAros < 0.32f)
			{
				// 1 - Se pliegan hacia la base compacta.
				float t = (progresoAros - 0.12f) / 0.20f;
				poseAro1 = mezclarAros(frenteAro1, 0.0f, t);
				poseAro2 = mezclarAros(frenteAro2, 0.0f, t);
				poseAro3 = mezclarAros(frenteAro3, 0.0f, t);
				poseAro4 = mezclarAros(frenteAro4, 0.0f, t);
			}
			else if (progresoAros < 0.45f)
			{
				// 2 - Pausa breve: todos quedan en la base compacta.
				poseAro1 = 0.0f;
				poseAro2 = 0.0f;
				poseAro3 = 0.0f;
				poseAro4 = 0.0f;
			}
			else if (progresoAros < 0.58f)
			{
				// 3 - Se cargan hacia atras como resortera.
				float t = (progresoAros - 0.45f) / 0.13f;
				poseAro1 = mezclarAros(0.0f, retrocesoAro1, t);
				poseAro2 = mezclarAros(0.0f, retrocesoAro2, t);
				poseAro3 = mezclarAros(0.0f, retrocesoAro3, t);
				poseAro4 = mezclarAros(0.0f, retrocesoAro4, t);
			}
			else if (progresoAros < 0.68f)
			{
				// 4 - PU2: regresan al estado base nuevo, que es el rebote/cono.
				float t = (progresoAros - 0.58f) / 0.10f;
				poseAro1 = mezclarAros(retrocesoAro1, frenteAro1, t);
				poseAro2 = mezclarAros(retrocesoAro2, frenteAro2, t);
				poseAro3 = mezclarAros(retrocesoAro3, frenteAro3, t);
				poseAro4 = mezclarAros(retrocesoAro4, frenteAro4, t);
			}
			else if (progresoAros < 0.82f)
			{
				// Rebote pequeno hacia enfrente para que se sienta la elasticidad.
				float t = (progresoAros - 0.68f) / 0.14f;
				float rebote = sin(suavizarAros(t) * 3.14159265f);
				poseAro1 = frenteAro1 + (0.35f * rebote);
				poseAro2 = frenteAro2 + (0.65f * rebote);
				poseAro3 = frenteAro3 + (0.95f * rebote);
				poseAro4 = frenteAro4 + (1.25f * rebote);
			}
			else
			{
				// Termina exactamente en el estado base nuevo: rebote/cono.
				poseAro1 = frenteAro1;
				poseAro2 = frenteAro2;
				poseAro3 = frenteAro3;
				poseAro4 = frenteAro4;
			}
		}

		// Primero se compensa la separacion original del OBJ para que todos queden al ras del aro 1.
		// Despues se suma la pose animada sobre el eje de salida, como una resortera.
		offsetAro1 = -separacionOriginalAro1 + poseAro1;
		offsetAro2 = -separacionOriginalAro2 + poseAro2;
		offsetAro3 = -separacionOriginalAro3 + poseAro3;
		offsetAro4 = -separacionOriginalAro4 + poseAro4;

		//Recibir eventos del usuario
		glfwPollEvents();

		// Camara restaurada al manejo original del proyecto.
		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());


		// Teclas para prender y apagar luces sin crear mas PointLights.
		// I: luz azul del Incognium.
		// F: luz compartida de los cuatro faros.
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
				printf("Luz faros: %s\n", luzFarosActiva ? "ENCENDIDA" : "APAGADA");
				teclaFLiberada = false;
			}
		}
		else
		{
			teclaFLiberada = true;
		}


		// Animacion de Nave 2 por teclado sin tocar la camara.
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
			// Se usa frameTime real para que no se acelere por el deltaTime alterado del proyecto.
			tiempoFaseNave2 += frameTime;

			if (faseAnimacionNave2 == 1)
			{
				float t = SuavizarAnimacion(tiempoFaseNave2 / duracionTramoNave2_1);
				posicionNave2 = glm::mix(nave2Inicio, nave2Media, t);
				rotacionNave2Y = nave2RotYInicio + ((nave2RotYMedia - nave2RotYInicio) * t);
				// No se achica aqui para que no parezca que desaparece a medio camino.
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
				// Mantiene tamaño completo durante el tramo medio.
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

				// El achicamiento ocurre casi al final del recorrido, justo antes del flash.
				// Durante la mayor parte de este tramo la nave conserva su tamaño.
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
			// Animacion inversa de la copia: flash -> aparece -> recorre en sentido contrario.
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
				// Giro final: al terminar el recorrido, la copia se voltea para mirar al otro lado.
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

		/*
		// Menu para mover Rosa.
		// IMPORTANTE: se reutiliza el mismo menu base del codigo.
		// Solo debe existir un menu de posicionamiento activo al mismo tiempo.
		// Flechas / H L J K / Numpad 4 6 8 2 = mover X/Z
		// Q / E / Numpad 7 9 = subir/bajar
		// + / - = escala
		// R / T / Numpad 1 3 = rotar
		// P = imprimir posicion en consola
		bool movioRosa = false;
		static bool teclaPRosaLiberada = true;
		bool* keysRosa = mainWindow.getsKeys();

		if (keysRosa[GLFW_KEY_LEFT] || keysRosa[GLFW_KEY_H] || keysRosa[GLFW_KEY_KP_4])
		{
			posicionRosa.x -= velocidadMoverRosa * deltaTime;
			movioRosa = true;
		}
		if (keysRosa[GLFW_KEY_RIGHT] || keysRosa[GLFW_KEY_L] || keysRosa[GLFW_KEY_KP_6])
		{
			posicionRosa.x += velocidadMoverRosa * deltaTime;
			movioRosa = true;
		}
		if (keysRosa[GLFW_KEY_UP] || keysRosa[GLFW_KEY_J] || keysRosa[GLFW_KEY_KP_8])
		{
			posicionRosa.z -= velocidadMoverRosa * deltaTime;
			movioRosa = true;
		}
		if (keysRosa[GLFW_KEY_DOWN] || keysRosa[GLFW_KEY_K] || keysRosa[GLFW_KEY_KP_2])
		{
			posicionRosa.z += velocidadMoverRosa * deltaTime;
			movioRosa = true;
		}

		if (keysRosa[GLFW_KEY_Q] || keysRosa[GLFW_KEY_KP_7])
		{
			posicionRosa.y += velocidadAlturaRosa * deltaTime;
			movioRosa = true;
		}
		if (keysRosa[GLFW_KEY_E] || keysRosa[GLFW_KEY_KP_9])
		{
			posicionRosa.y -= velocidadAlturaRosa * deltaTime;
			movioRosa = true;
		}

		if (keysRosa[GLFW_KEY_EQUAL] || keysRosa[GLFW_KEY_KP_ADD])
		{
			escalaRosa += velocidadEscalaRosa * deltaTime;
			if (escalaRosa > escalaMaximaRosa)
				escalaRosa = escalaMaximaRosa;
			movioRosa = true;
		}
		if (keysRosa[GLFW_KEY_MINUS] || keysRosa[GLFW_KEY_KP_SUBTRACT])
		{
			escalaRosa -= velocidadEscalaRosa * deltaTime;
			if (escalaRosa < escalaMinimaRosa)
				escalaRosa = escalaMinimaRosa;
			movioRosa = true;
		}

		if (keysRosa[GLFW_KEY_R] || keysRosa[GLFW_KEY_KP_1])
		{
			rotacionRosaY -= velocidadRotacionRosa * deltaTime;
			movioRosa = true;
		}
		if (keysRosa[GLFW_KEY_T] || keysRosa[GLFW_KEY_KP_3])
		{
			rotacionRosaY += velocidadRotacionRosa * deltaTime;
			movioRosa = true;
		}

		if (keysRosa[GLFW_KEY_P])
		{
			if (teclaPRosaLiberada)
			{
				printf("Rosa pos = glm::vec3(%.2ff, %.2ff, %.2ff); | escala = %.2ff | rotY = %.2ff\n",
					posicionRosa.x, posicionRosa.y, posicionRosa.z, escalaRosa, rotacionRosaY);
				teclaPRosaLiberada = false;
			}
		}
		else
		{
			teclaPRosaLiberada = true;
		}

		tiempoImpresionRosa += deltaTime;
		if (movioRosa && tiempoImpresionRosa >= 0.35f)
		{
			printf("Rosa pos = glm::vec3(%.2ff, %.2ff, %.2ff); | escala = %.2ff | rotY = %.2ff\n",
				posicionRosa.x, posicionRosa.y, posicionRosa.z, escalaRosa, rotacionRosaY);
			tiempoImpresionRosa = 0.0f;
		}

		*/

		// Animacion continua final de los globos/naves de la estacion del aire.
		// Se mantiene antes de crear las jerarquias para actualizar sus posiciones locales
		// sin modificar la estructura: siguen siendo hijos de jerarquiaAireEstacion.
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

		// JerarquiaArcane.
		// Padre general de la tematica Arcane. Absorbe a jerarquiaPueblo, jerarquiaBlitz y jerarquiaMaquinarr.
		glm::mat4 jerarquiaArcane = glm::mat4(1.0f);
		jerarquiaArcane = glm::translate(jerarquiaArcane, posicionArcane);
		jerarquiaArcane = glm::rotate(jerarquiaArcane, rotacionArcaneY * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		jerarquiaArcane = glm::scale(jerarquiaArcane, glm::vec3(escalaArcane, escalaArcane, escalaArcane));

		// JerarquiaPueblo.
		// Solo contiene Pueblo_M, Incognium, los 4 faros y Puente del Progreso.
		glm::mat4 jerarquiaPueblo = jerarquiaArcane;
		jerarquiaPueblo = glm::translate(jerarquiaPueblo, posicionGrupoPueblo);
		jerarquiaPueblo = glm::rotate(jerarquiaPueblo, rotacionGrupoPuebloY * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		jerarquiaPueblo = glm::scale(jerarquiaPueblo, glm::vec3(escalaGrupoPueblo, escalaGrupoPueblo, escalaGrupoPueblo));

		// JerarquiaAireEstacion queda separada de jerarquiaPueblo.
		// Esta rama NO se mueve con jerarquiaPueblo.
		// De aqui cuelga jerarquiaEstacion, y de jerarquiaEstacion cuelgan juntos:
		// Estacion_M + Globo principal + Globo arriba + Globo abajo.
		glm::mat4 jerarquiaAireEstacion = glm::mat4(1.0f);
		jerarquiaAireEstacion = glm::translate(jerarquiaAireEstacion, posicionAireEstacion);
		jerarquiaAireEstacion = glm::rotate(jerarquiaAireEstacion, rotacionAireEstacionY * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		jerarquiaAireEstacion = glm::scale(jerarquiaAireEstacion, glm::vec3(escalaAireEstacion, escalaAireEstacion, escalaAireEstacion));

		// JerarquiaEstacion es hija directa de jerarquiaAireEstacion.
		// No se vuelve a colgar de jerarquiaPueblo.
		glm::mat4 jerarquiaEstacion = jerarquiaAireEstacion;

		// JerarquiaAuraEstacion.
		// Es hija directa de jerarquiaEstacion, por eso los cuadritos y la luz azul
		// se quedan pegados a la estacion aunque se mueva jerarquiaAireEstacion.
		glm::mat4 jerarquiaAuraEstacion = jerarquiaEstacion;
		jerarquiaAuraEstacion = glm::translate(jerarquiaAuraEstacion, posicionLocalAuraEstacion);
		jerarquiaAuraEstacion = glm::rotate(jerarquiaAuraEstacion, rotacionAuraEstacionY * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		jerarquiaAuraEstacion = glm::scale(jerarquiaAuraEstacion, glm::vec3(escalaAuraEstacion, escalaAuraEstacion, escalaAuraEstacion));

		// Luz azul de la estacion.
		// Antes no habia una luz propia de Estacion_M: solo existia la azul del Incognium.
		// Esta luz usa la misma jerarquiaEstacion, asi que acompaña a la estacion y a sus naves.
		glm::vec3 posicionLuzEstacion = glm::vec3(jerarquiaAuraEstacion * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
		float intensidadEstacion = luzEstacionActiva ? 1.0f : 0.0f;
		pointLights[0] = PointLight(0.0f, 0.42f, 1.0f,
			0.32f * intensidadEstacion, 10.85f * intensidadEstacion,
			posicionLuzEstacion.x, posicionLuzEstacion.y, posicionLuzEstacion.z,
			1.0f, 0.006f, 0.0008f);

		// JerarquiaElprincipito.
		// Padre de la tematica del Principito. De aqui cuelgan jerarquiaPlanetas y jerarquiaPrincipito.
		glm::mat4 jerarquiaElprincipito = glm::mat4(1.0f);

		// JerarquiaPlanetas.
		// Hija de jerarquiaElprincipito.
		glm::mat4 jerarquiaPlanetas = jerarquiaElprincipito;
		jerarquiaPlanetas = glm::translate(jerarquiaPlanetas, posicionPlanetas);
		jerarquiaPlanetas = glm::rotate(jerarquiaPlanetas, rotacionPlanetasY * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		jerarquiaPlanetas = glm::scale(jerarquiaPlanetas, glm::vec3(escalaPlanetas, escalaPlanetas, escalaPlanetas));

		// JerarquiaPrincipito.
		// Hija de jerarquiaElprincipito. Aqui se unen todas las piezas del Principito.
		glm::mat4 jerarquiaPrincipito = jerarquiaElprincipito;
		jerarquiaPrincipito = glm::translate(jerarquiaPrincipito, posicionPrincipito);
		jerarquiaPrincipito = glm::rotate(jerarquiaPrincipito, rotacionPrincipitoY * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		jerarquiaPrincipito = glm::scale(jerarquiaPrincipito, glm::vec3(escalaPrincipito, escalaPrincipito, escalaPrincipito));


		// Vibracion muy leve para los planetas.
		// Cada uno tiene fase y velocidad distinta para que no latan al mismo tiempo.
		float tiempoPlanetas = (float)glfwGetTime();
		float vibracionMorado001Y = 0.028f * sin(tiempoPlanetas * 1.70f + 0.20f) + 0.010f * sin(tiempoPlanetas * 3.10f + 1.10f);
		float vibracionAmarilloY = 0.026f * sin(tiempoPlanetas * 1.95f + 1.40f) + 0.009f * sin(tiempoPlanetas * 3.60f + 0.30f);
		float vibracionAzulY = 0.030f * sin(tiempoPlanetas * 1.55f + 2.10f) + 0.008f * sin(tiempoPlanetas * 2.90f + 1.70f);
		float vibracionGrisY = 0.024f * sin(tiempoPlanetas * 2.05f + 0.80f) + 0.010f * sin(tiempoPlanetas * 3.40f + 2.40f);
		float vibracionMoradoY = 0.027f * sin(tiempoPlanetas * 1.82f + 2.80f) + 0.009f * sin(tiempoPlanetas * 3.00f + 0.90f);
		float vibracionRosaY = 0.025f * sin(tiempoPlanetas * 2.15f + 1.90f) + 0.008f * sin(tiempoPlanetas * 3.80f + 2.00f);
		float vibracionVerdeY = 0.029f * sin(tiempoPlanetas * 1.62f + 3.40f) + 0.010f * sin(tiempoPlanetas * 3.25f + 1.30f);

		float pulsoMorado001 = 1.0f + 0.012f * sin(tiempoPlanetas * 2.20f + 0.50f);
		float pulsoAmarillo = 1.0f + 0.011f * sin(tiempoPlanetas * 2.45f + 1.60f);
		float pulsoAzul = 1.0f + 0.013f * sin(tiempoPlanetas * 2.00f + 2.30f);
		float pulsoGris = 1.0f + 0.010f * sin(tiempoPlanetas * 2.55f + 0.70f);
		float pulsoMorado = 1.0f + 0.012f * sin(tiempoPlanetas * 2.18f + 2.90f);
		float pulsoRosa = 1.0f + 0.011f * sin(tiempoPlanetas * 2.70f + 1.20f);
		float pulsoVerde = 1.0f + 0.013f * sin(tiempoPlanetas * 2.08f + 3.10f);

		// Matriz animada compartida del planeta azul.
		// La rosa usa exactamente esta misma matriz como padre, por eso ya no tiene
		// una vibracion propia ni una fase distinta: se queda pegada al planeta azul.
		float escalaAnimadaPlanetaAzul = escalaPlanetaAzul * pulsoAzul;
		glm::mat4 jerarquiaPlanetaAzulAnimada = jerarquiaPlanetas;
		jerarquiaPlanetaAzulAnimada = glm::translate(jerarquiaPlanetaAzulAnimada, posicionPlanetaAzul + glm::vec3(0.0f, vibracionAzulY, 0.0f));
		jerarquiaPlanetaAzulAnimada = glm::scale(jerarquiaPlanetaAzulAnimada, glm::vec3(escalaAnimadaPlanetaAzul, escalaAnimadaPlanetaAzul, escalaAnimadaPlanetaAzul));

		// Jerarquia de Blitzcrank.
		// Ya no es hijo de jerarquiaPueblo; ahora cuelga directamente de jerarquiaArcane.
		float tiempoBlitz = (float)glfwGetTime();
		float fasePasoBlitz = tiempoBlitz * velocidadPasoBlitz;
		float senoPasoBlitz = sin(fasePasoBlitz);
		float cosPasoBlitz = cos(fasePasoBlitz);
		float bobBlitz = 0.0f;
		float anguloBrazoDerBlitz = amplitudPasoBrazoBlitz * senoPasoBlitz;
		float anguloBrazoIzqBlitz = -amplitudPasoBrazoBlitz * senoPasoBlitz;
		float anguloPiernaDerBlitz = -amplitudPasoPiernaBlitz * senoPasoBlitz;
		float anguloPiernaIzqBlitz = amplitudPasoPiernaBlitz * senoPasoBlitz;
		float anguloPieDerBlitz = 3.0f * cosPasoBlitz;
		float anguloPieIzqBlitz = -3.0f * cosPasoBlitz;

		glm::mat4 jerarquiaBlitz = jerarquiaArcane;
		jerarquiaBlitz = glm::translate(jerarquiaBlitz, posicionBlitz + glm::vec3(0.0f, bobBlitz, 0.0f));
		jerarquiaBlitz = glm::rotate(jerarquiaBlitz, rotacionBlitzY * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		jerarquiaBlitz = glm::scale(jerarquiaBlitz, glm::vec3(escalaBlitz, escalaBlitz, escalaBlitz));

		//Jerarquia del grupo Incognium.
		//El Incognium queda como hijo del pueblo, pero conserva su propia
		//posicion local, escala, pivote de malla y luz azul.
		glm::mat4 jerarquiaIncognium = jerarquiaPueblo;
		jerarquiaIncognium = glm::translate(jerarquiaIncognium, posicionGrupoIncognium);
		jerarquiaIncognium = glm::scale(jerarquiaIncognium, glm::vec3(escalaGrupoIncognium, escalaGrupoIncognium, escalaGrupoIncognium));

		glm::vec3 posicionLuzIncognium = glm::vec3(jerarquiaIncognium * glm::vec4(pivoteMalla, 1.0f));
		float intensidadIncogniumManual = luzIncogniumActiva ? 1.0f : 0.0f;
		pointLights[1] = PointLight(0.0f, 0.45f, 1.0f,
			0.35f * intensidadIncogniumManual, 1.4f * intensidadIncogniumManual,
			posicionLuzIncognium.x, posicionLuzIncognium.y, posicionLuzIncognium.z,
			1.0f, 0.010f, 0.0010f);

		// Jerarquias de los faros.
		// Los faros ahora son hijos de la jerarquia general del pueblo.
		// Si despues se mueve, rota o escala el pueblo, los 4 faros se van con el.
		glm::mat4 jerarquiaFaros[4];
		for (int i = 0; i < 4; i++)
		{
			jerarquiaFaros[i] = jerarquiaPueblo;
			jerarquiaFaros[i] = glm::translate(jerarquiaFaros[i], posicionFaros[i]);
			jerarquiaFaros[i] = glm::scale(jerarquiaFaros[i], glm::vec3(escalaFaro, escalaFaro, escalaFaro));
		}

		// Luz real compartida para los 4 faros.
		// No se crean mas PointLights: se usa una sola luz muy alta, suave y con mucha cobertura.
		// La luz tambien queda como hija del pueblo: primero se calcula su posicion local
		// y luego se transforma con jerarquiaPueblo para que se mueva junto con todo.
		glm::vec3 posicionLuzFarosLocal = glm::vec3(
			(posicionFaros[0].x + posicionFaros[1].x + posicionFaros[2].x + posicionFaros[3].x) / 4.0f,
			45.0f,
			(posicionFaros[0].z + posicionFaros[1].z + posicionFaros[2].z + posicionFaros[3].z) / 4.0f
		);
		glm::vec3 posicionLuzFaros = glm::vec3(jerarquiaPueblo * glm::vec4(posicionLuzFarosLocal, 1.0f));

		float intensidadFaro = luzFarosActiva ? intensidadNoche : 0.0f;
		if (intensidadFaro < 0.12f)
			intensidadFaro = 0.0f;

		// Atenuacion muy baja = llega a todo el pueblo.
		// Intensidad suave = evita el manchon quemado en el centro.
		pointLights[2] = PointLight(1.0f, 0.86f, 0.55f,
			0.055f * intensidadFaro, 1.05f * intensidadFaro,
			posicionLuzFaros.x, posicionLuzFaros.y, posicionLuzFaros.z,
			1.0f, 0.00025f, 0.0000025f);

		// Flash visual de Nave 2.
		// No se usa ninguna luz puntual extra ni se reutiliza pointLights[0],
		// para no alterar el conteo de luces ni iluminar de más la escena.

		// Clear the window
		glClearColor(0.02f * intensidadDia, 0.04f * intensidadDia, 0.08f + (0.22f * intensidadDia), 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// El skybox se oscurece junto con el ciclo de dia y noche.
		// No se cambia la textura: se multiplica su color en el shader del skybox.
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
		uniformTextureOffset = shaderList[0].getOffsetLocation(); // para la textura con movimiento

		//información en el shader de intensidad especular y brillo
		uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
		uniformShininess = shaderList[0].GetShininessLocation();

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

		// luz ligada a la cámara de tipo flash
		lowerLight = camera.getCameraPosition();
		lowerLight.y -= 0.3f;
		spotLights[0].SetFlash(lowerLight, camera.getCameraDirection());

		//información al shader de fuentes de iluminación
		shaderList[0].SetDirectionalLight(&mainLight);
		shaderList[0].SetPointLights(pointLights, pointLightCount);
		shaderList[0].SetSpotLights(spotLights, spotLightCount);


		//Reinicializando variables cada ciclo de reloj
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

		//Pueblo
		//El pueblo usa la jerarquia padre general.
		//Los modelos hijos, como Incognium y faros, usan la misma matriz padre.
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

		// Blitzcrank.
		// Las piezas rotan sobre el origen que se exporto desde Blender.
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		toffset = glm::vec2(0.0f, 0.0f);
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);

		model = jerarquiaBlitz;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		BlitzTorso_M.RenderModel();

		model = jerarquiaBlitz;
		model = glm::rotate(model, (anguloBrazoDerBlitz * 0.35f) * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		BlitzHombroDer_M.RenderModel();

		model = jerarquiaBlitz;
		model = glm::rotate(model, anguloBrazoDerBlitz * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		BlitzBrazoDer_M.RenderModel();

		model = jerarquiaBlitz;
		model = glm::rotate(model, (anguloBrazoIzqBlitz * 0.35f) * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		BlitzHombroIzq_M.RenderModel();

		model = jerarquiaBlitz;
		model = glm::rotate(model, anguloBrazoIzqBlitz * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		BlitzBrazoIzq_M.RenderModel();

		model = jerarquiaBlitz;
		model = glm::rotate(model, anguloPiernaDerBlitz * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		BlitzPiernaDer_M.RenderModel();

		model = jerarquiaBlitz;
		model = glm::rotate(model, anguloPiernaIzqBlitz * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		BlitzPiernaIzq_M.RenderModel();

		model = jerarquiaBlitz;
		model = glm::rotate(model, anguloPieDerBlitz * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		BlitzPieDer_M.RenderModel();

		model = jerarquiaBlitz;
		model = glm::rotate(model, anguloPieIzqBlitz * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		BlitzPieIzq_M.RenderModel();

		// Estacion del aire.
		// Queda como hija de jerarquiaAireEstacion a traves de jerarquiaEstacion.
		// Los tres globos/naves usan esta misma jerarquiaEstacion como padre.
		model = jerarquiaEstacion;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		toffset = glm::vec2(0.0f, 0.0f);
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Estacion_M.RenderModel();

		// Aura visual azul de la estacion.
		// La PointLight ilumina la escena, pero este brillo ayuda a que se vea de donde sale la luz.
		if (luzEstacionActiva)
		{
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE);
			glDepthMask(GL_FALSE);
			plainTexture.UseTexture();
			Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
			color = glm::vec3(0.0f, 2.8f, 8.0f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			toffset = glm::vec2(0.0f, 0.0f);
			glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));

			model = jerarquiaAuraEstacion;
			model = glm::rotate(model, 90.0f * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
			model = glm::scale(model, glm::vec3(0.16f, 1.0f, 0.16f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			meshList[5]->RenderMesh();

			model = jerarquiaAuraEstacion;
			model = glm::rotate(model, 90.0f * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
			model = glm::rotate(model, 90.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
			model = glm::scale(model, glm::vec3(0.16f, 1.0f, 0.16f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			meshList[5]->RenderMesh();

			model = jerarquiaAuraEstacion;
			model = glm::scale(model, glm::vec3(0.11f, 1.0f, 0.11f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			meshList[5]->RenderMesh();

			glDepthMask(GL_TRUE);
			glDisable(GL_BLEND);
		}



		// Principito.
		// Todas las piezas son hijas de jerarquiaPrincipito, que a su vez cuelga de jerarquiaElprincipito.
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		toffset = glm::vec2(0.0f, 0.0f);
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);

		model = jerarquiaPrincipito;
		model = glm::translate(model, posicionTorsoPrincipito);
		model = glm::scale(model, glm::vec3(escalaTorsoPrincipito, escalaTorsoPrincipito, escalaTorsoPrincipito));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		PrincipitoTorso_M.RenderModel();

		model = jerarquiaPrincipito;
		model = glm::translate(model, posicionCabezaPrincipito);
		model = glm::scale(model, glm::vec3(escalaCabezaPrincipito, escalaCabezaPrincipito, escalaCabezaPrincipito));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		PrincipitoCabeza_M.RenderModel();

		model = jerarquiaPrincipito;
		model = glm::translate(model, posicionBrazoDerPrincipito);
		model = glm::scale(model, glm::vec3(escalaBrazoDerPrincipito, escalaBrazoDerPrincipito, escalaBrazoDerPrincipito));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		PrincipitoBrazoDer_M.RenderModel();

		model = jerarquiaPrincipito;
		model = glm::translate(model, posicionBrazoIzqPrincipito);
		model = glm::scale(model, glm::vec3(escalaBrazoIzqPrincipito, escalaBrazoIzqPrincipito, escalaBrazoIzqPrincipito));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		PrincipitoBrazoIzq_M.RenderModel();

		model = jerarquiaPrincipito;
		model = glm::translate(model, posicionPiernaDerPrincipito);
		model = glm::scale(model, glm::vec3(escalaPiernaDerPrincipito, escalaPiernaDerPrincipito, escalaPiernaDerPrincipito));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		PrincipitoPiernaDer_M.RenderModel();

		model = jerarquiaPrincipito;
		model = glm::translate(model, posicionPiernaIzqPrincipito);
		model = glm::scale(model, glm::vec3(escalaPiernaIzqPrincipito, escalaPiernaIzqPrincipito, escalaPiernaIzqPrincipito));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		PrincipitoPiernaIzq_M.RenderModel();

		// Planetas.
		// Todos son hijos de jerarquiaPlanetas, que a su vez cuelga de jerarquiaElprincipito.
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		toffset = glm::vec2(0.0f, 0.0f);
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);

		model = jerarquiaPlanetas;
		model = glm::translate(model, posicionPlanetaMorado001 + glm::vec3(0.0f, vibracionMorado001Y, 0.0f));
		model = glm::scale(model, glm::vec3(escalaPlanetaMorado001 * pulsoMorado001, escalaPlanetaMorado001 * pulsoMorado001, escalaPlanetaMorado001 * pulsoMorado001));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		PlanetaMorado001_M.RenderModel();

		model = jerarquiaPlanetas;
		model = glm::translate(model, posicionPlanetaAmarillo + glm::vec3(0.0f, vibracionAmarilloY, 0.0f));
		model = glm::scale(model, glm::vec3(escalaPlanetaAmarillo * pulsoAmarillo, escalaPlanetaAmarillo * pulsoAmarillo, escalaPlanetaAmarillo * pulsoAmarillo));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		PlanetaAmarillo_M.RenderModel();

		model = jerarquiaPlanetaAzulAnimada;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		PlanetaAzul_M.RenderModel();

		// Rosa del planeta azul.
		// Usa la MISMA matriz animada del planeta azul como padre.
		// La posicion de la rosa si hereda la vibracion/pulso del planeta,
		// pero se compensa la escala heredada para que la rosa no tenga una
		// animacion propia ni parezca separarse cuando el planeta llega a su maximo.
		model = jerarquiaPlanetaAzulAnimada;
		model = glm::translate(model, posicionRosa);
		model = glm::rotate(model, rotacionRosaY * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		float compensacionEscalaRosa = 1.0f / escalaAnimadaPlanetaAzul;
		model = glm::scale(model, glm::vec3(escalaRosa * compensacionEscalaRosa, escalaRosa * compensacionEscalaRosa, escalaRosa * compensacionEscalaRosa));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Rosa_M.RenderModel();

		model = jerarquiaPlanetas;
		model = glm::translate(model, posicionPlanetaGris + glm::vec3(0.0f, vibracionGrisY, 0.0f));
		model = glm::scale(model, glm::vec3(escalaPlanetaGris * pulsoGris, escalaPlanetaGris * pulsoGris, escalaPlanetaGris * pulsoGris));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		PlanetaGris_M.RenderModel();

		model = jerarquiaPlanetas;
		model = glm::translate(model, posicionPlanetaMorado + glm::vec3(0.0f, vibracionMoradoY, 0.0f));
		model = glm::scale(model, glm::vec3(escalaPlanetaMorado * pulsoMorado, escalaPlanetaMorado * pulsoMorado, escalaPlanetaMorado * pulsoMorado));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		PlanetaMorado_M.RenderModel();


		model = jerarquiaPlanetas;
		model = glm::translate(model, posicionPlanetaVerde + glm::vec3(0.0f, vibracionVerdeY, 0.0f));
		model = glm::scale(model, glm::vec3(escalaPlanetaVerde * pulsoVerde, escalaPlanetaVerde * pulsoVerde, escalaPlanetaVerde * pulsoVerde));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		PlanetaVerde_M.RenderModel();

		// Jerarquia padre: Maquinarr.
		// Ya no es hija de jerarquiaPueblo; ahora cuelga directamente de jerarquiaArcane.
		// De esta matriz cuelgan los aros originales, Nave 2 original,
		// la copia de aros y la copia de Nave 2.
		glm::mat4 jerarquiaMaquinarr = jerarquiaArcane;
		jerarquiaMaquinarr = glm::translate(jerarquiaMaquinarr, posicionMaquinarr);
		jerarquiaMaquinarr = glm::rotate(jerarquiaMaquinarr, rotacionMaquinarrY * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		jerarquiaMaquinarr = glm::scale(jerarquiaMaquinarr, glm::vec3(escalaMaquinarr, escalaMaquinarr, escalaMaquinarr));

		float escalaCompensacionMaquinarr = 1.0f / escalaMaquinarr;

		// Maquinarr: padre visual del conjunto.
		model = jerarquiaMaquinarr;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		toffset = glm::vec2(0.0f, 0.0f);
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Maquinarr_M.RenderModel();

		// Grupo base de aros originales.
		// Ahora es hijo de la jerarquia de Maquinarr.
		glm::mat4 matrizAros = jerarquiaMaquinarr;
		matrizAros = glm::translate(matrizAros, posicionGrupoAros);
		matrizAros = glm::rotate(matrizAros, rotacionGrupoArosY * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		matrizAros = glm::scale(matrizAros, glm::vec3(escalaGrupoAros * escalaCompensacionMaquinarr, escalaGrupoAros * escalaCompensacionMaquinarr, escalaGrupoAros * escalaCompensacionMaquinarr));

		modelaux = matrizAros;

		//Padre visual: basearo.
		model = modelaux;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		toffset = glm::vec2(0.0f, 0.0f);
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		BaseAro_M.RenderModel();

		//Hijos: solo reciben transformaciones a partir de la matriz padre de basearo.
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

		// Copia estatica del grupo de aros.
		// Queda a la misma separacion lateral que la copia de Nave 2.
		// Estos aros NO usan offsetAro1/2/3/4 para que no se animen.
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

		// Nave 2
		// Recorre inicio -> media -> final. Luego desaparece y deja un flash.
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

			// Brillo pequeño y discreto: ya no usa rectangulos atravesados ni luz real.
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

		// Copia de Nave 2: aparece con flash y luego recorre el camino contrario.
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

		//Globo principal
		//Queda como hijo de la jerarquia de la estacion.
		model = jerarquiaEstacion;
		model = glm::translate(model, posicionGlobo);
		model = glm::rotate(model, rotacionGloboY * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(escalaGlobo, escalaGlobo, escalaGlobo));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		toffset = glm::vec2(0.0f, 0.0f);
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Globo_M.RenderModel();

		//Globo extra 1: va más arriba y en dirección opuesta.
		//Tambien es hijo de la estacion.
		model = jerarquiaEstacion;
		model = glm::translate(model, posicionGloboArriba);
		model = glm::rotate(model, rotacionGloboArribaY * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(escalaGloboArriba, escalaGloboArriba, escalaGloboArriba));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		toffset = glm::vec2(0.0f, 0.0f);
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Globo_M.RenderModel();

		//Globo extra 2: va más abajo, en la misma dirección, pero desfasado.
		//Tambien es hijo de la estacion.
		model = jerarquiaEstacion;
		model = glm::translate(model, posicionGloboAbajo);
		model = glm::rotate(model, rotacionGloboAbajoY * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(escalaGloboAbajo, escalaGloboAbajo, escalaGloboAbajo));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		toffset = glm::vec2(0.0f, 0.0f);
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Globo_M.RenderModel();

		//Puente del Progreso
		//Queda como hijo de la jerarquia general del pueblo.
		//Sus coordenadas son locales al pueblo y se ajustan con el editor de teclado.
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

		//Grupo Incognium de Zindelo
		//Queda unido como hijo de la jerarquia del pueblo.
		//Dentro de esta jerarquia conserva la base, esfera, malla y luz azul juntas.
		modelaux = jerarquiaIncognium;

		//Base con esfera azul
		model = modelaux;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		toffset = glm::vec2(0.0f, 0.0f);
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		BaseYBolaAzul_M.RenderModel();

		//Malla circular
		//La malla usa la misma matriz padre de la base para moverse junto con ella.
		//Para que no orbite, se rota alrededor de su propio origen/local pivot.
		//En Blender la altura del pivote está en Z, pero en este proyecto el eje vertical es Y.
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

		//Faros con luminaria nocturna.
		//Se renderiza el mismo modelo cuatro veces.
		//Cada faro ya viene unido a jerarquiaPueblo desde jerarquiaFaros[i].
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

		// Cristales y aura brillante de los 4 faros.
		// Esto NO agrega mas luces reales: solo dibuja planos cruzados dentro y alrededor
		// del cristal del farol para que cada uno parezca que emite luz.
		// Se usa meshList[5] porque es un plano cuadrado, no la piramide de meshList[0].
		if (intensidadFaro > 0.0f)
		{
			for (int i = 0; i < 4; i++)
			{
				// Aura exterior vertical 1: mas grande y un poco menos intensa.
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

				// Aura exterior vertical 2, cruzada, para que se vea desde otro angulo.
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

				// Cristal central vertical 1: mas pequeno, muy brillante, dentro del farol.
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

				// Cristal central vertical 2, cruzado.
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

				// Brillo horizontal pequeño dentro del cristal.
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
