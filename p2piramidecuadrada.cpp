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
const float toRadians = 3.14159265f/180.0; //grados a radianes
Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<MeshColor*> meshColorList;
std::vector<Shader>shaderList;
//Vertex Shader
static const char* vShader = "shaders/shader.vert";
static const char* fShader = "shaders/shader.frag";
static const char* vShaderColor = "shaders/shadercolor.vert";
static const char* fShaderColor = "shaders/shadercolor.frag";
//shaders nuevos se crearían acá (Shaders para las letras con matrices y color base)

static const char* vShaderLetraC = "#version 330\n"
"layout (location =0) in vec3 pos;\n"
"out vec4 vColor;\n"
"uniform mat4 model;\n"
"uniform mat4 projection;\n"
"void main()\n"
"{\n"
"	gl_Position=projection*model*vec4(pos,1.0f);\n"
"	vColor=vec4(0.0f, 0.6f, 1.0f, 1.0f);\n" // Azul para la C
"}";

static const char* vShaderLetraR = "#version 330\n"
"layout (location =0) in vec3 pos;\n"
"out vec4 vColor;\n"
"uniform mat4 model;\n"
"uniform mat4 projection;\n"
"void main()\n"
"{\n"
"	gl_Position=projection*model*vec4(pos,1.0f);\n"
"	vColor=vec4(1.0f, 0.3f, 0.3f, 1.0f);\n" // Rojo para la R
"}";

static const char* vShaderLetraD = "#version 330\n"
"layout (location =0) in vec3 pos;\n"
"out vec4 vColor;\n"
"uniform mat4 model;\n"
"uniform mat4 projection;\n"
"void main()\n"
"{\n"
"	gl_Position=projection*model*vec4(pos,1.0f);\n"
"	vColor=vec4(0.2f, 0.9f, 0.3f, 1.0f);\n" // Verde para la D
"}";

static const char* fShaderLetras = "#version 330\n"
"in vec4 vColor;\n"
"out vec4 color;\n"
"void main()\n"
"{\n"
"	color = vColor;\n"
"}";

// Variables globales para las letras C, R, D
GLuint VAO_Letras, VBO_Letras;

float angulo = 0.0f;

//color café/marrón en RGB : 0.478, 0.255, 0.067

using std::vector;

//Pirámide triangular regular
void CreaPiramide()
{
	unsigned int indices[] = {
		0,1,2,
		0,3,1,
		1,3,2,
		2,3,0
		
	};
	GLfloat vertices[] = {
		-0.5f,-0.5f, 0.5f,	//0
		0.5f,-0.5f, 0.5f,	//1
		0.0f,-0.5f, -0.5f,	//2
		0.0f,0.5f, 0.0f,	//3

	};
	Mesh *piramidetriangular = new Mesh();
	piramidetriangular->CreateMesh(vertices, indices, 12, 12);
	meshList.push_back(piramidetriangular);
}

