#define _CRT_SECURE_NO_WARNINGS


#include <stdio.h>
#include <glew.h>
#include <freeglut.h>

#include "algebra.h"
#include "shaders.h"
#include "mesh.h"

#include <math.h>
#include <iostream>
#include <fstream>
#include <string>
#include <random>
#include <time.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

int screen_width = 1024;
int screen_height = 768;

const bool render_all = false;

int widthh, heighth, nrChannels;
unsigned char *data = stbi_load("night-sky.jpg", &widthh, &heighth, &nrChannels, 0);

Mesh *meshList = NULL; // Global pointer to linked list of triangle meshes




//Camera set in the init
Camera cam; // Setup the global camera parameters
Vector View;

GLuint shprg; // Current shader program id
GLuint shprg_tex;


const int FIREWORKS_COUNT = 4;

typedef struct _ParticleSystem {
	Mesh *particleList = NULL;
	Vector initialPosition;
	Vector k_a;
	Vector k_d;
	Vector k_s;
	float shininess;
	bool if_color;
	float rad;
	struct _ParticleSystem *next;
}ParticleSystem;

ParticleSystem ps1, ps2, ps3, ps4;

ParticleSystem fireworks[FIREWORKS_COUNT];

			  // Global transform matrices
			  // V is the view transform
			  // P is the projection transform
			  // PV = P * V is the combined view-projection transform
Matrix V, P, PV;

Vector Light, La, Ld, Ls;


int width, height;
//unsigned char* image = SOIL_load_image("night-sky.jpg", &width, &height, 0, SOIL_LOAD_RGB);

void prepareShaderProgram(const char ** vs_src, const char ** fs_src) {
	GLint success = GL_FALSE;

	shprg = glCreateProgram();

	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs, 1, vs_src, NULL);
	glCompileShader(vs);
	glGetShaderiv(vs, GL_COMPILE_STATUS, &success);
	if (!success) printf("Error in vertex shader!\n");
	else printf("Vertex shader compiled successfully!\n");

	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs, 1, fs_src, NULL);
	glCompileShader(fs);
	glGetShaderiv(fs, GL_COMPILE_STATUS, &success);
	if (!success) printf("Error in fragment shader!\n");
	else printf("Fragment shader compiled successfully!\n");

	glAttachShader(shprg, vs);
	glAttachShader(shprg, fs);
	glLinkProgram(shprg);
	GLint isLinked = GL_FALSE;
	glGetProgramiv(shprg, GL_LINK_STATUS, &isLinked);
	if (!isLinked) printf("Link error in shader program!\n");
	else printf("Shader program linked successfully!\n");
}

void prepareShaderProgramTex(const char ** vs_src, const char ** fs_src) {
	GLint success = GL_FALSE;

	shprg_tex = glCreateProgram();

	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs, 1, vs_src, NULL);
	glCompileShader(vs);
	glGetShaderiv(vs, GL_COMPILE_STATUS, &success);
	if (!success) printf("Error in vertex shader!\n");
	else printf("Vertex shader compiled successfully!\n");

	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs, 1, fs_src, NULL);
	glCompileShader(fs);
	glGetShaderiv(fs, GL_COMPILE_STATUS, &success);
	if (!success) printf("Error in fragment shader!\n");
	else printf("Fragment shader compiled successfully!\n");

	glAttachShader(shprg_tex, vs);
	glAttachShader(shprg_tex, fs);
	glLinkProgram(shprg_tex);
	GLint isLinked = GL_FALSE;
	glGetProgramiv(shprg_tex, GL_LINK_STATUS, &isLinked);
	if (!isLinked) printf("Link error in shader program!\n");
	else printf("Shader program linked successfully!\n");
}

