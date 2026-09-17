//práctica 3: Modelado Geométrico y Cámara Sintética.
#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>
#include <glew.h>
#include <glfw3.h>

//glm
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <gtc/random.hpp>

//clases para dar orden y limpieza al código
#include "Mesh.h"
#include "Shader.h"
#include "Sphere.h"
#include "Window.h"
#include "Camera.h"
#include "definition.h"

//tecla E: Rotar sobre el eje X
//tecla R: Rotar sobre el eje Y
//tecla T: Rotar sobre el eje Z

using std::vector;

//Dimensiones de la ventana
const float toRadians = 3.14159265f / 180.0f; //grados a radianes
const float PI = 3.14159265f;
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;
Camera camera;
Window mainWindow;
vector<Mesh*> meshList;
vector<Shader> shaderList;

// --- Ejercicio 2: union de 8 piramides con caras de colores ---
MeshColor piramideColorMesh;                    // 1 sola piramide con color por vertice (se reutiliza 8 veces)
vector<glm::mat4> piramidesUnionTransforms;      // rotacion + traslacion de cada una de las 8 piramides

//Vertex Shader
static const char* vShader = "shaders/shader.vert";
static const char* fShader = "shaders/shader.frag";
static const char* vShaderColor = "shaders/shadercolor.vert";
Sphere sp = Sphere(1.0, 20, 20);

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

void CrearPiramideTriangular()
{
	unsigned int indices_piramide_triangular[] = {
		0, 1, 2,
		1, 3, 2,
		3, 0, 2,
		1, 0, 3
	};
	GLfloat vertices_piramide_triangular[] = {
		-0.5f, -0.5f,  0.0f,  //0
		 0.5f, -0.5f,  0.0f,  //1
		 0.0f,  0.5f, -0.25f, //2
		 0.0f, -0.5f, -0.5f   //3
	};
	Mesh* piramidet = new Mesh();
	piramidet->CreateMesh(vertices_piramide_triangular, indices_piramide_triangular, 12, 12);
	meshList.push_back(piramidet);
}

// Pirámide cuadrangular centrada en el origen.
void CrearPiramideCuadrangular()
{
	unsigned int piramidecuadrangular_indices[] = {
		0, 3, 4, //frontal
		3, 2, 4, //izquierda
		2, 1, 4, //trasera
		1, 0, 4, //derecha
		0, 1, 2, //abajo1
		0, 2, 3  //abajo2
	};
	GLfloat piramidecuadrangular_vertices[] = {
		 0.5f, -0.5f,  0.5f, // 0
		 0.5f, -0.5f, -0.5f, // 1
		-0.5f, -0.5f, -0.5f, // 2
		-0.5f, -0.5f,  0.5f, // 3
		 0.0f,  0.5f,  0.0f  // 4 (punta)
	};
	Mesh* piramidec = new Mesh();
	piramidec->CreateMesh(piramidecuadrangular_vertices, piramidecuadrangular_indices, 15, 18);
	meshList.push_back(piramidec);
}

// Piramide cuadrangular con un color distinto por cada cara.
void CrearPiramideCuadrangularColor()
{
	// Mismos vertices que CrearPiramideCuadrangular()
	glm::vec3 v0(0.5f, -0.5f, 0.5f);
	glm::vec3 v1(0.5f, -0.5f, -0.5f);
	glm::vec3 v2(-0.5f, -0.5f, -0.5f);
	glm::vec3 v3(-0.5f, -0.5f, 0.5f);
	glm::vec3 v4(0.0f, 0.5f, 0.0f); // punta

	glm::vec3 rojo(1.0f, 0.0f, 0.0f);
	glm::vec3 verde(0.0f, 1.0f, 0.0f);
	glm::vec3 amarillo(1.0f, 1.0f, 0.0f);
	glm::vec3 magenta(1.0f, 0.0f, 1.0f);
	glm::vec3 azul(0.0f, 0.0f, 1.0f);

	vector<VertexColor> v;

	// 4 caras triangulares laterales, cada una de un color (rojo, verde, amarillo, magenta)
	v.push_back(VertexColor(v0, rojo));      v.push_back(VertexColor(v3, rojo));      v.push_back(VertexColor(v4, rojo));      // frontal
	v.push_back(VertexColor(v3, verde));     v.push_back(VertexColor(v2, verde));     v.push_back(VertexColor(v4, verde));     // izquierda
	v.push_back(VertexColor(v2, amarillo));  v.push_back(VertexColor(v1, amarillo));  v.push_back(VertexColor(v4, amarillo));  // trasera
	v.push_back(VertexColor(v1, magenta));   v.push_back(VertexColor(v0, magenta));   v.push_back(VertexColor(v4, magenta));   // derecha

	// cara cuadrada (base), 2 triangulos, ambos azules para que se vea como una sola cara azul.
	v.push_back(VertexColor(v0, azul)); v.push_back(VertexColor(v1, azul)); v.push_back(VertexColor(v2, azul));
	v.push_back(VertexColor(v0, azul)); v.push_back(VertexColor(v2, azul)); v.push_back(VertexColor(v3, azul));

	piramideColorMesh.CreateMeshColor(&v[0].position.x, (unsigned int)(v.size() * 6));
}
glm::mat4 RotarHaciaDireccion(glm::vec3 dir)
{
	glm::vec3 up(0.0f, 1.0f, 0.0f);
	dir = glm::normalize(dir);
	glm::vec3 eje = glm::cross(up, dir);
	float cosAngulo = glm::dot(up, dir);
	if (cosAngulo > 1.0f) cosAngulo = 1.0f;
	if (cosAngulo < -1.0f) cosAngulo = -1.0f;

	if (glm::length(eje) < 0.0001f)
	{
		if (cosAngulo > 0.0f) return glm::mat4(1.0f);
		return glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	}
	return glm::rotate(glm::mat4(1.0f), acos(cosAngulo), glm::normalize(eje));
}