void CrearLetrasyFiguras()
{
	// 0. Triángulo Amarillo
	GLfloat triAmarillo[] = {
		-0.5f, -0.5f, 0.0f,   1.0f, 1.0f, 0.0f,
		 0.5f, -0.5f, 0.0f,   1.0f, 1.0f, 0.0f,
		 0.0f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f
	};
	MeshColor* mTriAmarillo = new MeshColor();
	mTriAmarillo->CreateMeshColor(triAmarillo, 18);
	meshColorList.push_back(mTriAmarillo); // [0]

	// 1. Triángulo Verde
	GLfloat triVerde[] = {
		-0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,
		 0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,
		 0.0f,  0.5f, 0.0f,   0.0f, 1.0f, 0.0f
	};
	MeshColor* mTriVerde = new MeshColor();
	mTriVerde->CreateMeshColor(triVerde, 18);
	meshColorList.push_back(mTriVerde); // [1]

	// 2. Triángulo Rojo
	GLfloat triRojo[] = {
		-0.5f, -0.5f, 0.0f,   1.0f, 0.0f, 0.0f,
		 0.5f, -0.5f, 0.0f,   1.0f, 0.0f, 0.0f,
		 0.0f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f
	};
	MeshColor* mTriRojo = new MeshColor();
	mTriRojo->CreateMeshColor(triRojo, 18);
	meshColorList.push_back(mTriRojo); // [2]

	// 3. Triángulo Púrpura
	GLfloat triMagenta[] = {
		-0.5f, -0.5f, 0.0f,   1.0f, 0.0f, 1.0f,
		 0.5f, -0.5f, 0.0f,   1.0f, 0.0f, 1.0f,
		 0.0f,  0.5f, 0.0f,   1.0f, 0.0f, 1.0f
	};
	MeshColor* mTriMagenta = new MeshColor();
	mTriMagenta->CreateMeshColor(triMagenta, 18);
	meshColorList.push_back(mTriMagenta); // [3]

	// 4. Cuadrado Azul (base)
	GLfloat cuadradoAzul[] = {
		-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,
		 0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,
		 0.5f,  0.5f, 0.0f,   0.0f, 0.0f, 1.0f,
		-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,
		 0.5f,  0.5f, 0.0f,   0.0f, 0.0f, 1.0f,
		-0.5f,  0.5f, 0.0f,   0.0f, 0.0f, 1.0f
	};
	MeshColor* mCuadAzul = new MeshColor();
	mCuadAzul->CreateMeshColor(cuadradoAzul, 36);
	meshColorList.push_back(mCuadAzul); // [4]

	// 5. Cuadrado Café
	GLfloat cuadradoCafe[] = {
		-0.5f, -0.5f, 0.0f,   0.478f, 0.255f, 0.067f,
		 0.5f, -0.5f, 0.0f,   0.478f, 0.255f, 0.067f,
		 0.5f,  0.5f, 0.0f,   0.478f, 0.255f, 0.067f,
		-0.5f, -0.5f, 0.0f,   0.478f, 0.255f, 0.067f,
		 0.5f,  0.5f, 0.0f,   0.478f, 0.255f, 0.067f,
		-0.5f,  0.5f, 0.0f,   0.478f, 0.255f, 0.067f
	};
	MeshColor* mCuadCafe = new MeshColor();
	mCuadCafe->CreateMeshColor(cuadradoCafe, 36);
	meshColorList.push_back(mCuadCafe); // [5]

	// 6. Cuadrado Negro
	GLfloat cuadradoNegro[] = {
		-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 0.0f,
		 0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 0.0f,
		 0.5f,  0.5f, 0.0f,   0.0f, 0.0f, 0.0f,
		-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 0.0f,
		 0.5f,  0.5f, 0.0f,   0.0f, 0.0f, 0.0f,
		-0.5f,  0.5f, 0.0f,   0.0f, 0.0f, 0.0f
	};
	MeshColor* mCuadNegro = new MeshColor();
	mCuadNegro->CreateMeshColor(cuadradoNegro, 36);
	meshColorList.push_back(mCuadNegro); // [6]

	// 7. Cuadrado Rojo (para el rombo intermedio del centro)
	GLfloat cuadradoRojo[] = {
		-0.5f, -0.5f, 0.0f,   1.0f, 0.0f, 0.0f,
		 0.5f, -0.5f, 0.0f,   1.0f, 0.0f, 0.0f,
		 0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,
		-0.5f, -0.5f, 0.0f,   1.0f, 0.0f, 0.0f,
		 0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,
		-0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f
	};
	MeshColor* mCuadRojo = new MeshColor();
	mCuadRojo->CreateMeshColor(cuadradoRojo, 36);
	meshColorList.push_back(mCuadRojo); // [7]
}

//función para crear pirámide cuadrangular unitaria
void CrearPiramideCuadrangular()
{
	unsigned int piramidecuadrangular_indices[] = {
		0,3,4,
		3,2,4,
		2,1,4,
		1,0,4,
		0,1,2,
		2,3,0

	};
	GLfloat piramidecuadrangular_vertices[] = {
		0.5f,-0.5f,0.5f,
		0.5f,-0.5f,-0.5f,
		-0.5f,-0.5f,-0.5f,
		-0.5f,-0.5f,0.5f,
		0.0f,0.5f,0.0f,
	};
	Mesh* piramide = new Mesh();
	piramide->CreateMesh(piramidecuadrangular_vertices, piramidecuadrangular_indices, 15, 18);
	meshList.push_back(piramide);
}