void uniformShaderPhong(Matrix M, Matrix new_PV, Matrix MV, Mesh *mesh, GLuint shader) {
	GLint loc_PVp = glGetUniformLocation(shader, "PVM");
	glUniformMatrix4fv(loc_PVp, 1, GL_FALSE, new_PV.e);

	GLint loc_MVp = glGetUniformLocation(shader, "MV");
	glUniformMatrix4fv(loc_MVp, 1, GL_FALSE, MV.e);

	GLint loc_Vwp = glGetUniformLocation(shader, "Vw");
	glUniformMatrix4fv(loc_Vwp, 1, GL_FALSE, V.e);

	GLint loc_Mwp = glGetUniformLocation(shader, "M");
	glUniformMatrix4fv(loc_Mwp, 1, GL_FALSE, M.e);

	GLint loc_Lp = glGetUniformLocation(shader, "L");
	float Lightf[3] = { Light.x, Light.y, Light.z };
	glUniform3fv(loc_Lp, 1, Lightf);

	GLint loc_Lap = glGetUniformLocation(shader, "La");
	float Laf[3] = { La.x, La.y, La.z };
	glUniform3fv(loc_Lap, 1, Laf);

	GLint loc_Ldp = glGetUniformLocation(shader, "Ld");
	float Ldf[3] = { Ld.x, Ld.y, Ld.z };
	glUniform3fv(loc_Ldp, 1, Ldf);

	GLint loc_Lsp = glGetUniformLocation(shader, "Ls");
	float Lsf[3] = { Ls.x, Ls.y, Ls.z };
	glUniform3fv(loc_Lsp, 1, Lsf);

	GLint loc_Viewp = glGetUniformLocation(shader, "View");
	float Viewf[3] = { View.x, View.y, View.z };
	glUniform3fv(loc_Viewp, 1, Viewf);

	float ka_f[3] = { mesh->k_a.x, mesh->k_a.y, mesh->k_a.z };
	float kd_f[3] = { mesh->k_d.x, mesh->k_d.y, mesh->k_d.z };
	float ks_f[3] = { mesh->k_s.x, mesh->k_s.y, mesh->k_s.z };

	GLint loc_kap = glGetUniformLocation(shader, "Ka");
	glUniform3fv(loc_kap, 1, ka_f);

	GLint loc_kdp = glGetUniformLocation(shader, "Kd");
	glUniform3fv(loc_kdp, 1, kd_f);

	GLint loc_ksp = glGetUniformLocation(shader, "Ks");
	glUniform3fv(loc_ksp, 1, ks_f);

	GLint loc_shinp = glGetUniformLocation(shader, "Shin");
	glUniform1f(loc_shinp, mesh->shininess);


}


float rand_FloatRange(float min, float max)
{
	
	//int min = -1;
	//int max = 1;
	float r = (float)rand() / (float)RAND_MAX;
	return min + r * (max - min);
}

void prepareMesh(Mesh *mesh, Vector k_a, Vector k_d, Vector k_s, float shininess, Vector initpos) {
	
	mesh->k_a = k_a;
	mesh->k_d = k_d;
	mesh->k_s = k_s;
	mesh->shininess = shininess;

	mesh->pos = initpos;
	mesh->dir.y = rand_FloatRange(-2.0f, 2.0f);
	mesh->dir.x = rand_FloatRange(-2.0f, 2.0f);
	mesh->dir.z = rand_FloatRange(-2.0f, 2.0f);
	//mesh->life = rand_FloatRange(6.0f, 9.0f);
	mesh->life = 100.0f;


	int sizeVerts = mesh->nv * 3 * sizeof(float);
	int sizeCols = mesh->nv * 3 * sizeof(float);
	int sizeTris = mesh->nt * 3 * sizeof(int);

	// For storage of state and other buffer objects needed for vertex specification
	glGenVertexArrays(1, &mesh->vao);
	glBindVertexArray(mesh->vao);

	// Allocate VBO and load mesh data (vertices and normals)
	glGenBuffers(1, &mesh->vbo);
	glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeVerts + sizeCols, NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeVerts, (void *)mesh->vertices);
	glBufferSubData(GL_ARRAY_BUFFER, sizeVerts, sizeCols, (void *)mesh->vnorms);

	// Allocate index buffer and load mesh indices
	glGenBuffers(1, &mesh->ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeTris, (void *)mesh->triangles, GL_STATIC_DRAW);

	// Define the format of the vertex data
	GLint vPos = glGetAttribLocation(shprg, "vPos");
	glEnableVertexAttribArray(vPos);
	glVertexAttribPointer(vPos, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	// Define the format of the vertex data 
	GLint vNorm = glGetAttribLocation(shprg, "vNorm");
	glEnableVertexAttribArray(vNorm);
	glVertexAttribPointer(vNorm, 3, GL_FLOAT, GL_FALSE, 0, (void *)(mesh->nv * 3 * sizeof(float)));

	glBindVertexArray(0);

}