void CrearUnionDe8Piramides(float distanciaCentro, float escalaPiramide)
{
	piramidesUnionTransforms.clear();
	for (int sx = -1; sx <= 1; sx += 2)
	{
		for (int sy = -1; sy <= 1; sy += 2)
		{
			for (int sz = -1; sz <= 1; sz += 2)
			{
				glm::vec3 dir = glm::normalize(glm::vec3((float)sx, (float)sy, (float)sz));
				glm::mat4 t = glm::translate(glm::mat4(1.0f), dir * distanciaCentro);
				t = t * RotarHaciaDireccion(dir);
				t = glm::scale(t, glm::vec3(escalaPiramide));
				piramidesUnionTransforms.push_back(t);
			}
		}
	}
}

void CrearCilindro(int res, float R) {
	int n, i;
	GLfloat dt = 2 * PI / res, x, z, y = -0.5f;

	vector<GLfloat> vertices;
	vector<unsigned int> indices;

	for (n = 0; n <= (res); n++) {
		if (n != res) {
			x = R * cos((n)*dt);
			z = R * sin((n)*dt);
		}
		else {
			x = R * cos((0) * dt);
			z = R * sin((0) * dt);
		}
		for (i = 0; i < 6; i++) {
			switch (i) {
			case 0: vertices.push_back(x); break;
			case 1: vertices.push_back(y); break;
			case 2: vertices.push_back(z); break;
			case 3: vertices.push_back(x); break;
			case 4: vertices.push_back(0.5); break;
			case 5: vertices.push_back(z); break;
			}
		}
	}

	for (n = 0; n <= (res); n++) {
		int base = n * 2;
		indices.push_back(base);
		indices.push_back(base + 1);
		indices.push_back(base + 2);

		indices.push_back(base + 1);
		indices.push_back(base + 3);
		indices.push_back(base + 2);
	}

	Mesh* cilindro = new Mesh();
	cilindro->CreateMeshGeometry(vertices, indices, vertices.size(), indices.size());
	meshList.push_back(cilindro);
}

void CrearCono(int res, float R) {
	int n, i;
	GLfloat dt = 2 * PI / res, x, z, y = -0.5f;

	vector<GLfloat> vertices;
	vector<unsigned int> indices;

	vertices.push_back(0.0); vertices.push_back(0.5f); vertices.push_back(0.0f);
	vertices.push_back(0.0); vertices.push_back(-0.5f); vertices.push_back(0.0f);

	for (n = 0; n <= (res); n++) {
		x = R * cos((n)*dt);
		z = R * sin((n)*dt);
		for (i = 0; i < 3; i++) {
			switch (i) {
			case 0: vertices.push_back(x); break;
			case 1: vertices.push_back(y); break;
			case 2: vertices.push_back(z); break;
			}
		}
	}
	for (i = 2; i <= res + 1; i++){
		indices.push_back(0);
		indices.push_back(i);
		indices.push_back(1 + i);

		indices.push_back(1);
		indices.push_back(i + 1);
		indices.push_back(i);
		}

	Mesh* cono = new Mesh();
	cono->CreateMeshGeometry(vertices, indices, vertices.size(), indices.size());
	meshList.push_back(cono);
}

void CreateShaders()
{
	Shader* shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);

	Shader* shader2 = new Shader();
	shader2->CreateFromFiles(vShaderColor, fShader);
	shaderList.push_back(*shader2);
}