//Vértices de un cubo
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
Mesh *cubo = new Mesh();
cubo->CreateMesh(cubo_vertices, cubo_indices,24, 36);
meshList.push_back(cubo);
}

// Función para diseñar los vértices de las letras C, R, D
void CrearLetrasCRD()
{
	GLfloat vertices_letras[] = {
		//Letra C
		// Parte de arriba
		-0.9f, 0.5f, 0.0f,  -0.5f, 0.5f, 0.0f,  -0.9f, 0.3f, 0.0f,
		-0.5f, 0.5f, 0.0f,  -0.5f, 0.3f, 0.0f,  -0.9f, 0.3f, 0.0f,
		// Parte izquierda
		-0.9f, 0.3f, 0.0f,  -0.7f, 0.3f, 0.0f,  -0.9f,-0.3f, 0.0f,
		-0.7f, 0.3f, 0.0f,  -0.7f,-0.3f, 0.0f,  -0.9f,-0.3f, 0.0f,
		// Parte de abajo
		-0.9f,-0.3f, 0.0f,  -0.5f,-0.3f, 0.0f,  -0.9f,-0.5f, 0.0f,
		-0.5f,-0.3f, 0.0f,  -0.5f,-0.5f, 0.0f,  -0.9f,-0.5f, 0.0f,

		//Letra R
		//parte de izquierda
		-0.3f, 0.5f, 0.0f,  -0.1f, 0.5f, 0.0f,  -0.3f,-0.5f, 0.0f,
		-0.1f, 0.5f, 0.0f,  -0.1f,-0.5f, 0.0f,  -0.3f,-0.5f, 0.0f,
		//parte de superior
		-0.1f, 0.5f, 0.0f,   0.3f, 0.5f, 0.0f,  -0.1f, 0.3f, 0.0f,
		0.3f, 0.5f, 0.0f,   0.3f, 0.3f, 0.0f,  -0.1f, 0.3f, 0.0f,
		//parte derecha
		0.1f, 0.3f, 0.0f,   0.3f, 0.3f, 0.0f,   0.1f, 0.1f, 0.0f,
		0.3f, 0.3f, 0.0f,   0.3f, 0.1f, 0.0f,   0.1f, 0.1f, 0.0f,
		//parte inferior
		-0.1f, 0.1f, 0.0f,   0.3f, 0.1f, 0.0f,  -0.1f,-0.1f, 0.0f,
		0.3f, 0.1f, 0.0f,   0.3f,-0.1f, 0.0f,  -0.1f,-0.1f, 0.0f,
		//Pata diagonal
		0.0f,-0.1f, 0.0f,   0.2f,-0.1f, 0.0f,   0.1f,-0.5f, 0.0f,
		0.2f,-0.1f, 0.0f,   0.3f,-0.5f, 0.0f,   0.1f,-0.5f, 0.0f,

		//Letra D
		//parte de izquierda
		0.5f, 0.5f, 0.0f,   0.7f, 0.5f, 0.0f,   0.5f,-0.5f, 0.0f,
		0.7f, 0.5f, 0.0f,   0.7f,-0.5f, 0.0f,   0.5f,-0.5f, 0.0f,
		//parte de superior
		0.7f, 0.5f, 0.0f,   0.9f, 0.5f, 0.0f,   0.7f, 0.3f, 0.0f,
		0.9f, 0.5f, 0.0f,   0.9f, 0.3f, 0.0f,   0.7f, 0.3f, 0.0f,
		//parte derecha
		0.8f, 0.3f, 0.0f,   0.9f, 0.3f, 0.0f,   0.8f,-0.3f, 0.0f,
		0.9f, 0.3f, 0.0f,   0.9f,-0.3f, 0.0f,   0.8f,-0.3f, 0.0f,
		//parte inferior
		0.7f,-0.3f, 0.0f,   0.9f,-0.3f, 0.0f,   0.7f,-0.5f, 0.0f,
		0.9f,-0.3f, 0.0f,   0.9f,-0.5f, 0.0f,   0.7f,-0.5f, 0.0f
	};

	glGenVertexArrays(1, &VAO_Letras);
	glBindVertexArray(VAO_Letras);

	glGenBuffers(1, &VBO_Letras);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_Letras);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_letras), vertices_letras, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void CreateShaders()
{

	Shader *shader1 = new Shader(); //shader para usar índices: objetos: cubo y  pirámide
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1); //[0]

	Shader *shader2 = new Shader();//shader para usar color como parte del VAO: letras 
	shader2->CreateFromFiles(vShaderColor, fShaderColor);
	shaderList.push_back(*shader2); //[1]

	//shaders individuales para cada letra
	Shader* shaderC = new Shader();
	shaderC->CreateFromFiles("shaders/shaderC.vert", "shaders/shaderLetra.frag");
	shaderList.push_back(*shaderC); // [2]

	Shader* shaderR = new Shader();
	shaderR->CreateFromFiles("shaders/shaderR.vert", "shaders/shaderLetra.frag");
	shaderList.push_back(*shaderR); // [3]

	Shader* shaderD = new Shader();
	shaderD->CreateFromFiles("shaders/shaderD.vert", "shaders/shaderLetra.frag");
	shaderList.push_back(*shaderD); // [4]

	//Shaders nuevos para las figuras 3D
	Shader* shaderAzul = new Shader();
	shaderAzul->CreateFromFiles("shaders/shaderAzul.vert", "shaders/shaderLetra.frag");
	shaderList.push_back(*shaderAzul); // [5]

	Shader* shaderVerde = new Shader();
	shaderVerde->CreateFromFiles("shaders/shaderVerde.vert", "shaders/shaderLetra.frag");
	shaderList.push_back(*shaderVerde); // [6]

	Shader* shaderCafe = new Shader();
	shaderCafe->CreateFromFiles("shaders/shaderCafe.vert", "shaders/shaderLetra.frag");
	shaderList.push_back(*shaderCafe); // [7]

	Shader* shaderMagenta = new Shader();
	shaderMagenta->CreateFromFiles("shaders/shaderMagenta.vert", "shaders/shaderLetra.frag");
	shaderList.push_back(*shaderMagenta); // [8]

	Shader* shaderNegro = new Shader();
	shaderNegro->CreateFromFiles("shaders/shaderNegro.vert", "shaders/shaderLetra.frag");
	shaderList.push_back(*shaderNegro); //[9]

	Shader* shaderAmarillo = new Shader();
	shaderAmarillo->CreateFromFiles("shaders/shaderAmarillo.vert", "shaders/shaderLetra.frag");
	shaderList.push_back(*shaderAmarillo); //[10]

	Shader* shaderMorado = new Shader();
	shaderMorado->CreateFromFiles("shaders/shaderMorado.vert", "shaders/shaderLetra.frag");
	shaderList.push_back(*shaderMorado); //[11]
}