unsigned int texture;
unsigned int VAO;

int prepareBackgroundMesh() {
	//As a reference for a solution idea from the opengl tutorial was taken
	//https://learnopengl.com/Getting-started/Textures
	unsigned int VBO, EBO;

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	int width, height, nrChannels;
	unsigned char *data = stbi_load("night-sky1.jpg", &width, &height, &nrChannels, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	//glGenerateMipmap(GL_TEXTURE_2D);
	//const char* reason = stbi_failure_reason();
	//printf("%s ", reason);
	stbi_image_free(data);

	float vertices[] = {
		// positions          // colors           // texture coords
		1.0f,  1.0f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
		1.0f, -1.0f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
		-1.0f, -1.0f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
		-1.0f,  1.0f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // top left 
	};
	unsigned int indices[] = {
		0, 1, 3, // first triangle
		1, 2, 3  // second triangle
	};
	
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// texture coord attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);
	return 0;

}

Mesh dummy;

void renderEmpty() {
	Mesh mesh = dummy;
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glEnable(GL_DEPTH_TEST);
	glClear(GL_DEPTH_BUFFER_BIT);
	glDepthFunc(GL_LESS);
	glBindVertexArray(mesh.vao);

	// To accomplish wireframe rendering (can be removed to get filled triangles)

	//Unlock for 1-4, block for vertex and scene

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); 

	// Draw all triangles
	glDrawElements(GL_TRIANGLES, mesh.nt * 3, GL_UNSIGNED_INT, NULL);
	glBindVertexArray(0);
}
void renderBackground() {
	glClear(GL_COLOR_BUFFER_BIT);

	// bind textures on corresponding texture units
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);

	// render container
	glUseProgram(shprg_tex);
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
	// -------------------------------------------------------------------------------

}

void renderMesh(Mesh *mesh, Vector s, Vector r, Vector t) {

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glEnable(GL_DEPTH_TEST);
	glClear(GL_DEPTH_BUFFER_BIT);
	glDepthFunc(GL_LESS);

	// Assignment 1: Apply the transforms from local mesh coordinates to world coordinates here
	// Combine it with the viewing transform that is passed to the shader below

	//Vector s = {1.0, 1.0, 1.0};
	//Vector r = {0.0, -90.0, 0.0};
	//Vector t = {10.0, 0.0, 0.0};

	Matrix M = TransformModelMatrix(s, r, t);
	
	//Unblock for last, scene, and change in function

	Matrix new_PV = MatMatMul(PV, M);
	Matrix MV = MatMatMul(V, M);
	uniformShaderPhong(M, new_PV, MV, mesh, shprg);
	// Pass the viewing transform to the shader
	GLint loc_PV = glGetUniformLocation(shprg, "PV");
	glUniformMatrix4fv(loc_PV, 1, GL_FALSE, new_PV.e);


	// Select current resources 
	glBindVertexArray(mesh->vao);

	// To accomplish wireframe rendering (can be removed to get filled triangles)

	//Unlock for 1-4, block for vertex and scene

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); 

	// Draw all triangles
	glDrawElements(GL_TRIANGLES, mesh->nt * 3, GL_UNSIGNED_INT, NULL);
	glBindVertexArray(0);
}