int main()
{
	mainWindow = Window(800, 600);
	mainWindow.Initialise();

	CrearCubo();                  // índice 0 en meshList
	CrearPiramideTriangular();    // índice 1 en meshList
	CrearCilindro(20, 0.5f);      // índice 2 en meshList
	CrearCono(20, 0.5f);          // índice 3 en meshList
	CrearPiramideCuadrangular();  // indice 4 en meshList
	CrearPiramideCuadrangularColor();     // ejercicio 2: piramide con color por cara
	CrearUnionDe8Piramides(1.0f, 0.45f);  // ejercicio 2: 8 transformaciones (una por piramide)
	CreateShaders();

	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 0.3f, 0.3f);

	GLuint uniformProjection = 0;
	GLuint uniformModel = 0;
	GLuint uniformView = 0;
	GLuint uniformColor = 0;
	glm::mat4 projection = glm::perspective(glm::radians(60.0f), mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 100.0f);

	sp.init();
	sp.load();

	glm::mat4 model(1.0);
	glm::vec3 color = glm::vec3(0.0f, 0.0f, 0.0f);

	float rotacionAuto = 0.0f;

	// Activar Depth Test
	glEnable(GL_DEPTH_TEST);

	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;

		glfwPollEvents();
		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

		// 1. LIMPIA PANTALLA Y BUFFER DE PROFUNDIDAD
		glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// 2. SHADER Y UNIFORMS
		shaderList[0].useShader();
		uniformModel = shaderList[0].getModelLocation();
		uniformProjection = shaderList[0].getProjectLocation();
		uniformView = shaderList[0].getViewLocation();
		uniformColor = shaderList[0].getColorLocation();

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));

		glm::mat4 baseModel(1.0f);
		baseModel = glm::rotate(baseModel, glm::radians(mainWindow.getrotax()), glm::vec3(1.0f, 0.0f, 0.0f));
		baseModel = glm::rotate(baseModel, glm::radians(mainWindow.getrotay()), glm::vec3(0.0f, 1.0f, 0.0f));
		baseModel = glm::rotate(baseModel, glm::radians(mainWindow.getrotaz()), glm::vec3(0.0f, 0.0f, 1.0f));

		// 3. PLANO DEL PISO NEGRO
		model = baseModel;
		model = glm::translate(model, glm::vec3(0.0f, -2.5f, -5.0f));
		model = glm::scale(model, glm::vec3(12.0f, 0.1f, 12.0f));

		color = glm::vec3(0.0f, 0.0f, 0.0f);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[0]->RenderMesh();

		rotacionAuto += 1.0f * deltaTime;

		// 4. COHETE ESPACIAL
		glm::mat4 rocketModel = glm::translate(baseModel, glm::vec3(0.0f, 0.2f, -5.0f));

		// A. CUERPO - CILINDRO (meshList[2])
		model = glm::scale(rocketModel, glm::vec3(1.0f, 2.0f, 1.0f));
		color = glm::vec3(0.8f, 0.8f, 0.8f);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[2]->RenderMesh();

		// B. NARIZ / PUNTA - CONO (meshList[3])
		model = glm::translate(rocketModel, glm::vec3(0.0f, 1.75f, 0.0f));
		model = glm::scale(model, glm::vec3(1.05f, 1.5f, 1.05f));
		color = glm::vec3(0.9f, 0.1f, 0.1f);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[3]->RenderMesh();

		// C. BASE MOTORES - CUBO (meshList[0])
		model = glm::translate(rocketModel, glm::vec3(0.0f, -1.1f, 0.0f));
		model = glm::scale(model, glm::vec3(1.1f, 0.3f, 1.1f));
		color = glm::vec3(0.2f, 0.2f, 0.2f);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[0]->RenderMesh();

		// D. VENTANA - ESFERA (sp)
		model = glm::translate(rocketModel, glm::vec3(0.0f, 0.3f, 0.52f));
		model = glm::scale(model, glm::vec3(0.25f, 0.25f, 0.25f));
		color = glm::vec3(0.0f, 0.7f, 1.0f);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		sp.render();

		// E. ALETA DERECHA - PIRÁMIDE CUADRANGULAR (meshList[4])
		model = glm::translate(rocketModel, glm::vec3(0.75f, -0.6f, 0.0f));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.6f, 0.5f, 0.2f));
		color = glm::vec3(0.9f, 0.4f, 0.0f);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[4]->RenderMesh();

		// F. ALETA IZQUIERDA - PIRÁMIDE CUADRANGULAR (meshList[4])
		model = glm::translate(rocketModel, glm::vec3(-0.75f, -0.6f, 0.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.6f, 0.5f, 0.2f));
		color = glm::vec3(0.9f, 0.4f, 0.0f);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[4]->RenderMesh();

		// 5. EJERCICIO 2: FIGURA DE ENDER
		shaderList[1].useShader();
		GLuint uniformModelColor = shaderList[1].getModelLocation();
		GLuint uniformProjectionColor = shaderList[1].getProjectLocation();
		GLuint uniformViewColor = shaderList[1].getViewLocation();
		GLuint uniformColorColor = shaderList[1].getColorLocation();

		glUniformMatrix4fv(uniformProjectionColor, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformViewColor, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		if (uniformColorColor != (GLuint)-1)
		{
			glm::vec3 blanco(1.0f, 1.0f, 1.0f);
			glUniform3fv(uniformColorColor, 1, glm::value_ptr(blanco));
		}

		// Origen de toda la estructura
		glm::mat4 enderModel = glm::translate(baseModel, glm::vec3(-2.5f, 0.2f, -5.0f));

		// ==================== CONJUNTO SUPERIOR ====================
		glm::mat4 topSet = glm::translate(enderModel, glm::vec3(0.0f, 0.5f, 0.0f));

		// 1. Pirámide Superior (Punta hacia +Y)
		model = glm::translate(topSet, glm::vec3(0.0f, 0.25f, 0.0f));
		model = glm::rotate(model, glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.2f, 0.5f, 1.2f));
		glUniformMatrix4fv(uniformModelColor, 1, GL_FALSE, glm::value_ptr(model));
		piramideColorMesh.RenderMeshColor();

		// 2. Pirámide Superior Abajo (Punta central hacia -Y)
		model = glm::translate(topSet, glm::vec3(0.0f, -0.25f, 0.0f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::rotate(model, glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.2f, 0.5f, 1.2f));
		glUniformMatrix4fv(uniformModelColor, 1, GL_FALSE, glm::value_ptr(model));
		piramideColorMesh.RenderMeshColor();


		// ==================== CONJUNTO INFERIOR ====================
		glm::mat4 bottomSet = glm::translate(enderModel, glm::vec3(0.0f, -0.5f, 0.0f));

		// 3. Pirámide Inferior Arriba (Punta central hacia +Y)
		model = glm::translate(bottomSet, glm::vec3(0.0f, 0.25f, 0.0f));
		model = glm::rotate(model, glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.2f, 0.5f, 1.2f));
		glUniformMatrix4fv(uniformModelColor, 1, GL_FALSE, glm::value_ptr(model));
		piramideColorMesh.RenderMeshColor();

		// 4. Pirámide Inferior Abajo (Punta hacia -Y)
		model = glm::translate(bottomSet, glm::vec3(0.0f, -0.25f, 0.0f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::rotate(model, glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.2f, 0.5f, 1.2f));
		glUniformMatrix4fv(uniformModelColor, 1, GL_FALSE, glm::value_ptr(model));
		piramideColorMesh.RenderMeshColor();


		// ==================== PIRÁMIDES LATERALES ====================
		// 5. Pirámide Lateral Izquierda (base hacia -X)
		{
			glm::mat4 tilt = glm::rotate(glm::mat4(1.0f),
				glm::radians(-45.0f),
				glm::vec3(0.0f, 1.0f, 0.0f));

			glm::mat4 localModel = glm::translate(glm::mat4(1.0f), glm::vec3(-0.25f, 0.0f, 0.0f));
			localModel = glm::rotate(localModel, glm::radians(-90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
			localModel = glm::scale(localModel, glm::vec3(1.1f, 0.5f, 1.2f));

			model = enderModel * tilt * localModel; 
			glUniformMatrix4fv(uniformModelColor, 1, GL_FALSE, glm::value_ptr(model));
			piramideColorMesh.RenderMeshColor();
		}

		// 6. Pirámide Lateral Derecha (base hacia +X)
		{
			glm::mat4 tilt = glm::rotate(glm::mat4(1.0f),
				glm::radians(45.0f),
				glm::vec3(0.0f, 1.0f, 0.0f));

			glm::mat4 localModel = glm::translate(glm::mat4(1.0f), glm::vec3(0.25f, 0.0f, 0.0f));
			localModel = glm::rotate(localModel, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
			localModel = glm::scale(localModel, glm::vec3(1.1f, 0.5f, 1.2f));

			model = enderModel * tilt * localModel;
			glUniformMatrix4fv(uniformModelColor, 1, GL_FALSE, glm::value_ptr(model));
			piramideColorMesh.RenderMeshColor();
		}

		// ==================== PIRÁMIDE MULTICOLOR (EN EL LADO DERECHO DEL COHETE) ====================
		model = glm::translate(baseModel, glm::vec3(2.5f, 0.2f, -5.0f));
		model = glm::scale(model, glm::vec3(1.2f, 1.2f, 1.2f));
		glUniformMatrix4fv(uniformModelColor, 1, GL_FALSE, glm::value_ptr(model));
		piramideColorMesh.RenderMeshColor();

		glUseProgram(0);
		mainWindow.swapBuffers();
	}
	return 0;
}

		/*
		//ejercicio: Instanciar primitivas geométricas para recrear las figuras 2 y 3 de la práctica pasada en 3D,
		//se requiere que exista piso
		*/
		