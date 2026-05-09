

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
#include "AnimatedModel.h"
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


// --- VARIABLES ARTURO ---
bool avanzaArturo;
float toffsetflechauArturo = 0.0f;
float toffsetflechavArturo = 0.0f;
float toffsetnumerouArturo = 0.0f;
float toffsetnumerovArturo = 0.0f;
float toffsetnumerocambiauArturo = 0.0;

Texture humoTexture;
Texture aguaTexture;

Model Puerta_base_M;
Model Puerta1_M;
Model Puerta_M;
Model Fabrica_M;
Model Centro_M;
Model Volcan_M;
Model Lampara_M;
Model Barco_M;
Model Helice_M;
Model Tren_M;
Model Vias_M;

Model Piernad_as_M;
Model Piernai_as_M;
Model ColaAs_M;
Model As_M;
Model Piernas1_M;
Model Piernas2_M;
Model ColaParis_M;
Model Paris_M;
Model Terry_M;
Model Geodesico_M;
Model Council3_M;

float rotPuertaIzq = 0.0f;
float rotPuertaDer = 0.0f;
bool abrirPuerta = false;
float velocidadPuerta = 0.5f;

float largoRiel = 2.0f;
int piezasRecta = 10;
int piezasCurva = 12;
float anguloGiro = 180.0f / piezasCurva;

float humoOffsetV = 0.0f;
float humoVelocidad = 0.00000002f;
float humoEscalaAnim = 1.0f;
float humoAlphaAnim = 1.0f;
float humoTiempo = 0.0f;

float aguaOffset = 0.0f;

float rotHeliceBarco = 0.0f;

float movBarco_x = 400.0f;
float movBarco_z = 0.0f;
float giroBarco = 90.0f;

bool direccionPierna = true;
float velocidadAS = 0.05f;
float rotacionPierna = 0.0f;
float movimientoCola = 0.0f;
float rotacionPiernaParis = 0.0f;
float movimientoColaParis = 0.0f;

float posX_AS = 40.0f;
float posZ_AS = -40.0f;
float anguloAS = 0.0f;
int ladoAS = 0;
float limiteX_AS = 100.0f;
float limiteZ_AS = 20.0f;
float inicioX_AS = 40.0f;
float inicioZ_AS = -40.0f;

float posX_PARIS = 100.0f;
float posZ_PARIS = -160.0f;
float anguloPARIS = 90.0f;
float velocidadPARIS = 0.06f;
int ladoPARIS = 0;
float limiteX_PARIS = 160.0f;
float limiteZ_PARIS = -100.0f;
float inicioX_PARIS = 100.0f;
float inicioZ_PARIS = -160.0f;

float movAvion_x = 0.0f, movAvion_y = 0.0f;
float giroAvion = 0;

#define MAX_FRAMES 10000
int i_max_steps = 5000;
int i_curr_steps = 6;
typedef struct _frame
{
	float movAvion_x;
	float movAvion_y;
	float movAvion_xInc;
	float movAvion_yInc;
	float giroAvion;
	float giroAvionInc;
	float rotPuertaIzq, rotPuertaDer;
	float rotPuertaIzqInc, rotPuertaDerInc;
	float girobarco;
	float movBarco_x;
	float movBarco_z;
	float girobarcoInc;
	float movBarco_xInc;
	float movBarco_zInc;
} FRAME;

FRAME KeyFrame[MAX_FRAMES];
int FrameIndex = 0;
bool play = false;
int playIndex = 0;

static bool keySpace = false;
static bool key0 = false;
static bool keyL = false;
static bool keyP = false;

void saveFrame(void)
{
	KeyFrame[FrameIndex].movAvion_x = movAvion_x;
	KeyFrame[FrameIndex].movAvion_y = movAvion_y;
	KeyFrame[FrameIndex].giroAvion = giroAvion;
	KeyFrame[FrameIndex].rotPuertaIzq = rotPuertaIzq;
	KeyFrame[FrameIndex].rotPuertaDer = rotPuertaDer;
	KeyFrame[FrameIndex].movBarco_x = movBarco_x;
	KeyFrame[FrameIndex].movBarco_z = movBarco_z;
	FrameIndex++;
}

void resetElements(void)
{
	movAvion_x = KeyFrame[0].movAvion_x;
	movAvion_y = KeyFrame[0].movAvion_y;
	giroAvion = KeyFrame[0].giroAvion;
}

void interpolation(void)
{
	KeyFrame[playIndex].movAvion_xInc = (KeyFrame[playIndex + 1].movAvion_x - KeyFrame[playIndex].movAvion_x) / i_max_steps;
	KeyFrame[playIndex].movAvion_yInc = (KeyFrame[playIndex + 1].movAvion_y - KeyFrame[playIndex].movAvion_y) / i_max_steps;
	KeyFrame[playIndex].giroAvionInc = (KeyFrame[playIndex + 1].giroAvion - KeyFrame[playIndex].giroAvion) / i_max_steps;
	KeyFrame[playIndex].rotPuertaIzqInc = (KeyFrame[playIndex + 1].rotPuertaIzq - KeyFrame[playIndex].rotPuertaIzq) / i_max_steps;
	KeyFrame[playIndex].rotPuertaDerInc = (KeyFrame[playIndex + 1].rotPuertaDer - KeyFrame[playIndex].rotPuertaDer) / i_max_steps;
	KeyFrame[playIndex].movBarco_xInc = (KeyFrame[playIndex + 1].movBarco_x - KeyFrame[playIndex].movBarco_x) / i_max_steps;
	KeyFrame[playIndex].movBarco_zInc = (KeyFrame[playIndex + 1].movBarco_z - KeyFrame[playIndex].movBarco_z) / i_max_steps;
	KeyFrame[playIndex].girobarcoInc = (KeyFrame[playIndex + 1].girobarco - KeyFrame[playIndex].girobarco) / i_max_steps;
}

void animate(void)
{
	if (play)
	{
		if (i_curr_steps >= i_max_steps)
		{
			playIndex++;
			if (playIndex > FrameIndex - 2)
			{
				playIndex = 0;
				play = false;
			}
			else
			{
				i_curr_steps = 0;
				interpolation();
			}
		}
		else
		{
			movAvion_x += KeyFrame[playIndex].movAvion_xInc;
			movAvion_y += KeyFrame[playIndex].movAvion_yInc;
			giroAvion += KeyFrame[playIndex].giroAvionInc;
			rotPuertaIzq += KeyFrame[playIndex].rotPuertaIzqInc;
			rotPuertaDer += KeyFrame[playIndex].rotPuertaDerInc;
			movBarco_x += KeyFrame[playIndex].movBarco_xInc;
			movBarco_z += KeyFrame[playIndex].movBarco_zInc;
			giroBarco += KeyFrame[playIndex].girobarcoInc;
			i_curr_steps++;
		}
	}
}

void RenderHumo(
	glm::mat4 modelBase,
	glm::vec3 posLocal,
	glm::vec3 escalaLocal,
	GLuint uniformModel,
	GLuint uniformOffset,
	GLuint uniformAlpha)
{
	glm::mat4 model = modelBase;
	model = glm::translate(model, posLocal);
	model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::scale(model, escalaLocal * humoEscalaAnim);
	glUniform2f(uniformOffset, 0.0f, humoOffsetV);
	glUniform1f(uniformAlpha, humoAlphaAnim);
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	meshList[3]->RenderMesh();
}

void inputKeyframesArturo(bool* keys) {
	if (keys[GLFW_KEY_O]) {
		abrirPuerta = true;
	}
	if (keys[GLFW_KEY_C]) {
		abrirPuerta = false;
	}
	if (keys[GLFW_KEY_L] && !keyL) {
		saveFrame();
		keyL = true;
	}
	if (!keys[GLFW_KEY_L]) keyL = false;

	if (keys[GLFW_KEY_B] && !keySpace) {
		play = !play;
		if (play) interpolation();
		keySpace = true;
	}
	if (!keys[GLFW_KEY_B]) keySpace = false;
}

void renderRiel(glm::mat4 m, GLuint uniformLoc, Model& modelObj) {
	glUniformMatrix4fv(uniformLoc, 1, GL_FALSE, glm::value_ptr(m));
	modelObj.RenderModel();
}
// --- FIN VARIABLES ARTURO ---


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
Model Banca_M;
Model Arbol_M;

Model CabezaGabu_M;
Model PiernaDerGabu_M;
Model PiernaIzqGabu_M;
Model PiernaTrasDerGabu_M;
Model PiernaTrasIzqGabu_M;
Model TorsoGabu_M;

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

Model PlanetaMorado001_M;
Model PlanetaAmarillo_M;
Model PlanetaAzul_M;
Model PlanetaGris_M;
Model PlanetaMorado_M;
Model PlanetaVerde_M;

Model Rosa_M;

AnimatedModel Jayce_M;
AnimatedModel Vi_M;
AnimatedModel Zorro_M;
AnimatedModel Reptil_M;
AnimatedModel Rex_M;
AnimatedModel Tirnanog_M;
AnimatedModel IndustrialBuilding_M;
AnimatedModel Coliseo_M;
AnimatedModel Mils_M;
AnimatedModel SolBadguy_M;
AnimatedModel KyKiske_M;
AnimatedModel Loris_M;
Shader shaderAnimado;