Vector t;
static void Timer(int value) {

	/*if (l.pos.y > 40.0) {
	l.pos -= Vec3f(0.0f, 1.0f, 0.0f);
	}
	else if(l.pos.y < 40.0){
	l.pos += Vec3f(0.0f, 1.0f, 0.0f);
	}*/
	
	//printf("%f ", t.y);
	glutPostRedisplay();
	glutTimerFunc(40, Timer, 0);
}

double lastTime;

int row;

void initParticleSystem(int i) {
	Mesh * particle = fireworks[i].particleList;
	while (particle != NULL) {
		if (fireworks[i].if_color == false) {
			Vector new_k_a = { rand_FloatRange(0.0f, 0.99f),rand_FloatRange(0.0f, 0.99f),rand_FloatRange(0.0f, 0.99f) };
			Vector new_k_d = { rand_FloatRange(0.0f, 0.99f),rand_FloatRange(0.0f, 0.99f),rand_FloatRange(0.0f, 0.99f) };
			prepareMesh(particle, new_k_a, new_k_d, fireworks[i].k_s, fireworks[i].shininess, fireworks[i].initialPosition);
		}
		else {
			prepareMesh(particle, fireworks[i].k_a, fireworks[i].k_d, fireworks[i].k_s, fireworks[i].shininess, fireworks[i].initialPosition);
		}
		particle = particle->next;
	}

}

void CleanParticleSystem(int i) {
	Mesh * particle = fireworks[i].particleList;
	while (particle != NULL) {
		particle->pos = fireworks[i].initialPosition;
		particle->dir.y = rand_FloatRange(-1.0f, 1.0f);
		particle->dir.x = rand_FloatRange(-1.0f, 1.0f);
		particle->dir.z = rand_FloatRange(-1.0f, 1.0f);
		//mesh->life = rand_FloatRange(6.0f, 9.0f);
		particle->life = 12.0f;
		particle = particle->next;
	}
}


//Not working
bool renderAll(Vector s, Vector r) {
	Mesh *mesh;
	for (int j = 0; j < FIREWORKS_COUNT; j++) {
		mesh = fireworks[j].particleList;

		float inpoxx = fireworks[j].initialPosition.x;
		float inpoxy = fireworks[j].initialPosition.y;
		float inpoxz = fireworks[j].initialPosition.z;
		float rad = fireworks[j].rad;

		//int i = 0;

		double currentTime = glutGet(GLUT_ELAPSED_TIME);
		double delta = (currentTime - lastTime) / 100;
		//printf("%f ", delta);
		bool is_alive = false;
		while (mesh != NULL) {
			mesh->life -= delta;
			if (abs(inpoxx - mesh->pos.x) > rad || abs(inpoxy - mesh->pos.y) > rad || abs(inpoxz - mesh->pos.z) > rad) {
				mesh->life = 0;
			}

			if (mesh->life > 0) {
				float new_y = (float)(log(mesh->pos.x));
				Vector new_t = Add(mesh->pos, mesh->dir);
				mesh->pos = new_t;
				is_alive = true;
				renderMesh(mesh, s, r, new_t);
			}

			mesh = mesh->next;
			//i++;
			lastTime = glutGet(GLUT_ELAPSED_TIME);
			//s = vectorSList[i];
			//r = vectorRList[i];
			//t = vectorTList[i];

		}
		if (is_alive == false) {
			return false;
			renderEmpty();
			
		}
	}

}