int main()
{
	mainWindow = Window(800, 600);
	mainWindow.Initialise();
	CreaPiramide(); //índice 0 en MeshList
	CrearCubo();//índice 1 en MeshList
	CrearPiramideCuadrangular(); //índice 2 en MeshList
	CrearLetrasyFiguras(); //usa MeshColor, índices en MeshColorList
	CrearLetrasCRD();
	CreateShaders();
	GLuint uniformProjection = 0;
	GLuint uniformModel = 0;
	//Projection: Matriz de Dimensión 4x4 para indicar si vemos en 2D( orthogonal) o en 3D) perspectiva
	glm::mat4 projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 100.0f);
	//glm::mat4 projection = glm::perspective(glm::radians(60.0f)	,mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 100.0f);
	
	//Model: Matriz de Dimensión 4x4 en la cual se almacena la multiplicación de las transformaciones geométricas.
	glm::mat4 model(1.0f); //fuera del while se usa para inicializar la matriz con una identidad
	
	//Loop mientras no se cierra la ventana
	while (!mainWindow.getShouldClose())
	{
		//Recibir eventos del usuario
		glfwPollEvents();
		//Limpiar la ventana
		glClearColor(0.75f, 0.75f, 0.75f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shaderList[1].useShader();
		uniformModel = shaderList[1].getModelLocation();
		uniformProjection = shaderList[1].getProjectLocation();
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));

		glEnable(GL_DEPTH_TEST);

		//Activamos los shaders y dibujamos las letras C,R,D
		//Primero dibujamos la letra C (Azul)
		shaderList[2].useShader();
		uniformModel = shaderList[2].getModelLocation();
		uniformProjection = shaderList[2].getProjectLocation();
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.15f, 0.6f, 0.0f));
		model = glm::scale(model, glm::vec3(0.4f, 0.4f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		glBindVertexArray(VAO_Letras);
		glDrawArrays(GL_TRIANGLES, 0, 18); 
		glBindVertexArray(0);

		//Dibujamos la letra R (Rojo)
		shaderList[3].useShader();
		uniformModel = shaderList[3].getModelLocation();
		uniformProjection = shaderList[3].getProjectLocation();
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.15f, 0.6f, 0.0f));
		model = glm::scale(model, glm::vec3(0.4f, 0.4f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		glBindVertexArray(VAO_Letras);
		glDrawArrays(GL_TRIANGLES, 18, 30);
		glBindVertexArray(0);
		
		//Dibujamos la letra D (Verde)
		shaderList[4].useShader();
		uniformModel = shaderList[4].getModelLocation();
		uniformProjection = shaderList[4].getProjectLocation();
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.15f, 0.6f, 0.0f));
		model = glm::scale(model, glm::vec3(0.4f, 0.4f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		glBindVertexArray(VAO_Letras);
		glDrawArrays(GL_TRIANGLES, 48, 24);
		glBindVertexArray(0);

		// Activar shader de colores para las figuras
		shaderList[1].useShader();
		uniformModel = shaderList[1].getModelLocation();
		uniformProjection = shaderList[1].getProjectLocation();
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));

		glEnable(GL_DEPTH_TEST);

		//Ahora usamos las figuras 3D
		// Dibujamos la barra inferior negra ([9])
		shaderList[9].useShader(); 
		uniformModel = shaderList[9].getModelLocation();
		uniformProjection = shaderList[9].getProjectLocation();
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, -0.8f, 0.0f));
		model = glm::scale(model, glm::vec3(1.8f, 0.1f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[1]->RenderMesh(); // El cubo

		//Hacemos el primer conjunto: Escalera con barras y 3 triángulos invertidos

		// Poste Izquierdo Café
		shaderList[7].useShader();
		uniformModel = shaderList[7].getModelLocation();
		uniformProjection = shaderList[7].getProjectLocation();
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-0.83f, -0.20f, 0.0f)); 
		model = glm::scale(model, glm::vec3(0.08f, 1.1f, 0.2f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[1]->RenderMesh();

		// Poste Derecho Café
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-0.53f, -0.20f, 0.0f)); 
		model = glm::scale(model, glm::vec3(0.08f, 1.1f, 0.2f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[1]->RenderMesh(); // Cubo

		// Triángulo Superior (Amarillo - Invertido)
		shaderList[10].useShader();
		uniformModel = shaderList[10].getModelLocation();
		uniformProjection = shaderList[10].getProjectLocation();
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-0.68f, 0.0f, 0.0f)); 
		model = glm::scale(model, glm::vec3(0.30f, 0.3f, 0.3f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[0]->RenderMesh(); // Pirámide triangular

		// Triángulo Central ( Rojo - Invertido)
		shaderList[3].useShader();
		uniformModel = shaderList[3].getModelLocation();
		uniformProjection = shaderList[3].getProjectLocation();
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-0.68f, -0.30f, 0.0f)); 
		model = glm::scale(model, glm::vec3(0.30f, 0.3f, 0.3f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[0]->RenderMesh();

		// Triángulo Inferior (Verde - Invertido)
		shaderList[6].useShader();
		uniformModel = shaderList[6].getModelLocation();
		uniformProjection = shaderList[6].getProjectLocation();
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-0.68f, -0.60f, 0.0f)); 
		model = glm::scale(model, glm::vec3(0.30f, 0.3f, 0.3f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[0]->RenderMesh();

		//Hacemos el segundo conjunto: Cuadrado central con triángulos apuntando hacia afuera y rombos

		// Triángulo Superior Izquierda (Amarillo)
		shaderList[10].useShader();
		uniformModel = shaderList[10].getModelLocation();
		uniformProjection = shaderList[10].getProjectLocation();
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-0.32f, -0.23f, 0.0f)); 
		model = glm::rotate(model, glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.4f, 0.22f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshColorList[0]->RenderMeshColor();

		// Triángulo Superior Derecha (Rojo)
		shaderList[3].useShader();
		uniformModel = shaderList[3].getModelLocation();
		uniformProjection = shaderList[3].getProjectLocation();
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.10f, -0.23f, 0.0f)); 
		model = glm::rotate(model, glm::radians(-45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.4f, 0.22f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshColorList[2]->RenderMeshColor();

		// Triángulo Inferior Derecha (Verde)
		shaderList[6].useShader();
		uniformModel = shaderList[6].getModelLocation();
		uniformProjection = shaderList[6].getProjectLocation();
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.10f, -0.67f, 0.0f)); 
		model = glm::rotate(model, glm::radians(225.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.4f, 0.22f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshColorList[1]->RenderMeshColor();

		// Triángulo Inferior Izquierda (morado)
		shaderList[11].useShader();
		uniformModel = shaderList[11].getModelLocation();
		uniformProjection = shaderList[11].getProjectLocation();
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-0.32f, -0.67f, 0.0f)); 
		model = glm::rotate(model, glm::radians(135.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.4f, 0.22f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshColorList[3]->RenderMeshColor();

		// Rombo Base Azul (Cubo para la base 3D)
		shaderList[5].useShader();
		uniformModel = shaderList[5].getModelLocation();
		uniformProjection = shaderList[5].getProjectLocation();
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-0.11f, -0.45f, 0.0f));
		model = glm::scale(model, glm::vec3(0.4f, 0.4f, 1.0f));
		model = glm::rotate(model, glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[1]->RenderMesh();

		// Rombo Café Interno (Cubo interior)
		shaderList[7].useShader();
		uniformModel = shaderList[7].getModelLocation();
		uniformProjection = shaderList[7].getProjectLocation();
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-0.11f, -0.45f, 0.0f));
		model = glm::scale(model, glm::vec3(0.18f, 0.18f, 1.1f));
		model = glm::rotate(model, glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[1]->RenderMesh();

		//Hacemos el tercer conjunto: Pirámide de triángulos

		// Triángulo Superior (Morado)
		shaderList[11].useShader();
		uniformModel = shaderList[11].getModelLocation();
		uniformProjection = shaderList[11].getProjectLocation();
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.55f, -0.23f, 0.0f)); 
		model = glm::scale(model, glm::vec3(0.35f, 0.35f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[0]->RenderMesh();

		// Triángulo Central (Amarillo Invertido)
		shaderList[10].useShader(); 
		uniformModel = shaderList[10].getModelLocation();
		uniformProjection = shaderList[10].getProjectLocation();
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.55f, -0.58f, 0.0f)); 
		model = glm::scale(model, glm::vec3(0.35f, 0.36f, 1.0f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshColorList[0]->RenderMeshColor();

		// Triángulo Izquierdo (Verde)
		shaderList[6].useShader();
		uniformModel = shaderList[6].getModelLocation();
		uniformProjection = shaderList[6].getProjectLocation();
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.38f, -0.58f, 0.0f)); 
		model = glm::scale(model, glm::vec3(0.35f, 0.35f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[0]->RenderMesh();

		// Triángulo Derecho (Rojo)
		shaderList[3].useShader();
		uniformModel = shaderList[3].getModelLocation();
		uniformProjection = shaderList[3].getProjectLocation();
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.72f, -0.58f, 0.0f)); 
		model = glm::scale(model, glm::vec3(0.35f, 0.35f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshColorList[2]->RenderMeshColor();

		glUseProgram(0);
		mainWindow.swapBuffers();
	}
	return 0;
}
// inicializar matriz: glm::mat4 model(1.0);
// reestablecer matriz: model = glm::mat4(1.0);
//Traslación
//model = glm::translate(model, glm::vec3(0.0f, 0.0f, -5.0f));
//////////////// ROTACIÓN //////////////////
//model = glm::rotate(model, 45 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
////////////////  ESCALA ////////////////
//model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
///////////////////// T+R////////////////
/*model = glm::translate(model, glm::vec3(valor, 0.0f, 0.0f));
model = glm::rotate(model, 45 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
model = glm::rotate(model, glm::radians(angulo), glm::vec3(0.0f, 1.0f, 0.0f));
*/
/////////////R+T//////////
/*model = glm::rotate(model, 45 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
model = glm::translate(model, glm::vec3(valor, 0.0f, 0.0f));
*/