Model PrincipitoBrazoDer_M;
Model PrincipitoBrazoIzq_M;
Model PrincipitoCabeza_M;
Model PrincipitoPiernaDer_M;
Model PrincipitoPiernaIzq_M;
Model PrincipitoTorso_M;

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
static const char* vShaderAnimated = "shaders/shader_animated.vert";
static const char* fShaderAnimated = "shaders/shader_animated.frag";

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
		-10.0f, 0.0f, -10.0f,	1.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, -10.0f,	0.0f, 0.0f,	0.0f, -1.0f, 0.0f,
		-10.0f, 0.0f, 10.0f,	1.0f, 1.0f,	0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, 10.0f,		0.0f, 1.0f,	0.0f, -1.0f, 0.0f
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
	shaderAnimado.CreateFromFiles(vShaderAnimated, fShaderAnimated);

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


	humoTexture = Texture("Textures/humoproyecto.tga");
	humoTexture.LoadTextureHumo();
	aguaTexture = Texture("Textures/mar.tga");
	aguaTexture.LoadTextureA();

	Puerta_M = Model();
	Puerta_M.LoadModel("Models/PUERTAI.obj");
	Puerta_base_M = Model();
	Puerta_base_M.LoadModel("Models/PUERTABASE.obj");
	Puerta1_M = Model();
	Puerta1_M.LoadModel("Models/PUERTAD.obj");
	Fabrica_M = Model();
	Fabrica_M.LoadModel("Models/Fabric.obj");
	Centro_M = Model();
	Centro_M.LoadModel("Models/centrovisitantesfinal.obj");
	Volcan_M = Model();
	Volcan_M.LoadModel("Models/volcan.obj");
	Barco_M = Model();
	Barco_M.LoadModel("Models/bote.obj");

	Piernad_as_M = Model();
	Piernad_as_M.LoadModel("Models/PATDERAS.obj");
	Piernai_as_M = Model();
	Piernai_as_M.LoadModel("Models/PATIZQAS.obj");
	ColaAs_M = Model();
	ColaAs_M.LoadModel("Models/COLAAS.obj");
	As_M = Model();
	As_M.LoadModel("Models/AS.obj");
	Piernas1_M = Model();
	Piernas1_M.LoadModel("Models/Piernas1.obj");
	Piernas2_M = Model();
	Piernas2_M.LoadModel("Models/Piernas2.obj");
	ColaParis_M = Model();
	ColaParis_M.LoadModel("Models/colaparis.obj");
	Paris_M = Model();
	Paris_M.LoadModel("Models/paris.obj");
	Terry_M = Model();
	Terry_M.LoadModel("Models/ModelTERRY.obj");
	Helice_M = Model();
	Helice_M.LoadModel("Models/helicebote.obj");

	Geodesico_M = Model();
	Geodesico_M.LoadModel("Models/geodesico.obj");

	Council3_M = Model();
	Council3_M.LoadModel("Models/council3.obj");

	BaseYBolaAzul_M = Model();
	BaseYBolaAzul_M.LoadModel("Models/baseybolaazul.obj");

	MallaCircular_M = Model();
	MallaCircular_M.LoadModel("Models/mallacircular.obj");

	Faro_M = Model();
	Faro_M.LoadModel("Models/faro.obj");

	Banca_M = Model();
	Banca_M.LoadModel("Models/banca.obj");

	Arbol_M = Model();
	Arbol_M.LoadModel("Models/arbol.obj");

	CabezaGabu_M = Model();
	CabezaGabu_M.LoadModel("Models/cabezagabu.obj");

	PiernaDerGabu_M = Model();
	PiernaDerGabu_M.LoadModel("Models/piernadergabu.obj");

	PiernaIzqGabu_M = Model();
	PiernaIzqGabu_M.LoadModel("Models/piernaizqgabu.obj");

	PiernaTrasDerGabu_M = Model();
	PiernaTrasDerGabu_M.LoadModel("Models/piernatrasdergabu.obj");

	PiernaTrasIzqGabu_M = Model();
	PiernaTrasIzqGabu_M.LoadModel("Models/piernatrasizqgabu.obj");

	TorsoGabu_M = Model();
	TorsoGabu_M.LoadModel("Models/torosgabu.obj");

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


	Rosa_M = Model();
	Rosa_M.LoadModel("Models/rosa.obj");

	Jayce_M = AnimatedModel();
	Jayce_M.LoadModel("Models/jayce.glb");

	Vi_M = AnimatedModel();
	Vi_M.LoadModel("Models/arcane_undercity_vi.glb");

	Zorro_M = AnimatedModel();
	Zorro_M.LoadModel("Models/zorro.glb");

	Reptil_M = AnimatedModel();
	Reptil_M.LoadModel("Models/reptil.glb");

	Rex_M = AnimatedModel();
	Rex_M.LoadModel("Models/rex.glb");

	Tirnanog_M = AnimatedModel();
	Tirnanog_M.LoadModel("Models/tirnanog.glb");

	IndustrialBuilding_M = AnimatedModel();
	IndustrialBuilding_M.LoadModel("Models/industrial_building.glb");

	Coliseo_M = AnimatedModel();
	Coliseo_M.LoadModel("Models/coliseo.glb");

	Mils_M = AnimatedModel();
	Mils_M.LoadModel("Models/mils.glb");

	SolBadguy_M = AnimatedModel();
	SolBadguy_M.LoadModel("Models/solbadguy.glb");

	KyKiske_M = AnimatedModel();
	KyKiske_M.LoadModel("Models/kykiske.glb");

	Loris_M = AnimatedModel();
	Loris_M.LoadModel("Models/loris.glb");


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


	mainLight = DirectionalLight(1.0f, 1.0f, 1.0f,
		0.5f, 0.5f,
		0.0f, -1.0f, -1.0f);

	unsigned int pointLightCount = 0;


	pointLights[0] = PointLight(0.0f, 0.45f, 1.0f,
		0.0f, 0.0f,
		0.0f, 0.0f, 0.0f,
		1.0f, 0.025f, 0.0035f);
	pointLightCount++;



	pointLights[1] = PointLight(0.0f, 0.35f, 1.0f,
		0.7f, 2.8f,
		0.0f, 0.0f, 0.0f,
		1.0f, 0.025f, 0.006f);
	pointLightCount++;



	unsigned int spotLightCount = 0;

	spotLights[0] = SpotLight(1.0f, 1.0f, 1.0f,
		0.0f, 2.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		5.0f);
	spotLightCount++;

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



	bool luzIncogniumActiva = true;
	bool luzEstacionActiva = true;
	bool teclaILiberada = true;
	bool lucesInteractivasActivas = true;


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



	glm::vec3 posicionBanca = glm::vec3(0.0f, -2.0f, 0.0f);
	float rotacionBancaY = 0.0f;
	float escalaBanca = 1.0f;

	const int NUM_BANCAS_EXTRA = 18;
	glm::vec3 posicionesBancasExtra[NUM_BANCAS_EXTRA] = {
		// Entrada Jurassic / puerta
		glm::vec3(39.0f, -2.0f, -96.0f),
		glm::vec3(86.0f, -2.0f, -96.0f),
		glm::vec3(39.0f, -2.0f, -69.0f),
		glm::vec3(86.0f, -2.0f, -69.0f),
		// Camino hacia geodesico / fabrica
		glm::vec3(128.0f, -2.0f, -188.0f),
		glm::vec3(212.0f, -2.0f, -222.0f),
		glm::vec3(183.0f, -2.0f, -244.0f),
		glm::vec3(244.0f, -2.0f, -242.0f),
		// Plaza del Incognium
		glm::vec3(-36.0f, -2.0f, 44.0f),
		glm::vec3(36.0f, -2.0f, 44.0f),
		glm::vec3(-36.0f, -2.0f, -44.0f),
		glm::vec3(36.0f, -2.0f, -44.0f),
		glm::vec3(-50.0f, -2.0f, 0.0f),
		glm::vec3(50.0f, -2.0f, 0.0f),
		// Consejo / coliseo / tirnanog
		glm::vec3(214.0f, -2.0f, 188.0f),
		glm::vec3(286.0f, -2.0f, 188.0f),
		glm::vec3(214.0f, -2.0f, 252.0f),
		glm::vec3(286.0f, -2.0f, 252.0f)
	};
	float rotacionesBancasExtra[NUM_BANCAS_EXTRA] = {
		0.0f, 0.0f, 180.0f, 180.0f,
		90.0f, 90.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 180.0f, 180.0f, 90.0f, 270.0f,
		0.0f, 0.0f, 180.0f, 180.0f
	};

	const int NUM_FAROS_EXTRA = 12;
	glm::vec3 posicionesFarosExtra[NUM_FAROS_EXTRA] = {
		glm::vec3(-35.0f, -2.0f, 35.0f),
		glm::vec3(35.0f, -2.0f, 35.0f),
		glm::vec3(-35.0f, -2.0f, -35.0f),
		glm::vec3(35.0f, -2.0f, -35.0f),
		glm::vec3(40.0f, -2.0f, -105.0f),
		glm::vec3(85.0f, -2.0f, -105.0f),
		glm::vec3(135.0f, -2.0f, -190.0f),
		glm::vec3(210.0f, -2.0f, -230.0f),
		glm::vec3(215.0f, -2.0f, 185.0f),
		glm::vec3(285.0f, -2.0f, 185.0f),
		glm::vec3(215.0f, -2.0f, 255.0f),
		glm::vec3(285.0f, -2.0f, 255.0f)
	};
	float escalaFaroExtra = 1.35f;

	const int NUM_ARBOLES_EXTRA = 18;
	glm::vec3 posicionesArbolesExtra[NUM_ARBOLES_EXTRA] = {
		glm::vec3(-110.0f, -2.0f, 150.0f),
		glm::vec3(-65.0f, -2.0f, 170.0f),
		glm::vec3(-15.0f, -2.0f, 165.0f),
		glm::vec3(55.0f, -2.0f, 155.0f),
		glm::vec3(125.0f, -2.0f, 145.0f),
		glm::vec3(180.0f, -2.0f, 132.0f),
		glm::vec3(-320.0f, -2.0f, -210.0f),
		glm::vec3(-285.0f, -2.0f, -245.0f),
		glm::vec3(-225.0f, -2.0f, -275.0f),
		glm::vec3(-120.0f, -2.0f, -300.0f),
		glm::vec3(-40.0f, -2.0f, -310.0f),
		glm::vec3(55.0f, -2.0f, -285.0f),
		glm::vec3(320.0f, -2.0f, -180.0f),
		glm::vec3(350.0f, -2.0f, -95.0f),
		glm::vec3(335.0f, -2.0f, 25.0f),
		glm::vec3(315.0f, -2.0f, 120.0f),
		glm::vec3(155.0f, -2.0f, 275.0f),
		glm::vec3(-130.0f, -2.0f, 260.0f)
	};
	float escalaArbolExtra = 28.0f;

	glm::vec3 posicionGabu = glm::vec3(0.14f, -1.98f, -0.83f);
	float rotacionGabuY = 0.0f;
	float escalaGabu = 0.214f;
	float velocidadMoverGabu = 0.5f;
	float velocidadAlturaGabu = 0.35f;
	float velocidadEscalaGabu = 0.08f;
	float escalaMinimaGabu = 0.01f;
	float escalaMaximaGabu = 80.0f;
	float velocidadRotacionGabu = 8.0f;
	float tiempoImpresionGabu = 0.0f;



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

	float velocidadPasoBlitz = 7.5f;
	float amplitudPasoBrazoBlitz = 8.0f;
	float amplitudPasoPiernaBlitz = 6.0f;



	glm::vec3 posicionArcane = glm::vec3(-197.77f, -0.38f, 66.08f);
	float rotacionArcaneY = -360.35f;
	float escalaArcane = 2.09f;



	glm::vec3 posicionGrupoPueblo = glm::vec3(0.0f, 0.0f, 0.0f);
	float rotacionGrupoPuebloY = 0.0f;
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



	glm::vec3 posicionEstacion = glm::vec3(0.0f, 0.0f, 0.0f);
	float rotacionEstacionY = 0.0f;
	float escalaEstacion = 1.0f;



	glm::vec3 posicionAireEstacion = glm::vec3(-2.70f, 43.24f, -0.24f);
	float rotacionAireEstacionY = -1433.31f;
	float escalaAireEstacion = 68.88f;



	glm::vec3 posicionEstacionBaseRuta = glm::vec3(71.51f, 3.07f, -79.38f);
	float rotacionEstacionBaseRutaY = -1433.31f;
	float escalaEstacionBaseRuta = 39.63f;




	float velocidadMoverEstacion = 2.5f;
	float velocidadAlturaEstacion = 1.5f;
	float velocidadEscalaEstacion = 10.0f;
	float escalaMinimaEstacion = 1.0f;
	float escalaMaximaEstacion = 200.0f;
	float velocidadRotacionEstacion = 35.0f;
	float tiempoImpresionEstacion = 0.0f;



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



	float factorLocalEstacion = 1.0f / escalaEstacionBaseRuta;
	glm::mat4 matrizLocalEstacionInicial = glm::mat4(1.0f);
	matrizLocalEstacionInicial = glm::translate(matrizLocalEstacionInicial, posicionEstacionBaseRuta);
	matrizLocalEstacionInicial = glm::rotate(matrizLocalEstacionInicial, rotacionEstacionBaseRutaY * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	matrizLocalEstacionInicial = glm::scale(matrizLocalEstacionInicial, glm::vec3(escalaEstacionBaseRuta, escalaEstacionBaseRuta, escalaEstacionBaseRuta));
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




	glm::vec3 posicionMaquinarr = glm::vec3(50.60f, 0.38f, 83.71f);
	float rotacionMaquinarrY = 2701.93f;
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


	glm::vec3 posicionPlanetas = glm::vec3(7.77f, 133.38f, -3.01f);
	float rotacionPlanetasY = -2321.02f;
	float escalaPlanetas = 6.88f;

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


	glm::vec3 posicionPrincipito = glm::vec3(-2.68f, 274.28f, -333.72f);
	float rotacionPrincipitoY = -199.62f;
	float escalaPrincipito = 1.34f;


	glm::vec3 posicionBrazoDerPrincipito = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 posicionBrazoIzqPrincipito = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 posicionCabezaPrincipito = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 posicionPiernaDerPrincipito = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 posicionPiernaIzqPrincipito = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 posicionTorsoPrincipito = glm::vec3(0.0f, 0.0f, 0.0f);




	glm::vec3 pivoteHombro = glm::vec3(0.0f, 1.15f, 0.0f);
	glm::vec3 pivoteHombroDerPrincipito = pivoteHombro + glm::vec3(0.34f, 0.0f, 0.0f);
	glm::vec3 pivoteHombroIzqPrincipito = pivoteHombro + glm::vec3(-0.34f, 0.0f, 0.0f);



	glm::vec3 pivoteCadera = glm::vec3(0.0f, 0.60f, 0.0f);
	glm::vec3 pivoteCaderaDerPrincipito = pivoteCadera + glm::vec3(0.14f, 0.0f, 0.0f);
	glm::vec3 pivoteCaderaIzqPrincipito = pivoteCadera + glm::vec3(-0.14f, 0.0f, 0.0f);

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



	glm::vec3 posicionRosa = glm::vec3(-18.25f, 11.23f, -28.35f);
	float rotacionRosaY = 2321.02f;
	float escalaRosa = 0.817f;
	float velocidadMoverRosa = 1.5f;
	float velocidadAlturaRosa = 0.9f;
	float velocidadEscalaRosa = 0.30f;
	float escalaMinimaRosa = 0.05f;
	float escalaMaximaRosa = 200.0f;
	float velocidadRotacionRosa = 0.70f;
	float tiempoImpresionRosa = 0.0f;







	glm::vec3 jayceInicio = glm::vec3(3.28f, 74.35f, -12.59f);
	glm::vec3 jayceFinal = glm::vec3(5.98f, -2.01f, -16.11f);
	glm::vec3 posicionJayce = jayceInicio;
	float rotacionJayceY = 153.20f;
	float escalaJayce = 0.01f;
	bool recorridoJayceActivo = false;
	bool animacionJayceActiva = false;
	bool tecla3JayceLiberada = true;
	int faseRecorridoJayce = 0;
	float tiempoRecorridoJayce = 0.0f;
	float duracionJayceRecorrido = 12.00f;



	float progresoFisicoAterrizajeJayce = 0.75f;
	float progresoAnimAterrizajeJayce = 0.70f;
	float alturaExtraSaltoJayce = 9.0f;
	float proporcionSubidaJayce = 0.24f;
	float velocidadMoverJayce = 0.15f;
	float velocidadAlturaJayce = 0.15f;
	float velocidadEscalaJayce = 0.15f;
	float escalaMinimaJayce = 0.01f;
	float escalaMaximaJayce = 200.0f;
	float velocidadRotacionJayce = 0.15f;
	float tiempoImpresionJayce = 0.0f;


	glm::vec3 posicionVi = glm::vec3(-183.59f, -1.98f, 91.20f);
	float rotacionViY = 48.98f;
	float escalaVi = 0.015f;
	float velocidadMoverVi = 0.15f;
	float velocidadAlturaVi = 0.15f;
	float velocidadEscalaVi = 0.015f;
	float escalaMinimaVi = 0.001f;
	float escalaMaximaVi = 200.0f;
	float velocidadRotacionVi = 0.15f;
	float tiempoImpresionVi = 0.0f;

	glm::vec3 posicionZorro = glm::vec3(-261.23f, 267.66f, 152.85f);
	float rotacionZorroY = 0.0f;
	float rotacionZorroX = -90.0f;
	float escalaZorro = 0.33075f;
	float velocidadMoverZorro = 0.15f;
	float velocidadAlturaZorro = 0.15f;
	float velocidadEscalaZorro = 0.005f;
	float escalaMinimaZorro = 0.001f;
	float escalaMaximaZorro = 200.0f;
	float velocidadRotacionZorro = 0.15f;
	float tiempoImpresionZorro = 0.0f;




	glm::vec3 posicionVolcan = glm::vec3(367.70f, -70.66f, -516.27f);
	float rotacionVolcanY = -4971.55f;
	float escalaVolcan = 5.18f;
	float velocidadMoverVolcan = 5.0f;
	float velocidadAlturaVolcan = 5.0f;
	float velocidadEscalaVolcan = 0.5f;
	float escalaMinimaVolcan = 0.5f;
	float escalaMaximaVolcan = 100.0f;
	float velocidadRotacionVolcan = 5.0f;
	float tiempoImpresionVolcan = 0.0f;

	glm::vec3 posicionFabrica = glm::vec3(266.07f, -2.00f, -252.86f);
	float rotacionFabricaY = -631.28f;
	float escalaFabrica = 0.59f;
	float velocidadMoverFabrica = 5.0f;
	float velocidadAlturaFabrica = 5.0f;
	float velocidadEscalaFabrica = 0.05f;
	float escalaMinimaFabrica = 0.01f;
	float escalaMaximaFabrica = 20.0f;
	float velocidadRotacionFabrica = 5.0f;
	float tiempoImpresionFabrica = 0.0f;

	glm::vec3 posicionPuertaJurassic = glm::vec3(61.70f, -2.00f, -74.75f);
	float rotacionPuertaJurassicY = -399.88f;
	float escalaPuertaJurassic = 1.51f;
	float velocidadMoverPuertaJurassic = 5.0f;
	float velocidadAlturaPuertaJurassic = 5.0f;
	float velocidadEscalaPuertaJurassic = 0.05f;
	float escalaMinimaPuertaJurassic = 0.01f;
	float escalaMaximaPuertaJurassic = 20.0f;
	float velocidadRotacionPuertaJurassic = 5.0f;
	float tiempoImpresionPuertaJurassic = 0.0f;

	glm::vec3 posicionCentroJurassic = glm::vec3(140.59f, -11.04f, -251.90f);
	float rotacionCentroJurassicY = -452.68f;
	float escalaCentroJurassic = 3.40f;
	float velocidadMoverCentroJurassic = 5.0f;
	float velocidadAlturaCentroJurassic = 5.0f;
	float velocidadEscalaCentroJurassic = 0.05f;
	float escalaMinimaCentroJurassic = 0.01f;
	float escalaMaximaCentroJurassic = 20.0f;
	float velocidadRotacionCentroJurassic = 5.0f;
	float tiempoImpresionCentroJurassic = 0.0f;


	glm::vec3 posicionGeodesico = glm::vec3(195.34f, -2.00f, -253.70f);
	float rotacionGeodesicoY = -143.74f;
	float escalaGeodesico = 0.73f;
	float velocidadMoverGeodesico = 5.0f;
	float velocidadAlturaGeodesico = 5.0f;
	float velocidadEscalaGeodesico = 0.05f;
	float escalaMinimaGeodesico = 0.01f;
	float escalaMaximaGeodesico = 100.0f;
	float velocidadRotacionGeodesico = 5.0f;
	float tiempoImpresionGeodesico = 0.0f;

	glm::vec3 posicionReptil = glm::vec3(80.00f, 6.26f, -110.00f);
	float rotacionReptilY = -748.56f;
	float rotacionReptilX = 0.0f;
	float escalaReptil = 0.0800f;
	float velocidadMoverReptil = 5.0f;
	float velocidadAlturaReptil = 5.0f;
	float velocidadEscalaReptil = 0.01f;
	float escalaMinimaReptil = 0.001f;
	float escalaMaximaReptil = 10.0f;
	float velocidadRotacionReptil = 5.0f;
	float tiempoImpresionReptil = 0.0f;

	glm::vec3 rexInicio = glm::vec3(77.28f, -2.00f, -240.57f);
	glm::vec3 rexFinal = glm::vec3(238.64f, -2.00f, -61.79f);
	glm::vec3 rexControlRegreso = glm::vec3(213.71f, -2.00f, -91.62f);
	float rexRotIda = 48.50f;
	float rexRotRegreso = -129.41f;
	glm::vec3 posicionRex = rexInicio;
	float rotacionRexY = rexRotIda;
	float rotacionRexX = 0.0f;
	float escalaRex = 0.0800f;
	float tiempoAnimacionRex = 0.0f;
	float duracionIdaRex = 6.0f;
	float duracionGiroFinalRex = 1.35f;
	float duracionRegresoRex = 6.0f;
	float duracionGiroInicioRex = 1.35f;
	bool animacionRexActiva = true;

	float velocidadMoverRex = 5.0f;
	float velocidadAlturaRex = 5.0f;
	float velocidadEscalaRex = 0.01f;
	float escalaMinimaRex = 0.001f;
	float escalaMaximaRex = 10.0f;
	float velocidadRotacionRex = 5.0f;
	float tiempoImpresionRex = 0.0f;

	glm::vec3 posicionTirnanog = glm::vec3(-168.98f, -0.03f, -226.72f);
	float rotacionTirnanogY = -669.45f;
	float rotacionTirnanogX = 0.0f;
	float escalaTirnanog = 2.9601f;
	float velocidadMoverTirnanog = 5.0f;
	float velocidadAlturaTirnanog = 5.0f;
	float velocidadEscalaTirnanog = 0.01f;
	float escalaMinimaTirnanog = 0.001f;
	float escalaMaximaTirnanog = 10.0f;
	float velocidadRotacionTirnanog = 5.0f;
	float tiempoImpresionTirnanog = 0.0f;

	glm::vec3 posicionCouncil3 = glm::vec3(251.25f, -2.00f, 220.85f);
	float rotacionCouncil3Y = -630.43f;
	float escalaCouncil3 = 2.1736f;
	float velocidadMoverCouncil3 = 5.0f;
	float velocidadAlturaCouncil3 = 5.0f;
	float velocidadEscalaCouncil3 = 0.05f;
	float escalaMinimaCouncil3 = 0.01f;
	float escalaMaximaCouncil3 = 100.0f;
	float velocidadRotacionCouncil3 = 5.0f;
	float tiempoImpresionCouncil3 = 0.0f;

	glm::vec3 posicionIndustrialBuilding = glm::vec3(219.02f, 20.14f, 53.43f);
	float rotacionIndustrialBuildingY = -989.77f;
	float rotacionIndustrialBuildingX = 0.0f;
	float escalaIndustrialBuilding = 4.6449f;
	float velocidadMoverIndustrialBuilding = 5.0f;
	float velocidadAlturaIndustrialBuilding = 5.0f;
	float velocidadEscalaIndustrialBuilding = 0.05f;
	float escalaMinimaIndustrialBuilding = 0.001f;
	float escalaMaximaIndustrialBuilding = 100.0f;
	float velocidadRotacionIndustrialBuilding = 5.0f;
	float tiempoImpresionIndustrialBuilding = 0.0f;

	glm::vec3 posicionColiseo = glm::vec3(-269.28f, -2.00f, -88.08f);
	float rotacionColiseoY = -943.76f;
	float rotacionColiseoX = 0.0f;
	float escalaColiseo = 0.9285f;
	float velocidadMoverColiseo = 5.0f;
	float velocidadAlturaColiseo = 5.0f;
	float velocidadEscalaColiseo = 0.05f;
	float escalaMinimaColiseo = 0.001f;
	float escalaMaximaColiseo = 100.0f;
	float velocidadRotacionColiseo = 5.0f;
	float tiempoImpresionColiseo = 0.0f;

	// Mils colocada con el ultimo valor impreso.
	glm::vec3 posicionMils = glm::vec3(-26.62f, 140.30f, -9.38f);
	float rotacionMilsY = -424.63f;
	float rotacionMilsX = 0.0f;
	float escalaMils = 0.0462f;
	float velocidadMoverMils = 5.0f;
	float velocidadAlturaMils = 5.0f;
	float velocidadEscalaMils = 0.05f;
	float escalaMinimaMils = 0.001f;
	float escalaMaximaMils = 100.0f;
	float velocidadRotacionMils = 5.0f;
	float tiempoImpresionMils = 0.0f;

	glm::vec3 posicionSolBadguy = glm::vec3(240.93f, -0.90f, 174.51f);
	float rotacionSolBadguyY = -185.43f;
	float rotacionSolBadguyX = 0.0f;
	float escalaSolBadguy = 0.0260f;
	float velocidadMoverSolBadguy = 5.0f;
	float velocidadAlturaSolBadguy = 5.0f;
	float velocidadEscalaSolBadguy = 0.05f;
	float escalaMinimaSolBadguy = 0.001f;
	float escalaMaximaSolBadguy = 100.0f;
	float velocidadRotacionSolBadguy = 5.0f;
	float tiempoImpresionSolBadguy = 0.0f;

	glm::vec3 posicionKyKiske = glm::vec3(-189.41f, 1.06f, -237.01f);
	float rotacionKyKiskeY = 0.0f;
	float rotacionKyKiskeX = 0.0f;
	float escalaKyKiske = 0.0300f;
	float velocidadMoverKyKiske = 2.0f;
	float velocidadAlturaKyKiske = 2.0f;
	float velocidadEscalaKyKiske = 0.005f;
	float escalaMinimaKyKiske = 0.001f;
	float escalaMaximaKyKiske = 10.0f;
	float velocidadRotacionKyKiske = 2.0f;
	float tiempoImpresionKyKiske = 0.0f;

	glm::vec3 lorisP1 = glm::vec3(-155.83f, -2.00f, 19.70f);
	glm::vec3 lorisP2 = glm::vec3(-155.83f, -2.00f, 111.16f);
	glm::vec3 lorisP3 = glm::vec3(-246.23f, -2.00f, 111.16f);
	glm::vec3 lorisP4 = glm::vec3(-246.23f, -2.00f, 16.29f);
	glm::vec3 posicionLoris = lorisP1;
	float rotacionLorisY = 0.0f;
	float rotacionLorisX = 0.0f;
	float escalaLoris = 0.0157f;
	float tiempoAnimacionLoris = 0.0f;
	float duracionLorisTramo1 = 8.0f;
	float duracionLorisGiro1 = 1.8f;
	float duracionLorisTramo2 = 8.0f;
	float duracionLorisGiro2 = 1.8f;
	float duracionLorisTramo3 = 8.0f;
	float duracionLorisGiro3 = 1.8f;
	float duracionLorisTramo4 = 8.0f;
	float duracionLorisGiro4 = 1.8f;
	bool animacionLorisActiva = true;

	float velocidadMoverLoris = 2.0f;
	float velocidadAlturaLoris = 2.0f;
	float velocidadEscalaLoris = 0.005f;
	float escalaMinimaLoris = 0.001f;
	float escalaMaximaLoris = 10.0f;
	float velocidadRotacionLoris = 2.0f;
	float tiempoImpresionLoris = 0.0f;

	int cameraIndex = 0;
	bool teclaTabLiberada = true;
	float velocidadModeloTPS = 0.5f;
	float velocidadRotacionModeloTPS = 8.0f;
	float velocidadVueloPrincipito = 0.5f;
	glm::vec3 posicionCamaraAerea = glm::vec3(0.0f, 120.0f, 0.0f);
	float rotacionCamaraAereaY = -90.0f;
	float rotacionCamaraAereaPitch = -65.0f;
	float velocidadCamaraAerea = 0.5f;
	float velocidadAlturaCamaraAerea = 0.5f;
	float velocidadRotacionCamaraAerea = 8.0f;


	float tiempoMovimientoBlitz = 0.0f;
	float tiempoMovimientoGabu = 0.0f;
	float tiempoMovimientoPrincipito = 0.0f;


	KeyFrame[0].movBarco_x = 400.0f;  KeyFrame[0].movBarco_z = 0.0f;   KeyFrame[0].girobarco = 0.0f;
	KeyFrame[1].movBarco_x = 400.0f;  KeyFrame[1].movBarco_z = 800.0f; KeyFrame[1].girobarco = 0.0f;
	KeyFrame[2].movBarco_x = 400.0f;  KeyFrame[2].movBarco_z = 800.0f; KeyFrame[2].girobarco = -90.0f;
	KeyFrame[3].movBarco_x = -400.0f; KeyFrame[3].movBarco_z = 800.0f; KeyFrame[3].girobarco = -90.0f;
	KeyFrame[4].movBarco_x = -400.0f; KeyFrame[4].movBarco_z = 800.0f; KeyFrame[4].girobarco = -180.0f;
	KeyFrame[5].movBarco_x = -400.0f; KeyFrame[5].movBarco_z = -800.0f; KeyFrame[5].girobarco = -180.0f;
	KeyFrame[6].movBarco_x = -400.0f; KeyFrame[6].movBarco_z = -800.0f; KeyFrame[6].girobarco = -270.0f;
	KeyFrame[7].movBarco_x = 400.0f;  KeyFrame[7].movBarco_z = -800.0f; KeyFrame[7].girobarco = -270.0f;
	KeyFrame[8].movBarco_x = 400.0f;  KeyFrame[8].movBarco_z = -800.0f; KeyFrame[8].girobarco = -360.0f;
	KeyFrame[9].movBarco_x = 400.0f;  KeyFrame[9].movBarco_z = 0.0f;   KeyFrame[9].girobarco = -360.0f;
	FrameIndex = 10;

	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime();
		GLfloat frameTime = now - lastTime;
		deltaTime = frameTime;
		deltaTime += frameTime / limitFPS;
		lastTime = now;

		angulovaria += 0.5f * deltaTime;


		humoTiempo += deltaTime;
		humoOffsetV += humoVelocidad * deltaTime;
		if (humoOffsetV > 1.0f) humoOffsetV -= 1.0f;
		humoEscalaAnim = 1.0f + sin(humoTiempo * 0.5f) * 0.005f;
		humoAlphaAnim = 0.7f + sin(humoTiempo * 5.2f) * 0.03f;
		aguaOffset += 0.001f * deltaTime;
		if (aguaOffset > 1.0f) aguaOffset = 0.0f;
		rotHeliceBarco += 10.0f * deltaTime;
		if (rotHeliceBarco > 360.0f) rotHeliceBarco -= 360.0f;

		inputKeyframesArturo(mainWindow.getsKeys());
		animate();

		float tiempoActual = glfwGetTime();

		// Animación de piernas y cola AS
		rotacionPierna = sin(tiempoActual * 8.0f) * 16.0f;
		movimientoCola = sin(tiempoActual * 2.0f) * 8.0f;

		// Animación de piernas y cola PARIS
		rotacionPiernaParis = sin(tiempoActual * 4.0f) * 16.0f;
		movimientoColaParis = sin(tiempoActual * 2.0f) * 8.0f;

		// MOVIMIENTO RECTANGULAR AS
		switch (ladoAS)
		{
		case 0: // DERECHA
			posX_AS += velocidadAS;
			anguloAS = 90.0f;
			if (posX_AS >= limiteX_AS) ladoAS = 1;
			break;
		case 1: // ABAJO
			posZ_AS -= velocidadAS;
			anguloAS = 180.0f;
			if (posZ_AS <= inicioZ_AS) ladoAS = 2;
			break;
		case 2: // IZQUIERDA
			posX_AS -= velocidadAS;
			anguloAS = 270.0f;
			if (posX_AS <= inicioX_AS) ladoAS = 3;
			break;
		case 3: // ARRIBA
			posZ_AS += velocidadAS;
			anguloAS = 0.0f;
			if (posZ_AS >= limiteZ_AS) ladoAS = 0;
			break;
		}

		// MOVIMIENTO RECTANGULAR PARIS
		switch (ladoPARIS)
		{
		case 0:
			posX_PARIS += velocidadPARIS;
			anguloPARIS = 90.0f;
			if (posX_PARIS >= limiteX_PARIS) ladoPARIS = 1;
			break;
		case 1:
			posZ_PARIS -= velocidadPARIS;
			anguloPARIS = 180.0f;
			if (posZ_PARIS <= inicioZ_PARIS) ladoPARIS = 2;
			break;
		case 2:
			posX_PARIS -= velocidadPARIS;
			anguloPARIS = 270.0f;
			if (posX_PARIS <= inicioX_PARIS) ladoPARIS = 3;
			break;
		case 3:
			posZ_PARIS += velocidadPARIS;
			anguloPARIS = 0.0f;
			if (posZ_PARIS >= limiteZ_PARIS) ladoPARIS = 0;
			break;
		}

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



		bool* keysCamara = mainWindow.getsKeys();
		bool blitzMovimientoTPS = false;
		bool gabuMovimientoTPS = false;
		bool principitoMovimientoTPS = false;
		if (keysCamara[GLFW_KEY_TAB])
		{
			if (teclaTabLiberada)
			{
				cameraIndex++;
				if (cameraIndex > 4)
					cameraIndex = 0;

				printf("Camara activa: %d\n", cameraIndex + 1);
				if (cameraIndex == 0) printf("Camara 1: libre.\n");
				if (cameraIndex == 1) printf("Camara 2: tercera persona Blitzcrank.\n");
				if (cameraIndex == 2) printf("Camara 3: tercera persona Gabu.\n");
				if (cameraIndex == 3) printf("Camara 4: tercera persona Principito.\n");
				if (cameraIndex == 4) printf("Camara 5: aerea sobre XZ.\n");

				teclaTabLiberada = false;
			}
		}
		else
		{
			teclaTabLiberada = true;
		}

		if (cameraIndex == 0)
		{
			camera.keyControl(keysCamara, deltaTime);
			camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());
		}
		else if (cameraIndex == 1)
		{

			glm::vec3 forwardBlitz = glm::normalize(glm::vec3(sin(rotacionBlitzY * toRadians), 0.0f, cos(rotacionBlitzY * toRadians)));
			if (keysCamara[GLFW_KEY_W])
			{
				posicionBlitz -= forwardBlitz * velocidadModeloTPS * deltaTime;
				blitzMovimientoTPS = true;
			}
			if (keysCamara[GLFW_KEY_S])
			{
				posicionBlitz += forwardBlitz * velocidadModeloTPS * deltaTime;
				blitzMovimientoTPS = true;
			}
			if (keysCamara[GLFW_KEY_A])
			{
				rotacionBlitzY += velocidadRotacionModeloTPS * deltaTime;
				blitzMovimientoTPS = true;
			}
			if (keysCamara[GLFW_KEY_D])
			{
				rotacionBlitzY -= velocidadRotacionModeloTPS * deltaTime;
				blitzMovimientoTPS = true;
			}
		}
		else if (cameraIndex == 2)
		{


			glm::vec3 forwardGabu = glm::normalize(glm::vec3(sin(rotacionGabuY * toRadians), 0.0f, cos(rotacionGabuY * toRadians)));
			if (keysCamara[GLFW_KEY_W])
			{
				posicionGabu -= forwardGabu * velocidadModeloTPS * deltaTime;
				gabuMovimientoTPS = true;
			}
			if (keysCamara[GLFW_KEY_S])
			{
				posicionGabu += forwardGabu * velocidadModeloTPS * deltaTime;
				gabuMovimientoTPS = true;
			}
			if (keysCamara[GLFW_KEY_A])
			{
				rotacionGabuY += velocidadRotacionModeloTPS * deltaTime;
				gabuMovimientoTPS = true;
			}
			if (keysCamara[GLFW_KEY_D])
			{
				rotacionGabuY -= velocidadRotacionModeloTPS * deltaTime;
				gabuMovimientoTPS = true;
			}
		}
		else if (cameraIndex == 3)
		{


			glm::vec3 forwardPrincipito = glm::normalize(glm::vec3(sin(rotacionPrincipitoY * toRadians), 0.0f, cos(rotacionPrincipitoY * toRadians)));
			if (keysCamara[GLFW_KEY_W])
			{
				posicionPrincipito -= forwardPrincipito * velocidadModeloTPS * deltaTime;
				principitoMovimientoTPS = true;
			}
			if (keysCamara[GLFW_KEY_S])
			{
				posicionPrincipito += forwardPrincipito * velocidadModeloTPS * deltaTime;
				principitoMovimientoTPS = true;
			}
			if (keysCamara[GLFW_KEY_A])
			{
				rotacionPrincipitoY += velocidadRotacionModeloTPS * deltaTime;
				principitoMovimientoTPS = true;
			}
			if (keysCamara[GLFW_KEY_D])
			{
				rotacionPrincipitoY -= velocidadRotacionModeloTPS * deltaTime;
				principitoMovimientoTPS = true;
			}
			if (keysCamara[GLFW_KEY_Q]) posicionPrincipito.y += velocidadVueloPrincipito * deltaTime;
			if (keysCamara[GLFW_KEY_E]) posicionPrincipito.y -= velocidadVueloPrincipito * deltaTime;
		}
		else if (cameraIndex == 4)
		{

			if (keysCamara[GLFW_KEY_A]) rotacionCamaraAereaY -= velocidadRotacionCamaraAerea * deltaTime;
			if (keysCamara[GLFW_KEY_D]) rotacionCamaraAereaY += velocidadRotacionCamaraAerea * deltaTime;
			glm::vec3 forwardAerea = glm::normalize(glm::vec3(cos(rotacionCamaraAereaY * toRadians), 0.0f, sin(rotacionCamaraAereaY * toRadians)));
			glm::vec3 rightAerea = glm::normalize(glm::cross(forwardAerea, glm::vec3(0.0f, 1.0f, 0.0f)));
			if (keysCamara[GLFW_KEY_W]) posicionCamaraAerea += forwardAerea * velocidadCamaraAerea * deltaTime;
			if (keysCamara[GLFW_KEY_S]) posicionCamaraAerea -= forwardAerea * velocidadCamaraAerea * deltaTime;
			if (keysCamara[GLFW_KEY_LEFT]) posicionCamaraAerea -= rightAerea * velocidadCamaraAerea * deltaTime;
			if (keysCamara[GLFW_KEY_RIGHT]) posicionCamaraAerea += rightAerea * velocidadCamaraAerea * deltaTime;
			if (keysCamara[GLFW_KEY_Q]) posicionCamaraAerea.y += velocidadAlturaCamaraAerea * deltaTime;
			if (keysCamara[GLFW_KEY_E]) posicionCamaraAerea.y -= velocidadAlturaCamaraAerea * deltaTime;
		}

		if (blitzMovimientoTPS)
			tiempoMovimientoBlitz += frameTime;
		else
			tiempoMovimientoBlitz = 0.0f;

		if (gabuMovimientoTPS)
			tiempoMovimientoGabu += frameTime;
		else
			tiempoMovimientoGabu = 0.0f;

		if (principitoMovimientoTPS)
			tiempoMovimientoPrincipito += frameTime;
		else
			tiempoMovimientoPrincipito = 0.0f;



		bool* keysLuces = mainWindow.getsKeys();
		if (keysLuces[GLFW_KEY_I])
		{
			if (teclaILiberada)
			{
				lucesInteractivasActivas = !lucesInteractivasActivas;
				luzIncogniumActiva = lucesInteractivasActivas;
				luzEstacionActiva = lucesInteractivasActivas;
				printf("Luces I (Incognium / Globo / Barco): %s\n", lucesInteractivasActivas ? "ENCENDIDAS" : "APAGADAS");
				teclaILiberada = false;
			}
		}
		else
		{
			teclaILiberada = true;
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


		bool* keysJayceAnimacion = mainWindow.getsKeys();
		if (keysJayceAnimacion[GLFW_KEY_3])
		{
			if (tecla3JayceLiberada && !recorridoJayceActivo)
			{
				posicionJayce = jayceInicio;
				rotacionJayceY = 153.20f;
				escalaJayce = 0.01f;
				tiempoRecorridoJayce = 0.0f;
				faseRecorridoJayce = 1;
				recorridoJayceActivo = true;
				animacionJayceActiva = true;
				Jayce_M.ResetAnimation();
				printf("Jayce inicia recorrido completo con animacion una sola vez.\n");
				tecla3JayceLiberada = false;
			}
		}
		else
		{
			tecla3JayceLiberada = true;
		}

		if (recorridoJayceActivo && faseRecorridoJayce == 1)
		{
			tiempoRecorridoJayce += frameTime;

			float progresoJayce = Clamp01(tiempoRecorridoJayce / duracionJayceRecorrido);



			float progresoVueloJayce = Clamp01(progresoJayce / progresoFisicoAterrizajeJayce);
			float avanceHorizontalJayce = pow(progresoVueloJayce, 0.72f);

			glm::vec3 posicionHorizontalJayce = glm::mix(jayceInicio, jayceFinal, avanceHorizontalJayce);

			float alturaYJayce = jayceInicio.y;
			float cimaSaltoJayce = jayceInicio.y + alturaExtraSaltoJayce;

			if (progresoVueloJayce < proporcionSubidaJayce)
			{

				float tSubidaJayce = SuavizarAnimacion(progresoVueloJayce / proporcionSubidaJayce);
				alturaYJayce = jayceInicio.y + ((cimaSaltoJayce - jayceInicio.y) * tSubidaJayce);
			}
			else
			{

				float tCaidaJayce = (progresoVueloJayce - proporcionSubidaJayce) / (1.0f - proporcionSubidaJayce);
				tCaidaJayce = Clamp01(tCaidaJayce);
				float gravedadPesadaJayce = pow(tCaidaJayce, 1.85f);
				alturaYJayce = cimaSaltoJayce + ((jayceFinal.y - cimaSaltoJayce) * gravedadPesadaJayce);
			}

			posicionJayce = glm::vec3(posicionHorizontalJayce.x, alturaYJayce, posicionHorizontalJayce.z);

			if (progresoJayce >= progresoFisicoAterrizajeJayce)
			{

				posicionJayce = jayceFinal;
			}



			float progresoAnimJayce = 0.0f;
			if (progresoJayce < progresoFisicoAterrizajeJayce)
			{
				float tAnimCaidaJayce = Clamp01(progresoJayce / progresoFisicoAterrizajeJayce);
				float caidaAnimAceleradaJayce = pow(tAnimCaidaJayce, 0.65f);
				progresoAnimJayce = progresoAnimAterrizajeJayce * caidaAnimAceleradaJayce;
			}
			else
			{
				float tLevantarseJayce = Clamp01((progresoJayce - progresoFisicoAterrizajeJayce) / (1.0f - progresoFisicoAterrizajeJayce));
				tLevantarseJayce = SuavizarAnimacion(tLevantarseJayce);
				progresoAnimJayce = progresoAnimAterrizajeJayce + ((1.0f - progresoAnimAterrizajeJayce) * tLevantarseJayce);
			}

			animacionJayceActiva = true;
			Jayce_M.SetAnimationProgress(progresoAnimJayce);

			if (progresoJayce >= 1.0f)
			{
				posicionJayce = jayceFinal;
				Jayce_M.SetAnimationProgress(1.0f);
				recorridoJayceActivo = false;
				animacionJayceActiva = false;
				faseRecorridoJayce = 2;
				printf("Jayce aterrizo, se levanto y queda estatico: glm::vec3(%.2ff, %.2ff, %.2ff).\n",
					posicionJayce.x, posicionJayce.y, posicionJayce.z);
			}
		}

		if (Vi_M.IsLoaded())
		{
			Vi_M.UpdateAnimation(frameTime);
		}
		if (Zorro_M.IsLoaded())
		{
			Zorro_M.UpdateAnimation(frameTime);
		}
		if (Reptil_M.IsLoaded())
		{
			if (abrirPuerta || rotPuertaDer > 0.5f || rotPuertaIzq > 0.5f)
				Reptil_M.UpdateAnimation(frameTime);
			else
				Reptil_M.SetAnimationProgress(0.0f);
		}
		if (animacionRexActiva)
		{
			float duracionTotalRex = duracionIdaRex + duracionGiroFinalRex + duracionRegresoRex + duracionGiroInicioRex;
			tiempoAnimacionRex += frameTime;
			while (tiempoAnimacionRex >= duracionTotalRex)
				tiempoAnimacionRex -= duracionTotalRex;

			float tRex = tiempoAnimacionRex;

			if (tRex < duracionIdaRex)
			{
				float p = SuavizarAnimacion(tRex / duracionIdaRex);
				posicionRex = glm::mix(rexInicio, rexFinal, p);
				rotacionRexY = rexRotIda;
			}
			else if (tRex < duracionIdaRex + duracionGiroFinalRex)
			{
				float p = SuavizarAnimacion((tRex - duracionIdaRex) / duracionGiroFinalRex);
				posicionRex = rexFinal;
				rotacionRexY = rexRotIda + ((rexRotRegreso - rexRotIda) * p);
			}
			else if (tRex < duracionIdaRex + duracionGiroFinalRex + duracionRegresoRex)
			{
				float p = SuavizarAnimacion((tRex - duracionIdaRex - duracionGiroFinalRex) / duracionRegresoRex);
				glm::vec3 puntoA = glm::mix(rexFinal, rexControlRegreso, p);
				glm::vec3 puntoB = glm::mix(rexControlRegreso, rexInicio, p);
				posicionRex = glm::mix(puntoA, puntoB, p);
				rotacionRexY = rexRotRegreso;
			}
			else
			{
				float p = SuavizarAnimacion((tRex - duracionIdaRex - duracionGiroFinalRex - duracionRegresoRex) / duracionGiroInicioRex);
				posicionRex = rexInicio;
				rotacionRexY = rexRotRegreso + ((rexRotIda - rexRotRegreso) * p);
			}
		}

		if (Rex_M.IsLoaded())
		{
			Rex_M.UpdateAnimation(frameTime);
		}
		if (Tirnanog_M.IsLoaded())
		{
			Tirnanog_M.UpdateAnimation(frameTime);
		}
		if (IndustrialBuilding_M.IsLoaded())
		{
			IndustrialBuilding_M.UpdateAnimation(frameTime);
		}
		if (Coliseo_M.IsLoaded())
		{
			Coliseo_M.UpdateAnimation(frameTime);
		}
		if (Mils_M.IsLoaded())
		{
			Mils_M.UpdateAnimation(frameTime);
		}
		if (SolBadguy_M.IsLoaded())
		{
			SolBadguy_M.UpdateAnimation(frameTime);
		}
		if (KyKiske_M.IsLoaded())
		{
			KyKiske_M.UpdateAnimation(frameTime);
		}
		if (Loris_M.IsLoaded())
		{
			Loris_M.UpdateAnimation(frameTime);
		}

		if (animacionLorisActiva)
		{
			float duracionTotalLoris = duracionLorisTramo1 + duracionLorisGiro1 + duracionLorisTramo2 + duracionLorisGiro2 +
				duracionLorisTramo3 + duracionLorisGiro3 + duracionLorisTramo4 + duracionLorisGiro4;

			tiempoAnimacionLoris += frameTime;
			while (tiempoAnimacionLoris >= duracionTotalLoris)
				tiempoAnimacionLoris -= duracionTotalLoris;

			float tLoris = tiempoAnimacionLoris;
			float rotLorisP1 = 0.0f;
			float rotLorisP2 = -62.06f;
			float rotLorisP3 = -167.78f;
			float rotLorisP4 = -270.0f;
			float rotLorisFinal = -360.0f;

			if (tLoris < duracionLorisTramo1)
			{
				float p = SuavizarAnimacion(tLoris / duracionLorisTramo1);
				posicionLoris = glm::mix(lorisP1, lorisP2, p);
				rotacionLorisY = rotLorisP1;
			}
			else if (tLoris < duracionLorisTramo1 + duracionLorisGiro1)
			{
				float p = SuavizarAnimacion((tLoris - duracionLorisTramo1) / duracionLorisGiro1);
				posicionLoris = lorisP2;
				rotacionLorisY = rotLorisP1 + ((rotLorisP2 - rotLorisP1) * p);
			}
			else if (tLoris < duracionLorisTramo1 + duracionLorisGiro1 + duracionLorisTramo2)
			{
				float p = SuavizarAnimacion((tLoris - duracionLorisTramo1 - duracionLorisGiro1) / duracionLorisTramo2);
				posicionLoris = glm::mix(lorisP2, lorisP3, p);
				rotacionLorisY = rotLorisP2;
			}
			else if (tLoris < duracionLorisTramo1 + duracionLorisGiro1 + duracionLorisTramo2 + duracionLorisGiro2)
			{
				float p = SuavizarAnimacion((tLoris - duracionLorisTramo1 - duracionLorisGiro1 - duracionLorisTramo2) / duracionLorisGiro2);
				posicionLoris = lorisP3;
				rotacionLorisY = rotLorisP2 + ((rotLorisP3 - rotLorisP2) * p);
			}
			else if (tLoris < duracionLorisTramo1 + duracionLorisGiro1 + duracionLorisTramo2 + duracionLorisGiro2 + duracionLorisTramo3)
			{
				float p = SuavizarAnimacion((tLoris - duracionLorisTramo1 - duracionLorisGiro1 - duracionLorisTramo2 - duracionLorisGiro2) / duracionLorisTramo3);
				posicionLoris = glm::mix(lorisP3, lorisP4, p);
				rotacionLorisY = rotLorisP3;
			}
			else if (tLoris < duracionLorisTramo1 + duracionLorisGiro1 + duracionLorisTramo2 + duracionLorisGiro2 + duracionLorisTramo3 + duracionLorisGiro3)
			{
				float p = SuavizarAnimacion((tLoris - duracionLorisTramo1 - duracionLorisGiro1 - duracionLorisTramo2 - duracionLorisGiro2 - duracionLorisTramo3) / duracionLorisGiro3);
				posicionLoris = lorisP4;
				rotacionLorisY = rotLorisP3 + ((rotLorisP4 - rotLorisP3) * p);
			}
			else if (tLoris < duracionLorisTramo1 + duracionLorisGiro1 + duracionLorisTramo2 + duracionLorisGiro2 + duracionLorisTramo3 + duracionLorisGiro3 + duracionLorisTramo4)
			{
				float p = SuavizarAnimacion((tLoris - duracionLorisTramo1 - duracionLorisGiro1 - duracionLorisTramo2 - duracionLorisGiro2 - duracionLorisTramo3 - duracionLorisGiro3) / duracionLorisTramo4);
				posicionLoris = glm::mix(lorisP4, lorisP1, p);
				rotacionLorisY = rotLorisP4;
			}
			else
			{
				float p = SuavizarAnimacion((tLoris - duracionLorisTramo1 - duracionLorisGiro1 - duracionLorisTramo2 - duracionLorisGiro2 - duracionLorisTramo3 - duracionLorisGiro3 - duracionLorisTramo4) / duracionLorisGiro4);
				posicionLoris = lorisP1;
				rotacionLorisY = rotLorisP4 + ((rotLorisFinal - rotLorisP4) * p);
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


		glm::mat4 jerarquiaArcane = glm::mat4(1.0f);
		jerarquiaArcane = glm::translate(jerarquiaArcane, posicionArcane);
		jerarquiaArcane = glm::rotate(jerarquiaArcane, rotacionArcaneY * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		jerarquiaArcane = glm::scale(jerarquiaArcane, glm::vec3(escalaArcane, escalaArcane, escalaArcane));


		glm::mat4 jerarquiaPueblo = jerarquiaArcane;
		jerarquiaPueblo = glm::translate(jerarquiaPueblo, posicionGrupoPueblo);
		jerarquiaPueblo = glm::rotate(jerarquiaPueblo, rotacionGrupoPuebloY * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		jerarquiaPueblo = glm::scale(jerarquiaPueblo, glm::vec3(escalaGrupoPueblo, escalaGrupoPueblo, escalaGrupoPueblo));



		glm::mat4 jerarquiaAireEstacion = glm::mat4(1.0f);
		jerarquiaAireEstacion = glm::translate(jerarquiaAireEstacion, posicionAireEstacion);
		jerarquiaAireEstacion = glm::rotate(jerarquiaAireEstacion, rotacionAireEstacionY * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		jerarquiaAireEstacion = glm::scale(jerarquiaAireEstacion, glm::vec3(escalaAireEstacion, escalaAireEstacion, escalaAireEstacion));


		glm::mat4 jerarquiaEstacion = jerarquiaAireEstacion;



		glm::mat4 jerarquiaAuraEstacion = jerarquiaEstacion;
		jerarquiaAuraEstacion = glm::translate(jerarquiaAuraEstacion, posicionLocalAuraEstacion);
		jerarquiaAuraEstacion = glm::rotate(jerarquiaAuraEstacion, rotacionAuraEstacionY * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		jerarquiaAuraEstacion = glm::scale(jerarquiaAuraEstacion, glm::vec3(escalaAuraEstacion, escalaAuraEstacion, escalaAuraEstacion));



		glm::vec3 posicionLuzEstacion = glm::vec3(jerarquiaAuraEstacion * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
		float intensidadEstacion = luzEstacionActiva ? 1.0f : 0.0f;
		pointLights[0] = PointLight(0.0f, 0.42f, 1.0f,
			0.32f * intensidadEstacion, 10.85f * intensidadEstacion,
			posicionLuzEstacion.x, posicionLuzEstacion.y, posicionLuzEstacion.z,
			1.0f, 0.006f, 0.0008f);


		glm::mat4 jerarquiaElprincipito = glm::mat4(1.0f);


		glm::mat4 jerarquiaPlanetas = jerarquiaElprincipito;
		jerarquiaPlanetas = glm::translate(jerarquiaPlanetas, posicionPlanetas);
		jerarquiaPlanetas = glm::rotate(jerarquiaPlanetas, rotacionPlanetasY * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		jerarquiaPlanetas = glm::scale(jerarquiaPlanetas, glm::vec3(escalaPlanetas, escalaPlanetas, escalaPlanetas));



		float fasePasoPrincipito = tiempoMovimientoPrincipito * 8.0f;
		float senoPasoPrincipito = principitoMovimientoTPS ? sin(fasePasoPrincipito) : 0.0f;
		float senoCabezaPrincipito = principitoMovimientoTPS ? sin(fasePasoPrincipito * 0.72f) : 0.0f;
		float rebotePrincipito = principitoMovimientoTPS ? (0.13f * fabs(sin(fasePasoPrincipito))) : 0.0f;


		float limiteBrazoPrincipito = 7.0f;
		float anguloBrazoDerPrincipito = limiteBrazoPrincipito * senoPasoPrincipito;
		float anguloBrazoIzqPrincipito = -limiteBrazoPrincipito * senoPasoPrincipito;
		if (anguloBrazoDerPrincipito > limiteBrazoPrincipito) anguloBrazoDerPrincipito = limiteBrazoPrincipito;
		if (anguloBrazoDerPrincipito < -limiteBrazoPrincipito) anguloBrazoDerPrincipito = -limiteBrazoPrincipito;
		if (anguloBrazoIzqPrincipito > limiteBrazoPrincipito) anguloBrazoIzqPrincipito = limiteBrazoPrincipito;
		if (anguloBrazoIzqPrincipito < -limiteBrazoPrincipito) anguloBrazoIzqPrincipito = -limiteBrazoPrincipito;


		float limitePiernaPrincipito = 8.0f;
		float anguloPiernaDerPrincipito = -limitePiernaPrincipito * senoPasoPrincipito;
		float anguloPiernaIzqPrincipito = limitePiernaPrincipito * senoPasoPrincipito;
		if (anguloPiernaDerPrincipito > limitePiernaPrincipito) anguloPiernaDerPrincipito = limitePiernaPrincipito;
		if (anguloPiernaDerPrincipito < -limitePiernaPrincipito) anguloPiernaDerPrincipito = -limitePiernaPrincipito;
		if (anguloPiernaIzqPrincipito > limitePiernaPrincipito) anguloPiernaIzqPrincipito = limitePiernaPrincipito;
		if (anguloPiernaIzqPrincipito < -limitePiernaPrincipito) anguloPiernaIzqPrincipito = -limitePiernaPrincipito;

		float anguloCabezaPrincipito = 22.0f * senoCabezaPrincipito;


		glm::mat4 jerarquiaPrincipito = jerarquiaElprincipito;
		jerarquiaPrincipito = glm::translate(jerarquiaPrincipito, posicionPrincipito + glm::vec3(0.0f, rebotePrincipito, 0.0f));
		jerarquiaPrincipito = glm::rotate(jerarquiaPrincipito, rotacionPrincipitoY * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		jerarquiaPrincipito = glm::scale(jerarquiaPrincipito, glm::vec3(escalaPrincipito, escalaPrincipito, escalaPrincipito));



		float vibracionMorado001Y = 0.0f;
		float vibracionAmarilloY = 0.0f;
		float vibracionAzulY = 0.0f;
		float vibracionGrisY = 0.0f;
		float vibracionMoradoY = 0.0f;
		float vibracionRosaY = 0.0f;
		float vibracionVerdeY = 0.0f;

		float pulsoMorado001 = 1.0f;
		float pulsoAmarillo = 1.0f;
		float pulsoAzul = 1.0f;
		float pulsoGris = 1.0f;
		float pulsoMorado = 1.0f;
		float pulsoRosa = 1.0f;
		float pulsoVerde = 1.0f;



		float escalaAnimadaPlanetaAzul = escalaPlanetaAzul * pulsoAzul;
		glm::mat4 jerarquiaPlanetaAzulAnimada = jerarquiaPlanetas;
		jerarquiaPlanetaAzulAnimada = glm::translate(jerarquiaPlanetaAzulAnimada, posicionPlanetaAzul + glm::vec3(0.0f, vibracionAzulY, 0.0f));
		jerarquiaPlanetaAzulAnimada = glm::scale(jerarquiaPlanetaAzulAnimada, glm::vec3(escalaAnimadaPlanetaAzul, escalaAnimadaPlanetaAzul, escalaAnimadaPlanetaAzul));



		float fasePasoBlitz = tiempoMovimientoBlitz * velocidadPasoBlitz;
		float senoPasoBlitz = blitzMovimientoTPS ? sin(fasePasoBlitz) : 0.0f;
		float cosPasoBlitz = blitzMovimientoTPS ? cos(fasePasoBlitz) : 0.0f;
		float bobBlitz = 0.0f;
		float anguloBrazoDerBlitz = amplitudPasoBrazoBlitz * senoPasoBlitz;
		float anguloBrazoIzqBlitz = -amplitudPasoBrazoBlitz * senoPasoBlitz;
		float anguloPiernaDerBlitz = -amplitudPasoPiernaBlitz * senoPasoBlitz;
		float anguloPiernaIzqBlitz = amplitudPasoPiernaBlitz * senoPasoBlitz;
		float anguloPieDerBlitz = blitzMovimientoTPS ? (3.0f * cosPasoBlitz) : 0.0f;
		float anguloPieIzqBlitz = blitzMovimientoTPS ? (-3.0f * cosPasoBlitz) : 0.0f;

		glm::mat4 jerarquiaBlitz = jerarquiaArcane;
		jerarquiaBlitz = glm::translate(jerarquiaBlitz, posicionBlitz + glm::vec3(0.0f, bobBlitz, 0.0f));
		jerarquiaBlitz = glm::rotate(jerarquiaBlitz, rotacionBlitzY * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		jerarquiaBlitz = glm::scale(jerarquiaBlitz, glm::vec3(escalaBlitz, escalaBlitz, escalaBlitz));



		glm::mat4 jerarquiaIncognium = jerarquiaPueblo;
		jerarquiaIncognium = glm::translate(jerarquiaIncognium, posicionGrupoIncognium);
		jerarquiaIncognium = glm::scale(jerarquiaIncognium, glm::vec3(escalaGrupoIncognium, escalaGrupoIncognium, escalaGrupoIncognium));

		glm::vec3 posicionLuzIncognium = glm::vec3(jerarquiaIncognium * glm::vec4(pivoteMalla, 1.0f));

		float intensidadLuminariasNoche = (intensidadDia < 0.35f) ? 1.0f : 0.0f;
		glm::vec3 posicionLuzPlazaIncognium = glm::vec3(jerarquiaPueblo * glm::vec4(0.0f, 14.0f, 0.0f, 1.0f));
		glm::vec3 posicionLuzZonaJurassic = glm::vec3(135.0f, 18.0f, -155.0f);
		glm::vec3 posicionLuzConsejo = glm::vec3(250.0f, 18.0f, 220.0f);

		pointLights[0] = PointLight(1.0f, 0.82f, 0.55f,
			0.24f * intensidadLuminariasNoche, 3.20f * intensidadLuminariasNoche,
			posicionLuzPlazaIncognium.x, posicionLuzPlazaIncognium.y, posicionLuzPlazaIncognium.z,
			1.0f, 0.0022f, 0.000045f);

		pointLights[1] = PointLight(1.0f, 0.76f, 0.48f,
			0.24f * intensidadLuminariasNoche, 3.60f * intensidadLuminariasNoche,
			posicionLuzZonaJurassic.x, posicionLuzZonaJurassic.y, posicionLuzZonaJurassic.z,
			1.0f, 0.0020f, 0.000040f);

		pointLights[2] = PointLight(0.75f, 0.90f, 1.0f,
			0.22f * intensidadLuminariasNoche, 3.40f * intensidadLuminariasNoche,
			posicionLuzConsejo.x, posicionLuzConsejo.y, posicionLuzConsejo.z,
			1.0f, 0.0021f, 0.000042f);
		pointLightCount = 3;



		glm::mat4 jerarquiaFaros[4];
		for (int i = 0; i < 4; i++)
		{
			jerarquiaFaros[i] = jerarquiaPueblo;
			jerarquiaFaros[i] = glm::translate(jerarquiaFaros[i], posicionFaros[i]);
			jerarquiaFaros[i] = glm::scale(jerarquiaFaros[i], glm::vec3(escalaFaro, escalaFaro, escalaFaro));
		}


		glm::mat4 gabu = glm::mat4(1.0f);
		gabu = glm::translate(gabu, posicionGabu);
		gabu = glm::rotate(gabu, rotacionGabuY * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		gabu = glm::scale(gabu, glm::vec3(escalaGabu, escalaGabu, escalaGabu));

		float fasePasoGabu = tiempoMovimientoGabu * 8.5f;
		float pasoDiagonalA_Gabu = gabuMovimientoTPS ? sin(fasePasoGabu) : 0.0f;      // delantera izquierda + trasera derecha
		float pasoDiagonalB_Gabu = -pasoDiagonalA_Gabu;                             // delantera derecha + trasera izquierda

		float reboteGabu = 0.0f;
		float giroCabezaGabu = gabuMovimientoTPS ? (3.0f * sin(fasePasoGabu * 0.6f)) : 0.0f;
		float giroTorsoGabu = gabuMovimientoTPS ? (1.5f * sin(fasePasoGabu * 0.5f)) : 0.0f;

		float giroPiernaDelanteraIzqGabu = 7.0f * pasoDiagonalA_Gabu;
		float giroPiernaTraseraDerGabu = 7.0f * pasoDiagonalA_Gabu;
		float giroPiernaDelanteraDerGabu = 7.0f * pasoDiagonalB_Gabu;
		float giroPiernaTraseraIzqGabu = 7.0f * pasoDiagonalB_Gabu;

		float avancePiernaDelanteraIzqGabu = 0.16f * pasoDiagonalA_Gabu;
		float avancePiernaTraseraDerGabu = 0.16f * pasoDiagonalA_Gabu;
		float avancePiernaDelanteraDerGabu = 0.16f * pasoDiagonalB_Gabu;
		float avancePiernaTraseraIzqGabu = 0.16f * pasoDiagonalB_Gabu;

		float levantaA = pasoDiagonalA_Gabu > 0.0f ? 0.055f * pasoDiagonalA_Gabu : 0.0f;
		float levantaB = pasoDiagonalB_Gabu > 0.0f ? 0.055f * pasoDiagonalB_Gabu : 0.0f;
		float levantaPiernaDelanteraIzqGabu = levantaA;
		float levantaPiernaTraseraDerGabu = levantaA;
		float levantaPiernaDelanteraDerGabu = levantaB;
		float levantaPiernaTraseraIzqGabu = levantaB;

		gabu = glm::translate(gabu, glm::vec3(0.0f, reboteGabu, 0.0f));




		if (cameraIndex == 1)
		{

			glm::vec3 posicionBlitzMundo = glm::vec3(jerarquiaBlitz * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));

			camera.setThirdPersonCamera(posicionBlitzMundo, rotacionBlitzY + rotacionArcaneY + 180.0f, frameTime, 22.0f, 8.5f, 6.0f);
		}
		else if (cameraIndex == 2)
		{

			glm::vec3 posicionGabuMundo = glm::vec3(gabu * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));

			camera.setThirdPersonCamera(posicionGabuMundo, rotacionGabuY + 180.0f, frameTime, 6.5f, 3.4f, 3.0f);
		}
		else if (cameraIndex == 3)
		{

			glm::vec3 posicionPrincipitoMundo = glm::vec3(jerarquiaPrincipito * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));

			camera.setThirdPersonCamera(posicionPrincipitoMundo, rotacionPrincipitoY + 180.0f, frameTime, 10.0f, 5.2f, 5.0f);
		}
		else if (cameraIndex == 4)
		{
			camera.setAerialCamera(posicionCamaraAerea, rotacionCamaraAereaY, rotacionCamaraAereaPitch);
		}

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
		glUniform1f(glGetUniformLocation(shaderList[0].GetShaderID(), "alphaFactor"), 1.0f);
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



		float intensidadLucesI = lucesInteractivasActivas ? 1.0f : 0.0f;

		glm::vec3 origenSpotIncognium = posicionLuzIncognium + glm::vec3(0.0f, 12.0f, 0.0f);
		glm::vec3 direccionSpotIncognium = glm::normalize(glm::vec3(0.0f, -1.0f, 0.0f));
		spotLights[0] = SpotLight(0.0f, 0.55f, 1.0f,
			0.45f * intensidadLucesI, 4.80f * intensidadLucesI,
			origenSpotIncognium.x, origenSpotIncognium.y, origenSpotIncognium.z,
			direccionSpotIncognium.x, direccionSpotIncognium.y, direccionSpotIncognium.z,
			1.0f, 0.0025f, 0.000055f,
			58.0f);

		glm::vec3 origenSpotGlobo = glm::vec3(jerarquiaEstacion * glm::vec4(posicionGlobo + glm::vec3(0.0f, 6.5f, 0.0f), 1.0f));
		glm::vec3 direccionSpotGlobo = glm::normalize(glm::vec3(0.0f, -1.0f, 0.0f));
		spotLights[1] = SpotLight(0.15f, 0.75f, 1.0f,
			0.42f * intensidadLucesI, 4.20f * intensidadLucesI,
			origenSpotGlobo.x, origenSpotGlobo.y, origenSpotGlobo.z,
			direccionSpotGlobo.x, direccionSpotGlobo.y, direccionSpotGlobo.z,
			1.0f, 0.0024f, 0.000050f,
			56.0f);

		glm::vec3 origenSpotBarco = glm::vec3(movBarco_x, 18.0f, movBarco_z);
		glm::vec3 direccionSpotBarco = glm::normalize(glm::vec3(0.0f, -1.0f, 0.0f));
		spotLights[2] = SpotLight(1.0f, 0.78f, 0.45f,
			0.46f * intensidadLucesI, 5.20f * intensidadLucesI,
			origenSpotBarco.x, origenSpotBarco.y, origenSpotBarco.z,
			direccionSpotBarco.x, direccionSpotBarco.y, direccionSpotBarco.z,
			1.0f, 0.0022f, 0.000045f,
			60.0f);
		spotLightCount = 3;

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



		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		toffset = glm::vec2(0.0f, 0.0f);
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);

		model = glm::mat4(1.0f);
		model = glm::translate(model, posicionBanca);
		model = glm::rotate(model, rotacionBancaY * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(escalaBanca, escalaBanca, escalaBanca));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Banca_M.RenderModel();

		for (int i = 0; i < NUM_BANCAS_EXTRA; i++)
		{
			model = glm::mat4(1.0f);
			model = glm::translate(model, posicionesBancasExtra[i]);
			model = glm::rotate(model, rotacionesBancasExtra[i] * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
			model = glm::scale(model, glm::vec3(escalaBanca, escalaBanca, escalaBanca));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			Banca_M.RenderModel();
		}

		for (int i = 0; i < NUM_FAROS_EXTRA; i++)
		{
			model = glm::mat4(1.0f);
			model = glm::translate(model, posicionesFarosExtra[i]);
			model = glm::scale(model, glm::vec3(escalaFaroExtra, escalaFaroExtra, escalaFaroExtra));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			Faro_M.RenderModel();
		}

		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		for (int i = 0; i < NUM_ARBOLES_EXTRA; i++)
		{
			model = glm::mat4(1.0f);
			model = glm::translate(model, posicionesArbolesExtra[i]);
			model = glm::scale(model, glm::vec3(escalaArbolExtra, escalaArbolExtra, escalaArbolExtra));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			Arbol_M.RenderModel();
		}

		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		toffset = glm::vec2(0.0f, 0.0f);
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);

		model = gabu;
		model = glm::rotate(model, giroTorsoGabu * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		TorsoGabu_M.RenderModel();

		model = gabu;
		model = glm::rotate(model, giroCabezaGabu * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		CabezaGabu_M.RenderModel();

		model = gabu;
		model = glm::translate(model, glm::vec3(0.0f, levantaPiernaDelanteraDerGabu, avancePiernaDelanteraDerGabu));
		model = glm::rotate(model, giroPiernaDelanteraDerGabu * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		PiernaDerGabu_M.RenderModel();

		model = gabu;
		model = glm::translate(model, glm::vec3(0.0f, levantaPiernaDelanteraIzqGabu, avancePiernaDelanteraIzqGabu));
		model = glm::rotate(model, giroPiernaDelanteraIzqGabu * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		PiernaIzqGabu_M.RenderModel();

		model = gabu;
		model = glm::translate(model, glm::vec3(0.0f, levantaPiernaTraseraDerGabu, avancePiernaTraseraDerGabu));
		model = glm::rotate(model, giroPiernaTraseraDerGabu * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		PiernaTrasDerGabu_M.RenderModel();

		model = gabu;
		model = glm::translate(model, glm::vec3(0.0f, levantaPiernaTraseraIzqGabu, avancePiernaTraseraIzqGabu));
		model = glm::rotate(model, giroPiernaTraseraIzqGabu * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		PiernaTrasIzqGabu_M.RenderModel();



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



		model = jerarquiaEstacion;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		toffset = glm::vec2(0.0f, 0.0f);
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Estacion_M.RenderModel();


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
		model = glm::rotate(model, anguloCabezaPrincipito * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(escalaCabezaPrincipito, escalaCabezaPrincipito, escalaCabezaPrincipito));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		PrincipitoCabeza_M.RenderModel();



		model = jerarquiaPrincipito;
		model = glm::translate(model, posicionBrazoDerPrincipito);
		model = glm::translate(model, pivoteHombroDerPrincipito);
		model = glm::rotate(model, anguloBrazoDerPrincipito * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::translate(model, -pivoteHombroDerPrincipito);
		model = glm::scale(model, glm::vec3(escalaBrazoDerPrincipito, escalaBrazoDerPrincipito, escalaBrazoDerPrincipito));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		PrincipitoBrazoDer_M.RenderModel();

		model = jerarquiaPrincipito;
		model = glm::translate(model, posicionBrazoIzqPrincipito);
		model = glm::translate(model, pivoteHombroIzqPrincipito);
		model = glm::rotate(model, anguloBrazoIzqPrincipito * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::translate(model, -pivoteHombroIzqPrincipito);
		model = glm::scale(model, glm::vec3(escalaBrazoIzqPrincipito, escalaBrazoIzqPrincipito, escalaBrazoIzqPrincipito));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		PrincipitoBrazoIzq_M.RenderModel();



		model = jerarquiaPrincipito;
		model = glm::translate(model, posicionPiernaDerPrincipito);
		model = glm::translate(model, pivoteCaderaDerPrincipito);
		model = glm::rotate(model, anguloPiernaDerPrincipito * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::translate(model, -pivoteCaderaDerPrincipito);
		model = glm::scale(model, glm::vec3(escalaPiernaDerPrincipito, escalaPiernaDerPrincipito, escalaPiernaDerPrincipito));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		PrincipitoPiernaDer_M.RenderModel();

		model = jerarquiaPrincipito;
		model = glm::translate(model, posicionPiernaIzqPrincipito);
		model = glm::translate(model, pivoteCaderaIzqPrincipito);
		model = glm::rotate(model, anguloPiernaIzqPrincipito * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::translate(model, -pivoteCaderaIzqPrincipito);
		model = glm::scale(model, glm::vec3(escalaPiernaIzqPrincipito, escalaPiernaIzqPrincipito, escalaPiernaIzqPrincipito));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		PrincipitoPiernaIzq_M.RenderModel();


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



		glm::mat4 jerarquiaMaquinarr = jerarquiaArcane;
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





		if (Jayce_M.IsLoaded() || Vi_M.IsLoaded() || Zorro_M.IsLoaded() || Reptil_M.IsLoaded() || Rex_M.IsLoaded() || Tirnanog_M.IsLoaded() || IndustrialBuilding_M.IsLoaded() || Coliseo_M.IsLoaded() || Mils_M.IsLoaded() || SolBadguy_M.IsLoaded() || KyKiske_M.IsLoaded() || Loris_M.IsLoaded())
		{
			shaderAnimado.UseShader();

			GLuint uniformModelAnim = shaderAnimado.GetModelLocation();
			GLuint uniformProjectionAnim = shaderAnimado.GetProjectionLocation();
			GLuint uniformViewAnim = shaderAnimado.GetViewLocation();
			GLuint uniformEyeAnim = shaderAnimado.GetEyePositionLocation();
			GLuint uniformColorAnim = shaderAnimado.getColorLocation();
			GLuint uniformOffsetAnim = shaderAnimado.getOffsetLocation();
			GLuint uniformSpecularAnim = shaderAnimado.GetSpecularIntensityLocation();
			GLuint uniformShininessAnim = shaderAnimado.GetShininessLocation();

			glUniformMatrix4fv(uniformProjectionAnim, 1, GL_FALSE, glm::value_ptr(projection));
			glUniformMatrix4fv(uniformViewAnim, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
			glUniform3f(uniformEyeAnim, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);
			glUniform3fv(uniformColorAnim, 1, glm::value_ptr(glm::vec3(1.0f, 1.0f, 1.0f)));
			glUniform2fv(uniformOffsetAnim, 1, glm::value_ptr(glm::vec2(0.0f, 0.0f)));
			glUniform1i(glGetUniformLocation(shaderAnimado.GetShaderID(), "theTexture"), 0);
			Material_brillante.UseMaterial(uniformSpecularAnim, uniformShininessAnim);
			shaderAnimado.SetDirectionalLight(&mainLight);
			shaderAnimado.SetPointLights(pointLights, pointLightCount);
			shaderAnimado.SetSpotLights(spotLights, spotLightCount);

			if (Jayce_M.IsLoaded())
			{
				Jayce_M.SetBoneUniforms(shaderAnimado.GetShaderID());
				model = glm::mat4(1.0f);
				model = glm::translate(model, posicionJayce);
				model = glm::rotate(model, rotacionJayceY * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
				model = glm::scale(model, glm::vec3(escalaJayce, escalaJayce, escalaJayce));
				glUniformMatrix4fv(uniformModelAnim, 1, GL_FALSE, glm::value_ptr(model));
				Jayce_M.RenderModel();
			}

			if (Vi_M.IsLoaded())
			{
				Vi_M.SetBoneUniforms(shaderAnimado.GetShaderID());
				model = glm::mat4(1.0f);
				model = glm::translate(model, posicionVi);
				model = glm::rotate(model, rotacionViY * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
				model = glm::scale(model, glm::vec3(escalaVi, escalaVi, escalaVi));
				glUniformMatrix4fv(uniformModelAnim, 1, GL_FALSE, glm::value_ptr(model));
				Vi_M.RenderModel();
			}

			if (Zorro_M.IsLoaded())
			{
				Zorro_M.SetBoneUniforms(shaderAnimado.GetShaderID());
				model = glm::mat4(1.0f);
				model = glm::translate(model, posicionZorro);
				model = glm::rotate(model, rotacionZorroY * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
				model = glm::rotate(model, rotacionZorroX * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
				model = glm::scale(model, glm::vec3(escalaZorro, escalaZorro, escalaZorro));
				glUniformMatrix4fv(uniformModelAnim, 1, GL_FALSE, glm::value_ptr(model));
				Zorro_M.RenderModel();
			}

			if (Reptil_M.IsLoaded())
			{
				Reptil_M.SetBoneUniforms(shaderAnimado.GetShaderID());
				model = glm::mat4(1.0f);
				model = glm::translate(model, posicionReptil);
				model = glm::rotate(model, rotacionReptilY * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
				model = glm::rotate(model, rotacionReptilX * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
				model = glm::scale(model, glm::vec3(escalaReptil, escalaReptil, escalaReptil));
				glUniformMatrix4fv(uniformModelAnim, 1, GL_FALSE, glm::value_ptr(model));
				Reptil_M.RenderModel();
			}

			if (Rex_M.IsLoaded())
			{
				Rex_M.SetBoneUniforms(shaderAnimado.GetShaderID());
				model = glm::mat4(1.0f);
				model = glm::translate(model, posicionRex);
				model = glm::rotate(model, rotacionRexY * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
				model = glm::rotate(model, rotacionRexX * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
				model = glm::scale(model, glm::vec3(escalaRex, escalaRex, escalaRex));
				glUniformMatrix4fv(uniformModelAnim, 1, GL_FALSE, glm::value_ptr(model));
				Rex_M.RenderModel();
			}

			if (Tirnanog_M.IsLoaded())
			{
				Tirnanog_M.SetBoneUniforms(shaderAnimado.GetShaderID());
				model = glm::mat4(1.0f);
				model = glm::translate(model, posicionTirnanog);
				model = glm::rotate(model, rotacionTirnanogY * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
				model = glm::rotate(model, rotacionTirnanogX * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
				model = glm::scale(model, glm::vec3(escalaTirnanog, escalaTirnanog, escalaTirnanog));
				glUniformMatrix4fv(uniformModelAnim, 1, GL_FALSE, glm::value_ptr(model));
				Tirnanog_M.RenderModel();
			}

			if (IndustrialBuilding_M.IsLoaded())
			{
				IndustrialBuilding_M.SetBoneUniforms(shaderAnimado.GetShaderID());
				model = glm::mat4(1.0f);
				model = glm::translate(model, posicionIndustrialBuilding);
				model = glm::rotate(model, rotacionIndustrialBuildingY * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
				model = glm::rotate(model, rotacionIndustrialBuildingX * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
				model = glm::scale(model, glm::vec3(escalaIndustrialBuilding, escalaIndustrialBuilding, escalaIndustrialBuilding));
				glUniformMatrix4fv(uniformModelAnim, 1, GL_FALSE, glm::value_ptr(model));
				IndustrialBuilding_M.RenderModel();
			}

			if (Coliseo_M.IsLoaded())
			{
				Coliseo_M.SetBoneUniforms(shaderAnimado.GetShaderID());
				model = glm::mat4(1.0f);
				model = glm::translate(model, posicionColiseo);
				model = glm::rotate(model, rotacionColiseoY * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
				model = glm::rotate(model, rotacionColiseoX * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
				model = glm::scale(model, glm::vec3(escalaColiseo, escalaColiseo, escalaColiseo));
				glUniformMatrix4fv(uniformModelAnim, 1, GL_FALSE, glm::value_ptr(model));
				Coliseo_M.RenderModel();
			}

			if (Mils_M.IsLoaded())
			{
				Mils_M.SetBoneUniforms(shaderAnimado.GetShaderID());
				model = glm::mat4(1.0f);
				model = glm::translate(model, posicionMils);
				model = glm::rotate(model, rotacionMilsY * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
				model = glm::rotate(model, rotacionMilsX * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
				model = glm::scale(model, glm::vec3(escalaMils, escalaMils, escalaMils));
				glUniformMatrix4fv(uniformModelAnim, 1, GL_FALSE, glm::value_ptr(model));
				Mils_M.RenderModel();
			}

			if (SolBadguy_M.IsLoaded())
			{
				SolBadguy_M.SetBoneUniforms(shaderAnimado.GetShaderID());
				model = glm::mat4(1.0f);
				model = glm::translate(model, posicionSolBadguy);
				model = glm::rotate(model, rotacionSolBadguyY * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
				model = glm::rotate(model, rotacionSolBadguyX * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
				model = glm::scale(model, glm::vec3(escalaSolBadguy, escalaSolBadguy, escalaSolBadguy));
				glUniformMatrix4fv(uniformModelAnim, 1, GL_FALSE, glm::value_ptr(model));
				SolBadguy_M.RenderModel();
			}

			if (KyKiske_M.IsLoaded())
			{
				KyKiske_M.SetBoneUniforms(shaderAnimado.GetShaderID());
				model = glm::mat4(1.0f);
				model = glm::translate(model, posicionKyKiske);
				model = glm::rotate(model, rotacionKyKiskeY * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
				model = glm::rotate(model, rotacionKyKiskeX * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
				model = glm::scale(model, glm::vec3(escalaKyKiske, escalaKyKiske, escalaKyKiske));
				glUniformMatrix4fv(uniformModelAnim, 1, GL_FALSE, glm::value_ptr(model));
				KyKiske_M.RenderModel();
			}

			if (Loris_M.IsLoaded())
			{
				Loris_M.SetBoneUniforms(shaderAnimado.GetShaderID());
				model = glm::mat4(1.0f);
				model = glm::translate(model, posicionLoris);
				model = glm::rotate(model, rotacionLorisY * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
				model = glm::rotate(model, rotacionLorisX * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
				model = glm::scale(model, glm::vec3(escalaLoris, escalaLoris, escalaLoris));
				glUniformMatrix4fv(uniformModelAnim, 1, GL_FALSE, glm::value_ptr(model));
				Loris_M.RenderModel();
			}
		}


		// ==========================================
		// JERARQUIA JURASIC PARK
		// ==========================================
		shaderList[0].UseShader();
		GLuint uniformModelJP = shaderList[0].GetModelLocation();
		GLuint uniformTextureOffsetJP = shaderList[0].getOffsetLocation();
		GLuint uniformAlphaJP = glGetUniformLocation(shaderList[0].GetShaderID(), "alphaFactor");
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		glm::vec2 toffsetLimpiarJP(0.0f, 0.0f);

		glm::mat4 modelJurasicPark = glm::mat4(1.0f);
		modelJurasicPark = glm::translate(modelJurasicPark, glm::vec3(0.0f, 0.0f, 0.0f));

		// === DIBUJAR AGUA GIGANTE ===
		glm::mat4 modelAgua = modelJurasicPark;
		modelAgua = glm::translate(modelAgua, glm::vec3(0.0f, -3.5f, 0.0f));
		modelAgua = glm::scale(modelAgua, glm::vec3(1500.0f, 1.0f, 1500.0f));
		glUniformMatrix4fv(uniformModelJP, 1, GL_FALSE, glm::value_ptr(modelAgua));
		aguaTexture.UseTexture();
		glUniform2f(uniformTextureOffsetJP, 0.0f, aguaOffset);
		meshList[2]->RenderMesh();
		glUniform2fv(uniformTextureOffsetJP, 1, glm::value_ptr(toffsetLimpiarJP));

		// BASE DE LA PUERTA
		glm::mat4 modelBasePuerta = modelJurasicPark;
		modelBasePuerta = glm::translate(modelBasePuerta, posicionPuertaJurassic);
		modelBasePuerta = glm::rotate(modelBasePuerta, rotacionPuertaJurassicY * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));

		glm::mat4 modelauxPuerta = modelBasePuerta;
		glm::mat4 modelBasePuertaDibujo = modelBasePuerta;
		modelBasePuertaDibujo = glm::scale(modelBasePuertaDibujo, glm::vec3(0.2f * escalaPuertaJurassic, 0.2f * escalaPuertaJurassic, 0.2f * escalaPuertaJurassic));
		glUniformMatrix4fv(uniformModelJP, 1, GL_FALSE, glm::value_ptr(modelBasePuertaDibujo));
		Puerta_base_M.RenderModel();

		if (abrirPuerta) {
			rotPuertaDer += velocidadPuerta * deltaTime;
			rotPuertaIzq += velocidadPuerta * deltaTime;
			if (rotPuertaDer > 75.0f) rotPuertaDer = 75.0f;
			if (rotPuertaIzq > 75.0f) rotPuertaIzq = 75.0f;
		}
		else {
			rotPuertaDer -= velocidadPuerta * deltaTime;
			rotPuertaIzq -= velocidadPuerta * deltaTime;
			if (rotPuertaDer < 0.0f) rotPuertaDer = 0.0f;
			if (rotPuertaIzq < 0.0f) rotPuertaIzq = 0.0f;
		}

		float separacionHojasPuerta = 24.5f;
		float alturaHojasPuerta = 13.0f;
		float profundidadHojasPuerta = 2.0f;

		glm::mat4 modelPD = modelauxPuerta;
		modelPD = glm::translate(modelPD, glm::vec3(separacionHojasPuerta, alturaHojasPuerta, profundidadHojasPuerta));
		modelPD = glm::rotate(modelPD, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		float offsetPivoteDer = -6.5f;
		modelPD = glm::translate(modelPD, glm::vec3(offsetPivoteDer, 0.0f, 0.0f));
		modelPD = glm::rotate(modelPD, glm::radians(rotPuertaDer), glm::vec3(0.0f, 0.0f, 1.0f));
		modelPD = glm::translate(modelPD, glm::vec3(-offsetPivoteDer, 0.0f, 0.0f));
		modelPD = glm::scale(modelPD, glm::vec3(5.3f * escalaPuertaJurassic, 6.2f * escalaPuertaJurassic, 4.2f * escalaPuertaJurassic));
		glUniformMatrix4fv(uniformModelJP, 1, GL_FALSE, glm::value_ptr(modelPD));
		Puerta_M.RenderModel();

		glm::mat4 modelPI = modelauxPuerta;
		modelPI = glm::translate(modelPI, glm::vec3(-separacionHojasPuerta, alturaHojasPuerta, profundidadHojasPuerta));
		modelPI = glm::rotate(modelPI, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		modelPI = glm::rotate(modelPI, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		float offsetPivoteIzq = -6.5f;
		modelPI = glm::translate(modelPI, glm::vec3(offsetPivoteIzq, 0.0f, 0.0f));
		modelPI = glm::rotate(modelPI, glm::radians(-rotPuertaIzq), glm::vec3(0.0f, 0.0f, 1.0f));
		modelPI = glm::translate(modelPI, glm::vec3(-offsetPivoteIzq, 0.0f, 0.0f));
		modelPI = glm::scale(modelPI, glm::vec3(5.3f * escalaPuertaJurassic, 6.2f * escalaPuertaJurassic, 4.2f * escalaPuertaJurassic));
		glUniformMatrix4fv(uniformModelJP, 1, GL_FALSE, glm::value_ptr(modelPI));
		Puerta1_M.RenderModel();
		glUniform2fv(uniformTextureOffsetJP, 1, glm::value_ptr(toffsetLimpiarJP));

		glm::mat4 modelFabrica = modelJurasicPark;
		modelFabrica = glm::translate(modelFabrica, posicionFabrica);
		modelFabrica = glm::rotate(modelFabrica, rotacionFabricaY * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		modelFabrica = glm::scale(modelFabrica, glm::vec3(escalaFabrica, escalaFabrica, escalaFabrica));
		glUniformMatrix4fv(uniformModelJP, 1, GL_FALSE, glm::value_ptr(modelFabrica));
		Fabrica_M.RenderModel();

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDepthMask(GL_FALSE);
		humoTexture.UseTexture();
		RenderHumo(modelFabrica, glm::vec3(38.0f, 130.0f, -32.0f), glm::vec3(18.0f, 50.0f, 1.0f), uniformModelJP, uniformTextureOffsetJP, uniformAlphaJP);
		RenderHumo(modelFabrica, glm::vec3(38.0f, 130.0f, 2.0f), glm::vec3(18.0f, 50.0f, 1.0f), uniformModelJP, uniformTextureOffsetJP, uniformAlphaJP);
		RenderHumo(modelFabrica, glm::vec3(38.0f, 130.0f, 34.0f), glm::vec3(18.0f, 50.0f, 1.0f), uniformModelJP, uniformTextureOffsetJP, uniformAlphaJP);
		glUniform2fv(uniformTextureOffsetJP, 1, glm::value_ptr(toffsetLimpiarJP));
		glDepthMask(GL_TRUE);
		glDisable(GL_BLEND);

		glm::mat4 modelVolcan = modelJurasicPark;
		modelVolcan = glm::translate(modelVolcan, posicionVolcan);
		modelVolcan = glm::rotate(modelVolcan, rotacionVolcanY * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		modelVolcan = glm::scale(modelVolcan, glm::vec3(escalaVolcan, escalaVolcan, escalaVolcan));
		glUniformMatrix4fv(uniformModelJP, 1, GL_FALSE, glm::value_ptr(modelVolcan));
		Volcan_M.RenderModel();

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDepthMask(GL_FALSE);
		humoTexture.UseTexture();
		RenderHumo(modelVolcan, glm::vec3(0.0f, 80.0f, 0.0f), glm::vec3(60.0f, 120.0f, 1.0f), uniformModelJP, uniformTextureOffsetJP, uniformAlphaJP);
		glUniform2fv(uniformTextureOffsetJP, 1, glm::value_ptr(toffsetLimpiarJP));
		glDepthMask(GL_TRUE);
		glDisable(GL_BLEND);


		glUniform1f(uniformAlphaJP, 1.0f);
		glUniform2fv(uniformTextureOffsetJP, 1, glm::value_ptr(toffsetLimpiarJP));
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);

		glm::mat4 modelGeodesico = modelJurasicPark;
		modelGeodesico = glm::translate(modelGeodesico, posicionGeodesico);
		modelGeodesico = glm::rotate(modelGeodesico, rotacionGeodesicoY * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		modelGeodesico = glm::scale(modelGeodesico, glm::vec3(escalaGeodesico, escalaGeodesico, escalaGeodesico));
		glUniformMatrix4fv(uniformModelJP, 1, GL_FALSE, glm::value_ptr(modelGeodesico));
		Geodesico_M.RenderModel();

		glUniform1f(uniformAlphaJP, 1.0f);
		glUniform2fv(uniformTextureOffsetJP, 1, glm::value_ptr(toffsetLimpiarJP));
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);

		glm::mat4 modelCouncil3 = modelJurasicPark;
		modelCouncil3 = glm::translate(modelCouncil3, posicionCouncil3);
		modelCouncil3 = glm::rotate(modelCouncil3, rotacionCouncil3Y * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		modelCouncil3 = glm::scale(modelCouncil3, glm::vec3(escalaCouncil3, escalaCouncil3, escalaCouncil3));
		glUniformMatrix4fv(uniformModelJP, 1, GL_FALSE, glm::value_ptr(modelCouncil3));
		Council3_M.RenderModel();

		glm::mat4 modelBarco = modelJurasicPark;
		modelBarco = glm::translate(modelBarco, glm::vec3(movBarco_x, -10.0f, movBarco_z));
		modelBarco = glm::rotate(modelBarco, glm::radians(giroBarco), glm::vec3(0.0f, 1.0f, 0.0f));
		modelBarco = glm::scale(modelBarco, glm::vec3(0.05f, 0.05f, 0.05f));
		glUniformMatrix4fv(uniformModelJP, 1, GL_FALSE, glm::value_ptr(modelBarco));
		Barco_M.RenderModel();

		glm::mat4 modelHelice = modelBarco;
		modelHelice = glm::translate(modelHelice, glm::vec3(600.0f, 65.0f, 4.0f));
		modelHelice = glm::rotate(modelHelice, glm::radians(rotHeliceBarco), glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModelJP, 1, GL_FALSE, glm::value_ptr(modelHelice));
		Helice_M.RenderModel();

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDepthMask(GL_FALSE);
		humoTexture.UseTexture();
		RenderHumo(modelBarco, glm::vec3(180.0f, 900.0f, 1.0f), glm::vec3(140.0f, 380.0f, 200.0f), uniformModelJP, uniformTextureOffsetJP, uniformAlphaJP);
		glUniform2fv(uniformTextureOffsetJP, 1, glm::value_ptr(toffsetLimpiarJP));
		glDepthMask(GL_TRUE);
		glDisable(GL_BLEND);

		// =====================================
		// RENDER DE AS
		// =====================================
		glm::mat4 modelCuerpo = modelJurasicPark;
		modelCuerpo = glm::translate(modelCuerpo, glm::vec3(posX_AS, 0.5f, posZ_AS));
		modelCuerpo = glm::rotate(modelCuerpo, glm::radians(anguloAS), glm::vec3(0.0f, 1.0f, 0.0f));
		modelCuerpo = glm::scale(modelCuerpo, glm::vec3(0.33f, 0.33f, 0.33f));

		glUniformMatrix4fv(uniformModelJP, 1, GL_FALSE, glm::value_ptr(modelCuerpo));
		As_M.RenderModel();

		// Pierna Derecha
		glm::mat4 modelAS = modelCuerpo;
		modelAS = glm::translate(modelAS, glm::vec3(-1.4f, -6.45f, -3.0f));
		modelAS = glm::rotate(modelAS, glm::radians(rotacionPierna), glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModelJP, 1, GL_FALSE, glm::value_ptr(modelAS));
		Piernad_as_M.RenderModel();

		// Pierna Izquierda
		modelAS = modelCuerpo;
		modelAS = glm::translate(modelAS, glm::vec3(1.4f, -5.3f, -3.0f));
		modelAS = glm::rotate(modelAS, glm::radians(-rotacionPierna), glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModelJP, 1, GL_FALSE, glm::value_ptr(modelAS));
		Piernai_as_M.RenderModel();

		// Cola
		modelAS = modelCuerpo;
		modelAS = glm::translate(modelAS, glm::vec3(0.0f, -8.2f, -2.2f));
		modelAS = glm::rotate(modelAS, glm::radians(movimientoCola), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModelJP, 1, GL_FALSE, glm::value_ptr(modelAS));
		ColaAs_M.RenderModel();


		// =====================================
		// RENDER DE PARIS
		// =====================================
		glm::mat4 modelCuerpoParis = modelJurasicPark;
		modelCuerpoParis = glm::translate(modelCuerpoParis, glm::vec3(posX_PARIS, 0.5f, posZ_PARIS));
		modelCuerpoParis = glm::rotate(modelCuerpoParis, glm::radians(anguloPARIS), glm::vec3(0.0f, 1.0f, 0.0f));
		modelCuerpoParis = glm::scale(modelCuerpoParis, glm::vec3(0.33f, 0.33f, 0.33f));

		glUniformMatrix4fv(uniformModelJP, 1, GL_FALSE, glm::value_ptr(modelCuerpoParis));
		Paris_M.RenderModel();

		// Pierna 1
		glm::mat4 modelParis = modelCuerpoParis;
		modelParis = glm::translate(modelParis, glm::vec3(-0.3f, -4.0f, -2.35f));
		modelParis = glm::rotate(modelParis, glm::radians(rotacionPiernaParis), glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModelJP, 1, GL_FALSE, glm::value_ptr(modelParis));
		Piernas1_M.RenderModel();

		// Pierna 2
		modelParis = modelCuerpoParis;
		modelParis = glm::translate(modelParis, glm::vec3(0.3f, -4.0f, -2.35f));
		modelParis = glm::rotate(modelParis, glm::radians(-rotacionPiernaParis), glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModelJP, 1, GL_FALSE, glm::value_ptr(modelParis));
		Piernas2_M.RenderModel();

		// Cola
		modelParis = modelCuerpoParis;
		modelParis = glm::translate(modelParis, glm::vec3(-0.6f, -2.25f, -5.0f));
		modelParis = glm::rotate(modelParis, glm::radians(movimientoColaParis), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModelJP, 1, GL_FALSE, glm::value_ptr(modelParis));
		ColaParis_M.RenderModel();


		// =====================================
		// RENDER DE TERRY
		// =====================================
		glm::mat4 modelTerry = modelJurasicPark;
		modelTerry = glm::translate(modelTerry, glm::vec3(100.0f, -2.0f, -140.0f));
		modelTerry = glm::scale(modelTerry, glm::vec3(0.05f, 0.05f, 0.05f));
		glUniformMatrix4fv(uniformModelJP, 1, GL_FALSE, glm::value_ptr(modelTerry));
		Terry_M.RenderModel();

		glUseProgram(0);

		mainWindow.swapBuffers();
	}

	return 0;
}