void display(void) {
	Mesh *mesh;

	glClear(GL_COLOR_BUFFER_BIT);

	V = TranslationCamera(0, 0, -cam.position.z);
	View = { cam.position.x, cam.position.y, cam.position.z };

	float aspect = 1024.0 / 768.0;
	P = ProjectionPer(cam.fov, aspect, cam.nearPlane, cam.farPlane);
	// This finds the combined view-projection matrix
	PV = MatMatMul(P, V);

	renderBackground();

	// Select the shader program to be used during rendering 
	glUseProgram(shprg);

	// Render all meshes in the scene
	
	Vector s = { 1.0, 1.0, 1.0 };
	Vector r = { 0.0, 0.0, 0.0 };
	Vector mesh_t = { 0.0, 0.0, 0.0 };

	renderMesh(meshList, s, r, mesh_t);
	
	if (render_all == true) {
		bool val = renderAll(s, r);
		if (val == false) {
			for (int k = 0; k > FIREWORKS_COUNT; k++) {
				CleanParticleSystem(k);
			}
			renderAll(s, r);
		}
	}
	else {
			mesh = fireworks[row].particleList;


			float inpoxx = fireworks[row].initialPosition.x;
			float inpoxy = fireworks[row].initialPosition.y;
			float inpoxz = fireworks[row].initialPosition.z;
			float rad = fireworks[row].rad;

			//int i = 0;

			double currentTime = glutGet(GLUT_ELAPSED_TIME);
			double delta = (currentTime - lastTime) / 100;
			//printf("%f ", delta);
			bool is_alive = false;
			while (mesh != NULL) {
				mesh->life -= delta;
				/*if (abs(inpoxx - mesh->pos.x) > rad || abs(inpoxy - mesh->pos.y) > rad || abs(inpoxz - mesh->pos.z) > rad) {
					mesh->life = 0;
				}*/
				if (sqrt(pow((mesh->pos.x - inpoxx), 2) + pow((mesh->pos.y - inpoxy), 2) + pow((mesh->pos.z - inpoxz), 2)) > rad) {
				mesh->life = 0;
				}

				if (mesh->life > 0) {
					float new_y = (float)(log(mesh->pos.x));
					Vector new_t = Add(mesh->pos, mesh->dir);
					mesh->pos = new_t;
					is_alive = true;
					renderMesh(mesh, s, r, new_t);
				}

				mesh = mesh->next;
				//i++;
				lastTime = glutGet(GLUT_ELAPSED_TIME);
				//s = vectorSList[i];
				//r = vectorRList[i];
				//t = vectorTList[i];

			}
			if (is_alive == false) {
				renderEmpty();
				if (row < FIREWORKS_COUNT)
					row++;
				else {
					//renderAll(s, r);
					row = 0;
				}
					//row = 0;
				CleanParticleSystem(row);
			}

	}
	glFlush();
}

void changeSize(int w, int h) {
	screen_width = w;
	screen_height = h;
	glViewport(0, 0, screen_width, screen_height);

}

void keypress(unsigned char key, int x, int y) {
	switch (key) {
	case 'z':
		cam.position.z -= 0.2f;
		break;
	case 'Z':
		cam.position.z += 0.2f;
		break;
	case 'x':
		cam.position.x += 0.2f;
		break;
	case 'X':
		cam.position.x -= 0.2f;
		break;
	case 'c':
		cam.position.y += 0.2f;
		break;
	case 'C':
		cam.position.y -= 0.2f;
		break;
	case 'w':
		cam.rotation.z += 10.0f;
		break;
	case 'W':
		cam.rotation.z -= 10.0f;
		break;
	case 'e':
		cam.rotation.x += 10.0f;
		break;
	case 'E':
		cam.rotation.x -= 10.0f;
		break;
	case 'r':
		cam.rotation.y += 10.0f;
		break;
	case 'R':
		cam.rotation.y -= 10.0f;
		break;
	case 'q':
		glutLeaveMainLoop();
		break;
	}
	glutPostRedisplay();
}



void init(void) {
	t = { 0.0f, 0.0f, 0.0f };
	row = 1;
	cam = { { 0,0,90 },{ 0,0,0 }, 120, 1, 1000 };
	lastTime = glutGet(GLUT_ELAPSED_TIME);
	// Compile and link the given shader program (vertex shader and fragment shader)

	std::ifstream in_vs_ph("vertex_phong.txt");
	std::string contents_vs_ph((std::istreambuf_iterator<char>(in_vs_ph)),
		std::istreambuf_iterator<char>());
	const char *data_vs_ph = contents_vs_ph.c_str();
	const char *vs_name_ph[] = { data_vs_ph };

	std::ifstream in_fs_ph("frag_phong.txt");
	std::string contents_fs_ph((std::istreambuf_iterator<char>(in_fs_ph)),
		std::istreambuf_iterator<char>());
	const char *data_fs_ph = contents_fs_ph.c_str();
	const char *fs_name_ph[] = { data_fs_ph };

	std::ifstream in_vs_tx("vertex_tex.txt");
	std::string contents_vs_tx((std::istreambuf_iterator<char>(in_vs_tx)),
		std::istreambuf_iterator<char>());
	const char *data_vs_tx = contents_vs_tx.c_str();
	const char *vs_name_tx[] = { data_vs_tx };

	std::ifstream in_fs_tx("frag_tex.txt");
	std::string contents_fs_tx((std::istreambuf_iterator<char>(in_fs_tx)),
		std::istreambuf_iterator<char>());
	const char *data_fs_tx = contents_fs_tx.c_str();
	const char *fs_name_tx[] = { data_fs_tx };

	prepareShaderProgram(vs_name_ph, fs_name_ph);
	prepareShaderProgramTex(vs_name_tx, fs_name_tx);
	//prepareShaderProgram(vs_n2c_src, fs_ci_src);

	// Setup OpenGL buffers for rendering of the meshes

	Vector k_a =  { 0.05375f, 0.05f, 0.06625f };
	Vector k_d =  { 0.5f, 0.22525f, 0.22525f };
	Vector k_s = { 0.5f, 0.22525f, 0.22525f };
	float shininess =  { 15.4f };

	Light = { 60.0f, 80.0f, 10.0f };
	La = { 0.2f, 0.2f, 0.2f };
	Ld = { 1.0f, 1.0f, 1.0f };
	Ls = { 1.0f, 1.0f, 1.0f };

	
	for(int i =0; i<FIREWORKS_COUNT; i++) {
		initParticleSystem(i);
	}

	prepareBackgroundMesh();

	Mesh * mesh = meshList;
	while (mesh != NULL) {
		
		mesh = mesh->next;
	}
}

void cleanUp(void) {
	printf("Running cleanUp function... ");
	// Free openGL resources
	// ...

	// Free meshes
	// ...
	printf("Done!\n\n");
}

// Include data for some triangle meshes (hard coded in struct variables)
#include "./models/mesh_bunny.h"
#include "./models/mesh_particle.h"
#include "./models/mesh_cow.h"
#include "./models/mesh_cube.h"
#include "./models/mesh_frog.h"
#include "./models/mesh_knot.h"
#include "./models/mesh_sphere.h"
#include "./models/mesh_teapot.h"
#include "./models/mesh_triceratops.h"



int main(int argc, char **argv) {

	// Setup freeGLUT	
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(screen_width, screen_height);
	glutCreateWindow("DVA338 Programming Assignments");
	glutDisplayFunc(display);
	glutReshapeFunc(changeSize);
	glutKeyboardFunc(keypress);
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);

	// Specify your preferred OpenGL version and profile
	glutInitContextVersion(4, 5);
	//glutInitContextProfile(GLUT_COMPATIBILITY_PROFILE);	
	glutInitContextProfile(GLUT_CORE_PROFILE);

	// Uses GLEW as OpenGL Loading Library to get access to modern core features as well as extensions
	GLenum err = glewInit();
	if (GLEW_OK != err) { fprintf(stdout, "Error: %s\n", glewGetErrorString(err)); return 1; }

	// Output OpenGL version info
	fprintf(stdout, "GLEW version: %s\n", glewGetString(GLEW_VERSION));
	fprintf(stdout, "OpenGL version: %s\n", (const char *)glGetString(GL_VERSION));
	fprintf(stdout, "OpenGL vendor: %s\n\n", glGetString(GL_VENDOR));


	// Insert the 3D models you want in your scene here in a linked list of meshes
	// Note that "meshList" is a pointer to the first mesh and new meshes are added to the front of the list	
	//insertModel(&meshList, cow.nov, cow.verts, cow.nof, cow.faces, 20.0);
	//insertModel(&meshList, cow.nov, cow.verts, cow.nof, cow.faces, 20.0);
	//insertModel(&meshList, triceratops.nov, triceratops.verts, triceratops.nof, triceratops.faces, 3.0);
	//insertModel(&meshList, bunny.nov, bunny.verts, bunny.nof, bunny.faces, 60.0);	
	//insertModel(&meshList, frog.nov, frog.verts, frog.nof, frog.faces, 2.5);
	//insertModel(&meshList, knot.nov, knot.verts, knot.nof, knot.faces, 1.0);
	//insertModel(&meshList, sphere.nov, sphere.verts, sphere.nof, sphere.faces, 12.0);
	//insertModel(&meshList, teapot.nov, teapot.verts, teapot.nof, teapot.faces, 3.0);

	
	for(int i = 0; i < 1500; i++)
		insertModel(&ps1.particleList, particle.nov, particle.verts, particle.nof, particle.faces, 0.3);

	for (int i = 0; i < 3000; i++)
		insertModel(&ps2.particleList, particle.nov, particle.verts, particle.nof, particle.faces, 0.2);

	for (int i = 0; i < 2000; i++)
		insertModel(&ps3.particleList, particle.nov, particle.verts, particle.nof, particle.faces, 0.3);

	for (int i = 0; i < 3000; i++)
		insertModel(&ps4.particleList, particle.nov, particle.verts, particle.nof, particle.faces, 0.2);

	fireworks[0] = ps1;
	fireworks[0].k_a = { 0.05375f, 0.05f, 0.06625f };
	fireworks[0].k_d = { 0.5f, 0.22525f, 0.22525f };
	fireworks[0].k_s = { 0.5f, 0.22525f, 0.22525f };
	fireworks[0].shininess = { 40.4f };
	fireworks[0].if_color = false;
	fireworks[0].rad = 15.0f;
	fireworks[0].initialPosition = Vector({ -30.0f, 0.0f, 0.0f });

	fireworks[1] = ps2;
	fireworks[1].k_a = { 0.05375f, 0.05f, 0.06625f };
	fireworks[1].k_d = { 0.5f, 0.22525f, 0.22525f };
	fireworks[1].k_s = { 0.5f, 0.22525f, 0.22525f };
	fireworks[1].shininess = { 15.4f };
	fireworks[1].if_color = true;
	fireworks[1].rad = 20.0f;
	fireworks[1].initialPosition = Vector({ 30.0f, 40.0f, 0.0f });

	fireworks[2] = ps3;
	fireworks[2].k_a = { 0.05375f, 0.05f, 0.06625f };
	fireworks[2].k_d = { 0.34f, 0.66642525f, 0.742525f };
	fireworks[2].k_s = { 0.5f, 0.22525f, 0.22525f };
	fireworks[2].shininess = { 12.4f };
	fireworks[2].if_color = true;
	fireworks[2].rad = 30.0f;
	fireworks[2].initialPosition = Vector({ -70.0f, 50.0f, 0.0f });

	fireworks[3] = ps4;
	fireworks[3].k_a = { 0.05375f, 0.05f, 0.06625f };
	fireworks[3].k_d = { 0.5f, 0.22525f, 0.22525f };
	fireworks[3].k_s = { 0.5f, 0.22525f, 0.22525f };
	fireworks[3].shininess = { 45.4f };
	fireworks[3].if_color = false;
	fireworks[3].rad = 50.0f;
	fireworks[3].initialPosition = Vector({ 10.0f, 60.0f, 0.0f });

	insertModel(&meshList, cube.nov, cube.verts, cube.nof, cube.faces, 1.0);
	meshList[0].id = 0;
	//insertModel(&meshList, particle.nov, particle.verts, particle.nof, particle.faces, 0.5);
	dummy = meshList[0];
	


	init();
	Timer(0);
	glutMainLoop();

	cleanUp();
	return 0;
}
