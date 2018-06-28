
#include <GL\glew.h>
#include <glm\gtc\type_ptr.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <cstdio>
#include <cassert>
#include <SDL2\SDL.h>
#include <iostream>

#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include "GL_framework.h"

using namespace std;

#define PI 3.14f
#define F 0.01f

int EXERCISE = 0;
int CAMERA_TYPE = 0;
int TOON_SHADER = 0;
double currentTime = (double)currentTime / 1000.0f;
float time_T = 0.0f;

using namespace glm;

extern bool loadOBJ(const char * path,
	std::vector < glm::vec3 > &
	out_vertices,
	std::vector < glm::vec2 > & out_uvs,
	std::vector < glm::vec3 > & out_normals
);

std::vector< glm::vec3 > vertices;
std::vector< glm::vec2 > uvs;
std::vector< glm::vec3 > normals;

std::vector< glm::vec3 > verticesCabins;
std::vector< glm::vec2 > uvsCabins;
std::vector< glm::vec3 > normalsCabins;

std::vector< glm::vec3 > verticesWheel_Feet;
std::vector< glm::vec2 > uvsWheel_Feet;
std::vector< glm::vec3 > normalsWheel_Feet;

std::vector< glm::vec3 > verticesTrump;
std::vector< glm::vec2 > uvsTrump;
std::vector< glm::vec3 > normalsTrump;

std::vector< glm::vec3 > verticesChicken;
std::vector< glm::vec2 > uvsChicken;
std::vector< glm::vec3 > normalsChicken;

std::vector< glm::vec3 > verticesCabeza;
std::vector< glm::vec2 > uvsCabeza;
std::vector< glm::vec3 > normalsCabeza;

std::vector< glm::vec3 > vertices5;
std::vector< glm::vec2 > uvs5;
std::vector< glm::vec3 > normals5;

namespace ImGui {
	void Render();
}
namespace Box {
	void setupCube();
	void cleanupCube();
	void drawCube();
}
namespace Axis {
	void setupAxis();
	void cleanupAxis();
	void drawAxis();
}
namespace Cube {
	void setupCube();
	void cleanupCube();
	void updateCube(const glm::mat4& transform);
	void drawCube();
}

namespace Cabins {
	void setupModel();
	void cleanupModel();
	void updateModel(const glm::mat4& transform);
	void drawModel();
}

namespace Wheel {
	void setupModel();
	void cleanupModel();
	void updateModel(const glm::mat4& transform);
	void drawModel();
}

namespace Wheel_Feet {
	void setupModel();
	void cleanupModel();
	void updateModel(const glm::mat4& transform);
	void drawModel();
}

namespace Trump {
	void setupModel();
	void cleanupModel();
	void updateModel(const glm::mat4& transform);
	void drawModel();
}

namespace Chicken {
	void setupModel();
	void cleanupModel();
	void updateModel(const glm::mat4& transform);
	void drawModel();
}

namespace Cabeza_Trump {
	void setupModel();
	void cleanupModel();
	void updateModel(const glm::mat4& transform);
	void drawModel();
}


namespace Sphere {
	void setupSphere(glm::vec3 pos, float radius);
	void cleanupSphere();
	void updateSphere(glm::vec3 pos, float radius);
	void drawSphere();
}

namespace RenderVars {

	float FOV = glm::radians(65.f);
	const float zNear = 0.1f;
	const float zFar = 2000.0f;

	glm::mat4 _projection;
	glm::mat4 _modelView;
	glm::mat4 _MVP;
	glm::mat4 _inv_modelview;
	glm::vec4 _cameraPoint;

	struct prevMouse {
		float lastx, lasty;
		MouseEvent::Button button = MouseEvent::Button::None;
		bool waspressed = false;
	} prevMouse;

	float panv[3] = { 0.f, 0.f, -25.f };
	float rota[2] = { 0.f, 0.f };
}

namespace RV = RenderVars;

vec3 lightPosition;
vec3 lightPosition2;
vec3 Color;
vec3 Color2;
vec3 Original_Color;
vec3 moonPosition;
float ToonShading = 0.0f;
float ToonShading2 = 0.0f;
float ToonShadingLEVEL = 0.0f;

void GLResize(int width, int height) {

	glViewport(0, 0, width, height);
	if (height != 0) RV::_projection = glm::perspective(RV::FOV, (float)width / (float)height, RV::zNear, RV::zFar);
	else RV::_projection = glm::perspective(RV::FOV, 0.f, RV::zNear, RV::zFar);

}

void GLmousecb(MouseEvent ev) {

	if (RV::prevMouse.waspressed && RV::prevMouse.button == ev.button) {

		float diffx = ev.posx - RV::prevMouse.lastx;
		float diffy = ev.posy - RV::prevMouse.lasty;

		switch (ev.button) {

		case MouseEvent::Button::Left: // ROTATE
			RV::rota[0] += diffx * 0.005f;
			RV::rota[1] += diffy * 0.005f;
			break;
		case MouseEvent::Button::Right: // MOVE XY
			RV::panv[0] += diffx * 0.03f;
			RV::panv[1] -= diffy * 0.03f;
			break;
		case MouseEvent::Button::Middle: // MOVE Z
			RV::panv[2] += diffy * 0.05f;
			break;
		default: break;

		}

	}

	else {
		RV::prevMouse.button = ev.button;
		RV::prevMouse.waspressed = true;
	}

	RV::prevMouse.lastx = ev.posx;
	RV::prevMouse.lasty = ev.posy;
}

vec3 positionWheel(float r, int N, int i) {

	vec3 position;
	double currentTime = (double)SDL_GetTicks() / 1000.0;
	position.x = (r*cos(2 * PI*F*(float)currentTime + 2 * PI*i / N));
	position.y = (r*sin(2 * PI*F*(float)currentTime + 2 * PI*i / N));
	position.z = 0;

	return position;
}

void DefaultCamera() {
	RV::_modelView = glm::mat4(1.f);
	RV::_modelView = glm::translate(RV::_modelView, glm::vec3(RV::panv[0], RV::panv[1], RV::panv[2]));
	RV::_modelView = glm::rotate(RV::_modelView, RV::rota[1], glm::vec3(1.f, 0.f, 0.f));
	RV::_modelView = glm::rotate(RV::_modelView, RV::rota[0], glm::vec3(0.f, 1.f, 0.f));
}

void ReverseShot(vec3 obj, vec3 separation, double currentTime, float time, bool trump) {

	RV::_modelView = glm::mat4(1.f);

	//if (currentTime > time_T) {
	//	time = currentTime + 2.0f;
	//	trump != trump;
	//}

	RV::_modelView = rotate(RV::_modelView, radians(90.0f), vec3(0, 1, 0));
	RV::_modelView = glm::translate(RV::_modelView, obj + separation + vec3(1.3f, 1.3, 0.4));

}

void WheelAxis() {
	RV::_modelView = glm::mat4(1.f);
	RV::_modelView = glm::translate(RV::_modelView, vec3(0, -1, -19));
}

void Zenith(vec3 obj) {
	RV::_modelView = glm::mat4(1.f);
	RV::_modelView = rotate(RV::_modelView, radians(-90.0f), vec3(1, 0, 0));
	RV::_modelView = glm::translate(RV::_modelView, obj - vec3(0, 7, 0));
}

//DIFFICULT WIHT ZNEAR
void ChickenDolly(vec3 obj) {
	RV::_modelView = glm::mat4(1.f);
	RV::_modelView = rotate(RV::_modelView, radians(90.0f), vec3(0, 1, 0));
	RV::_modelView = glm::translate(RV::_modelView, obj + vec3(1, 4, 0));
}

void Trump_FOV(vec3 obj, double currentTime, float time) {

	RV::_modelView = glm::mat4(1.f);
	RV::_modelView = rotate(RV::_modelView, radians(-90.0f), vec3(0, 1, 0));
	RV::_modelView = glm::translate(RV::_modelView, obj + vec3(-1.5f, 1, 0));

	if (time >= 0.0f) {

		RV::FOV += radians(1.0f);
		RV::_projection = glm::perspective(RV::FOV, (float)1280 / (float)720, RV::zNear, RV::zFar);
		time -= (float)currentTime;

	}
	else {

		RV::FOV = radians(65.0f);
		//RV::_projection = glm::perspective(RV::FOV, (float)1280 / (float)720, RV::zNear, RV::zFar);
		time = 5.0f;

	}

}

void InverseDolly(vec3 obj) {
	RV::_modelView = glm::mat4(1.f);
	float increment = 0.2f;
	RV::_modelView = rotate(RV::_modelView, radians(90.0f), vec3(0, 1, 0));
	RV::_modelView = glm::translate(RV::_modelView, obj + vec3(1, 4, 0));
	RV::_modelView = glm::translate(RV::_modelView, obj + vec3(0, 0, currentTime));
	RV::FOV += currentTime;
}

//void MODEL_LOADING(double currentTime) {
//
//	//Codigo para seguir el movimiento circular, de un solo objeto.
//	//2.0f * PI * 0.01f * (float)currentTime
//	//VARIABLES
//	float cabins_N = 11;
//	float trump_N = 12;
//	float chicken_N = 10;
//	const int N = 20;
//
//	//CODE
//	vec3 cubes[N];
//
//	for (int i = 0; i < N; i++) {
//
//		//POSITIONS
//		cubes[i] = positionWheel(cabins_N, N, i);
//
//		//DRAW
//		Cube::drawCube();
//		Trump::drawCube();
//		Chicken::drawCube();
//
//		mat4 cabins(1.0f), trumps(1.0f), chickens(1.0f);
//
//		cabins = translate(cabins, cubes[i]);
//		trumps = translate(trumps, cubes[i] + vec3(-0.4, -1, 0));
//		chickens = translate(chickens, cubes[i] + vec3(0.4, -1, 0));
//
//		trumps = rotate(trumps, radians(90.0f), vec3(0, 1, 0));
//		chickens = rotate(chickens, radians(-90.0f), vec3(0, 1, 0));
//
//		cabins = scale(cabins, vec3(0.009f, 0.009f, 0.009f));
//		trumps = scale(trumps, vec3(0.007f, 0.007f, 0.007f));
//		chickens = scale(chickens, vec3(0.012f, 0.012f, 0.012f));
//
//		Cube::updateCube(cabins);
//		Trump::updateCube(trumps);
//		Chicken::updateCube(chickens);
//
//	}
//
//	vec3 trumpSepartion = vec3(-0.4, -1, 0);
//	vec3 chickenSeparation = vec3(0.4, -1, 0);
//
//	//CAMERA
//	switch (CAMERA_TYPE) {
//		case 0: DefaultCamera();
//			break;
//		case 1: ReverseShot(cubes[0], trumpSepartion,currentTime);
//			break;
//		case 2: WheelAxis();
//			break;
//		case 3: Zenith(cubes[0]);
//			break;
//		case 4: ChickenDolly(cubes[0]);
//			break;
//		case 5: Trump_FOV(cubes[0],currentTime,5.0f);
//			break;
//	}
//
//	RV::_MVP = RV::_projection * RV::_modelView;
//
//	mat4 wheel(1.0f), wheel_feet(1.0f);
//
//	wheel = scale(wheel, vec3(0.002f, 0.002f, 0.002f));
//	quat wheel_aux = rotate(glm::quat(1.0f, 0, 0, 0), 2.0f * PI * F * (float)currentTime, vec3(0, 0, 1));
//	wheel *= toMat4(wheel_aux);
//	wheel_feet = scale(wheel_feet, vec3(0.002f, 0.002f, 0.002f));
//
//	Wheel::drawCube();	
//	Wheel_Feet::drawCube();
//
//	Wheel::updateCube(wheel);
//	Wheel_Feet::updateCube(wheel_feet);
//
//}

float aPendulo = 0.0f; float iPendulo = 0.0f;;

float angle_ = PI / 4;
float aAc = 0.0f;

void Model_Exercise(double currentTime) {

	const int CABINS = 20;
	vec3 cabins[CABINS];

	vec3 trumpSepartion = vec3(-0.4, -1, 0);
	vec3 chickenSeparation = vec3(0.4, -1, 0);

	for (int i = 0; i < CABINS; i++)
	{
		cabins[i] = positionWheel(16.5f, CABINS, i);

		Cabins::drawModel();
		Trump::drawModel();
		Chicken::drawModel();

		mat4 cabins_mat(1.f), trump_mat(1.f), chicken_mat(1.f);

		cabins_mat = translate(cabins_mat, cabins[i]);
		cabins_mat = scale(cabins_mat, vec3(0.01, 0.01, 0.01));

		trump_mat = translate(trump_mat, cabins[i] + trumpSepartion);
		trump_mat = rotate(trump_mat, radians(90.0f), vec3(0, 1, 0));
		trump_mat = scale(trump_mat, vec3(0.007, 0.007, 0.007));

		chicken_mat = translate(chicken_mat, cabins[i] + chickenSeparation);
		chicken_mat = rotate(chicken_mat, radians(-90.0f), vec3(0, 1, 0));
		chicken_mat = scale(chicken_mat, vec3(0.01, 0.01, 0.01));

		Cabins::updateModel(cabins_mat);
		Trump::updateModel(trump_mat);
		Chicken::updateModel(chicken_mat);

	}

	//Box::drawCube();
	Axis::drawAxis();

	switch (CAMERA_TYPE) {
	case 0: DefaultCamera();
		break;
	case 1:
		ReverseShot(cabins[0], trumpSepartion, currentTime, time_T, false);
		break;
	case 2: WheelAxis();
		break;
	case 3: Zenith(cabins[0]);
		break;
	case 4: ChickenDolly(cabins[0]);
		break;
	case 5: Trump_FOV(cabins[0], currentTime, 5.0f);
		break;
	case 6: InverseDolly(cabins[0]);
	}

	RV::_MVP = RV::_projection * RV::_modelView;


	switch (TOON_SHADER) {
	case 0:
		break;
	}



	//Rueda
	mat4 rueda(1.0f);
	rueda = scale(rueda, vec3(0.003f, 0.003f, 0.003f));
	quat wheel_aux = rotate(glm::quat(1.0f, 0, 0, 0), 2.0f * PI * F *
		(float)currentTime, vec3(0, 0, 1));
	rueda *= toMat4(wheel_aux);
	Wheel::updateModel(rueda);

	//Pie de la rueda
	mat4 rueda_Pie(1.0f);
	rueda_Pie = scale(rueda_Pie, vec3(0.003f, 0.003f, 0.003f));
	Wheel_Feet::updateModel(rueda_Pie);

	//Dibujar modelos
	Wheel::drawModel();
	Wheel_Feet::drawModel();

	//Luces
	lightPosition = vec3(0, 40 * sin((float)currentTime), 40 * cos((float)currentTime));
	moonPosition = rotateZ(vec3(lightPosition.x, lightPosition.y, -lightPosition.z), radians(135.f));
	float a1 = atan(lightPosition.y / lightPosition.x);
	//Fp = cross(Fg,sin angulo);
	//a = Fg * sin angulo / masa;

	angle_ += aAc;
	aAc *= 0.99;
	lightPosition2.x = 10.0f * sin(angle_);
	lightPosition2.y = 10.0f * cos(angle_);


	//Movimiento de las luces
	if (sin(a1) > 0.0f) {
		Color.x += 0.2f;
		Color.y += 0.2f;
	}
	else if (cos(a1) < 0.0f) {
		Color.y -= 0.8f;
		Color.x += 0.2f;
	}

	if (sin(a1) < 0.0f) {
		Color = vec3(0.0f, 0.0f, 0.0f);
	}
	else if (sin(a1) < 0.1f) {
		Color.x += 0.3f;
	}

	Sphere::updateSphere(lightPosition, 2.0f);
	Sphere::drawSphere();

	Sphere::updateSphere(moonPosition, 0.5f);
	Sphere::drawSphere();

	Sphere::updateSphere(lightPosition2, 4.f);
	Sphere::drawSphere();

}

void Cubes_Wheel() {

	DefaultCamera();

	RV::_MVP = RV::_projection * RV::_modelView;

	const int CUBES = 20;
	vec3 cubes[CUBES];

	for (int i = 0; i < CUBES; i++)
	{
		cubes[i] = positionWheel(20.0f, CUBES, i);

		Cube::drawCube();

		mat4 cubes_mat(1.f);

		cubes_mat = translate(cubes_mat, cubes[i]);

		Cube::updateCube(cubes_mat);

	}

}

void K_Simple() {

	DefaultCamera();
	RV::_MVP = RV::_projection * RV::_modelView;

	const int filas = 100;
	const int columnas = 100;
	const int OBJ_TOTAL = filas * columnas;
	vec3 cabins[OBJ_TOTAL];

	vec3 trumpSepartion = vec3(-0.4, -1, 0);
	vec3 chickenSeparation = vec3(0.4, -1, 0);

	int counter = 0;
	float space = 5.f;

	vec3 distancia = vec3(5.0f, 0.0f, 0.0f);

	for (int i = 0; i < filas; i++) {

		for (int j = 0; j < columnas; j++) {

			Cabins::drawModel();

			mat4 cabins_mat(1.f);

			cabins_mat = translate(cabins_mat, vec3(i*5.0f, 0, j*5.0f));
			cabins_mat = scale(cabins_mat, vec3(0.01, 0.01, 0.01));

			Cabins::updateModel(cabins_mat);

		}
	}
}




//Start
void myInitCode(int width, int height) {

	glViewport(0, 0, width, height);
	glClearColor(0.2f, 0.2f, 0.2f, 1.f);

	glDepthFunc(GL_LEQUAL);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glClearDepth(1.f);
	RV::_projection = glm::perspective(RV::FOV, (float)width / (float)height, RV::zNear, RV::zFar);

	Original_Color = vec3(1.f, 1.f, 0.0f);

	Color = Original_Color;

	Color2 = vec3(0.7f, 1.f, 0.7f);

	//Box::setupCube();
	//Axis::setupAxis();

	//Models
	bool Cabins = loadOBJ("Cabina.obj",
		verticesCabins, uvsCabins, normalsCabins);

	bool wheel = loadOBJ("Rueda.obj",
		vertices, uvs, normals);

	bool Wheel_Feet_ = loadOBJ("Rueda_Pie.obj",
		verticesWheel_Feet, uvsWheel_Feet, normalsWheel_Feet);

	bool trump = loadOBJ("Trump_.obj",
		verticesTrump, uvsTrump, normalsTrump);

	bool chicken = loadOBJ("Gallina.obj",
		verticesChicken, uvsChicken, normalsChicken);

	bool Cabeza_Trump = loadOBJ("Cabeza_Trump.obj",
		verticesCabeza, uvsCabeza, normalsCabeza);


	Wheel::setupModel();
	Wheel_Feet::setupModel();
	Cabins::setupModel();
	Trump::setupModel();
	Chicken::setupModel();
	Cube::setupCube();
	Cabeza_Trump::setupModel();



	lightPosition = glm::vec3(0.0f, 20, 80);
	lightPosition2 = glm::vec3(10, 10, 30);
	Color = vec3(1.f, 1.f, 0.0f);
	moonPosition = glm::vec3(0.0f, 20, 80);

	Sphere::setupSphere(lightPosition, 2.0f);
	Sphere::setupSphere(moonPosition, 0.5f);
	Sphere::setupSphere(lightPosition2, 4.0f);

}


//Update
void myRenderCode(double currentTime) {

	//glEnable(GL_DEPTH_TEST);

	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	/*
	//VARIABLES
	float cabins = 11;
	float trump = 12;
	float chicken = 10;
	const int N = 20;

	//CODE

	vec3 cubes[N];
	vec3 trumps[N];
	vec3 chickens[N];

	for (int i = 0; i < N; i++) {

	//POSITIONS
	cubes[i] = positionWheel(cabins, N, i);

	//DRAW
	Cube::drawCube();
	Trump::drawCube();
	Chicken::drawCube();

	mat4 obj(1.0f), obj2(1.0f), obj3(1.0f);

	obj  = translate(obj, cubes[i]);
	obj2 = translate(obj2, cubes[i]+vec3(-0.4,-1,0));
	obj3 = translate(obj3, cubes[i]+vec3(0.4,-1,0));

	obj2 = rotate(obj2, radians(90.0f), vec3(0, 1, 0));
	obj3 = rotate(obj3, radians(-90.0f), vec3(0, 1, 0));

	obj  = scale(obj, vec3(0.009f, 0.009f, 0.009f));
	obj2 = scale(obj2, vec3(0.007f, 0.007f, 0.007f));
	obj3 = scale(obj3, vec3(0.012f, 0.012f, 0.012f));

	Cube::updateCube(obj);
	Trump::updateCube(obj2);
	Chicken::updateCube(obj3);

	}

	mat4 wheel(1.0f), wheel_feet(1.0f);

	wheel = scale(wheel, vec3(0.002f, 0.002f, 0.002f));
	wheel_feet = scale(wheel_feet, vec3(0.002f, 0.002f, 0.002f));

	quat wheel_aux = rotate(glm::quat(1.0f, 0, 0, 0), 2.0f * PI * 0.01f * (float)currentTime, vec3(0, 0, 1));
	wheel *= toMat4(wheel_aux);

	Wheel::drawCube();	Wheel_Feet::drawCube();

	Wheel::updateCube(wheel);
	Wheel_Feet::updateCube(wheel_feet);
	*/

	switch (EXERCISE) {
	case 0: Cubes_Wheel();
		break;
	case 1: Model_Exercise(currentTime);
		break;
	case 2: K_Simple();
		break;
	}


	//Cabeza_Trump::drawModel();
	//mat4 scaleTrump(1.f);
	//scaleTrump = scale(scaleTrump, vec3(0.2f, 0.2f, 0.2f));
	//Cabeza_Trump::updateModel(scaleTrump);

	for (int i = 0; i < verticesCabeza.size(); i++) {
		Cube::drawCube();
		mat4 trumpCube(1.f);

		trumpCube = translate(trumpCube, vec3(verticesCabeza[i] / 8.0f - vec3(0, 0, +15)));
		trumpCube = scale(trumpCube, vec3(0.3, 0.3, 0.2));
		Cube::updateCube(trumpCube);
	}

	ImGui::Render();

}

//Cleanup
void myCleanupCode() {

	Box::cleanupCube();
	Axis::cleanupAxis();
	//Cube::cleanupCube();

}

GLuint compileShader(const char* shaderStr, GLenum shaderType, const char* name = "") {
	GLuint shader = glCreateShader(shaderType);
	glShaderSource(shader, 1, &shaderStr, NULL);
	glCompileShader(shader);
	GLint res;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &res);
	if (res == GL_FALSE) {
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &res);
		char *buff = new char[res];
		glGetShaderInfoLog(shader, res, &res, buff);
		fprintf(stderr, "Error Shader %s: %s", name, buff);
		delete[] buff;
		glDeleteShader(shader);
		return 0;
	}
	return shader;
}
void linkProgram(GLuint program) {
	glLinkProgram(program);
	GLint res;
	glGetProgramiv(program, GL_LINK_STATUS, &res);
	if (res == GL_FALSE) {
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &res);
		char *buff = new char[res];
		glGetProgramInfoLog(program, res, &res, buff);
		fprintf(stderr, "Error Link: %s", buff);
		delete[] buff;
	}
}

namespace Box {
	GLuint cubeVao;
	GLuint cubeVbo[2];
	GLuint cubeShaders[2];
	GLuint cubeProgram;

	float cubeVerts[] = {
		// -5,0,-5 -- 5, 10, 5
		-5.f,  0.f, -5.f,
		5.f,  0.f, -5.f,
		5.f,  0.f,  5.f,
		-5.f,  0.f,  5.f,
		-5.f, 10.f, -5.f,
		5.f, 10.f, -5.f,
		5.f, 10.f,  5.f,
		-5.f, 10.f,  5.f,
	};
	GLubyte cubeIdx[] = {
		1, 0, 2, 3, // Floor - TriangleStrip
		0, 1, 5, 4, // Wall - Lines
		1, 2, 6, 5, // Wall - Lines
		2, 3, 7, 6, // Wall - Lines
		3, 0, 4, 7  // Wall - Lines
	};

	const char* vertShader_xform =
		"#version 330\n\
	in vec3 in_Position;\n\
	uniform mat4 mvpMat;\n\
	void main() {\n\
		gl_Position = mvpMat * vec4(in_Position, 1.0);\n\
	}";
	const char* fragShader_flatColor =
		"#version 330\n\
out vec4 out_Color;\n\
uniform vec4 color;\n\
void main() {\n\
	out_Color = color;\n\
}";

	void setupCube() {
		glGenVertexArrays(1, &cubeVao);
		glBindVertexArray(cubeVao);
		glGenBuffers(2, cubeVbo);

		glBindBuffer(GL_ARRAY_BUFFER, cubeVbo[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 24, cubeVerts, GL_STATIC_DRAW);
		glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeVbo[1]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLubyte) * 20, cubeIdx, GL_STATIC_DRAW);

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		cubeShaders[0] = compileShader(vertShader_xform, GL_VERTEX_SHADER, "cubeVert");
		cubeShaders[1] = compileShader(fragShader_flatColor, GL_FRAGMENT_SHADER, "cubeFrag");

		cubeProgram = glCreateProgram();
		glAttachShader(cubeProgram, cubeShaders[0]);
		glAttachShader(cubeProgram, cubeShaders[1]);
		glBindAttribLocation(cubeProgram, 0, "in_Position");
		linkProgram(cubeProgram);
	}
	void cleanupCube() {
		glDeleteBuffers(2, cubeVbo);
		glDeleteVertexArrays(1, &cubeVao);

		glDeleteProgram(cubeProgram);
		glDeleteShader(cubeShaders[0]);
		glDeleteShader(cubeShaders[1]);
	}
	void drawCube() {
		glBindVertexArray(cubeVao);
		glUseProgram(cubeProgram);
		glUniformMatrix4fv(glGetUniformLocation(cubeProgram, "mvpMat"), 1, GL_FALSE, glm::value_ptr(RV::_MVP));
		// FLOOR
		glUniform4f(glGetUniformLocation(cubeProgram, "color"), 0.6f, 0.6f, 0.6f, 1.f);
		glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_BYTE, 0);
		// WALLS
		glUniform4f(glGetUniformLocation(cubeProgram, "color"), 0.f, 0.f, 0.f, 1.f);
		glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_BYTE, (void*)(sizeof(GLubyte) * 4));
		glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_BYTE, (void*)(sizeof(GLubyte) * 8));
		glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_BYTE, (void*)(sizeof(GLubyte) * 12));
		glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_BYTE, (void*)(sizeof(GLubyte) * 16));

		glUseProgram(0);
		glBindVertexArray(0);
	}
}

namespace Axis {
	GLuint AxisVao;
	GLuint AxisVbo[3];
	GLuint AxisShader[2];
	GLuint AxisProgram;

	float AxisVerts[] = {
		0.0, 0.0, 0.0,
		1.0, 0.0, 0.0,
		0.0, 0.0, 0.0,
		0.0, 1.0, 0.0,
		0.0, 0.0, 0.0,
		0.0, 0.0, 1.0
	};
	float AxisColors[] = {
		1.0, 0.0, 0.0, 1.0,
		1.0, 0.0, 0.0, 1.0,
		0.0, 1.0, 0.0, 1.0,
		0.0, 1.0, 0.0, 1.0,
		0.0, 0.0, 1.0, 1.0,
		0.0, 0.0, 1.0, 1.0
	};
	GLubyte AxisIdx[] = {
		0, 1,
		2, 3,
		4, 5
	};
	const char* Axis_vertShader =
		"#version 330\n\
in vec3 in_Position;\n\
in vec4 in_Color;\n\
out vec4 vert_color;\n\
uniform mat4 mvpMat;\n\
void main() {\n\
	vert_color = in_Color;\n\
	gl_Position = mvpMat * vec4(in_Position, 1.0);\n\
}";
	const char* Axis_fragShader =
		"#version 330\n\
in vec4 vert_color;\n\
out vec4 out_Color;\n\
void main() {\n\
	out_Color = vert_color;\n\
}";

	void setupAxis() {
		glGenVertexArrays(1, &AxisVao);
		glBindVertexArray(AxisVao);
		glGenBuffers(3, AxisVbo);

		glBindBuffer(GL_ARRAY_BUFFER, AxisVbo[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 24, AxisVerts, GL_STATIC_DRAW);
		glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, AxisVbo[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 24, AxisColors, GL_STATIC_DRAW);
		glVertexAttribPointer((GLuint)1, 4, GL_FLOAT, false, 0, 0);
		glEnableVertexAttribArray(1);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, AxisVbo[2]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLubyte) * 6, AxisIdx, GL_STATIC_DRAW);

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		AxisShader[0] = compileShader(Axis_vertShader, GL_VERTEX_SHADER, "AxisVert");
		AxisShader[1] = compileShader(Axis_fragShader, GL_FRAGMENT_SHADER, "AxisFrag");

		AxisProgram = glCreateProgram();
		glAttachShader(AxisProgram, AxisShader[0]);
		glAttachShader(AxisProgram, AxisShader[1]);
		glBindAttribLocation(AxisProgram, 0, "in_Position");
		glBindAttribLocation(AxisProgram, 1, "in_Color");
		linkProgram(AxisProgram);
	}
	void cleanupAxis() {
		glDeleteBuffers(3, AxisVbo);
		glDeleteVertexArrays(1, &AxisVao);

		glDeleteProgram(AxisProgram);
		glDeleteShader(AxisShader[0]);
		glDeleteShader(AxisShader[1]);
	}
	void drawAxis() {
		glBindVertexArray(AxisVao);
		glUseProgram(AxisProgram);
		glUniformMatrix4fv(glGetUniformLocation(AxisProgram, "mvpMat"), 1, GL_FALSE, glm::value_ptr(RV::_MVP));
		glDrawElements(GL_LINES, 6, GL_UNSIGNED_BYTE, 0);

		glUseProgram(0);
		glBindVertexArray(0);
	}
}

////////////////////////////////////////////////// CUBE
namespace Cube {
	GLuint cubeVao;
	GLuint cubeVbo[3];
	GLuint cubeShaders[2];
	GLuint cubeProgram;
	glm::mat4 objMat = glm::mat4(1.f);

	extern const float halfW = 0.5f;
	int numVerts = 24 + 6; // 4 vertex/face * 6 faces + 6 PRIMITIVE RESTART

						   //   4---------7
						   //  /|        /|
						   // / |       / |
						   //5---------6  |
						   //|  0------|--3
						   //| /       | /
						   //|/        |/
						   //1---------2
	glm::vec3 verts[] = {
		glm::vec3(-halfW, -halfW, -halfW),
		glm::vec3(-halfW, -halfW,  halfW),
		glm::vec3(halfW, -halfW,  halfW),
		glm::vec3(halfW, -halfW, -halfW),
		glm::vec3(-halfW,  halfW, -halfW),
		glm::vec3(-halfW,  halfW,  halfW),
		glm::vec3(halfW,  halfW,  halfW),
		glm::vec3(halfW,  halfW, -halfW)
	};
	glm::vec3 norms[] = {
		glm::vec3(0.f, -1.f,  0.f),
		glm::vec3(0.f,  1.f,  0.f),
		glm::vec3(-1.f,  0.f,  0.f),
		glm::vec3(1.f,  0.f,  0.f),
		glm::vec3(0.f,  0.f, -1.f),
		glm::vec3(0.f,  0.f,  1.f)
	};

	glm::vec3 cubeVerts[] = {
		verts[1], verts[0], verts[2], verts[3],
		verts[5], verts[6], verts[4], verts[7],
		verts[1], verts[5], verts[0], verts[4],
		verts[2], verts[3], verts[6], verts[7],
		verts[0], verts[4], verts[3], verts[7],
		verts[1], verts[2], verts[5], verts[6]
	};
	glm::vec3 cubeNorms[] = {
		norms[0], norms[0], norms[0], norms[0],
		norms[1], norms[1], norms[1], norms[1],
		norms[2], norms[2], norms[2], norms[2],
		norms[3], norms[3], norms[3], norms[3],
		norms[4], norms[4], norms[4], norms[4],
		norms[5], norms[5], norms[5], norms[5]
	};
	GLubyte cubeIdx[] = {
		0, 1, 2, 3, UCHAR_MAX,
		4, 5, 6, 7, UCHAR_MAX,
		8, 9, 10, 11, UCHAR_MAX,
		12, 13, 14, 15, UCHAR_MAX,
		16, 17, 18, 19, UCHAR_MAX,
		20, 21, 22, 23, UCHAR_MAX
	};

	const char* cube_vertShader =
		"#version 330\n\
	in vec3 in_Position;\n\
	in vec3 in_Normal;\n\
	out vec4 vert_Normal;\n\
	uniform mat4 objMat;\n\
	uniform mat4 mv_Mat;\n\
	uniform mat4 mvpMat;\n\
	void main() {\n\
		gl_Position = mvpMat * objMat * vec4(in_Position, 1.0);\n\
		vert_Normal = mv_Mat * objMat * vec4(in_Normal, 0.0);\n\
	}";


	const char* cube_fragShader =
		"#version 330\n\
in vec4 vert_Normal;\n\
out vec4 out_Color;\n\
uniform mat4 mv_Mat;\n\
uniform vec4 color;\n\
void main() {\n\
			out_Color = vec4(vec3(gl_FragCoord.z), 1.0);\n\
}";
	void setupCube() {
		glGenVertexArrays(1, &cubeVao);
		glBindVertexArray(cubeVao);
		glGenBuffers(3, cubeVbo);

		glBindBuffer(GL_ARRAY_BUFFER, cubeVbo[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVerts), cubeVerts, GL_STATIC_DRAW);
		glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, cubeVbo[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(cubeNorms), cubeNorms, GL_STATIC_DRAW);
		glVertexAttribPointer((GLuint)1, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(1);

		glPrimitiveRestartIndex(UCHAR_MAX);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeVbo[2]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeIdx), cubeIdx, GL_STATIC_DRAW);

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		cubeShaders[0] = compileShader(cube_vertShader, GL_VERTEX_SHADER, "cubeVert");
		cubeShaders[1] = compileShader(cube_fragShader, GL_FRAGMENT_SHADER, "cubeFrag");

		cubeProgram = glCreateProgram();
		glAttachShader(cubeProgram, cubeShaders[0]);
		glAttachShader(cubeProgram, cubeShaders[1]);
		glBindAttribLocation(cubeProgram, 0, "in_Position");
		glBindAttribLocation(cubeProgram, 1, "in_Normal");
		linkProgram(cubeProgram);
	}
	void cleanupCube() {
		glDeleteBuffers(3, cubeVbo);
		glDeleteVertexArrays(1, &cubeVao);

		glDeleteProgram(cubeProgram);
		glDeleteShader(cubeShaders[0]);
		glDeleteShader(cubeShaders[1]);
	}
	void updateCube(const glm::mat4& transform) {
		objMat = transform;
	}
	void drawCube() {
		glEnable(GL_PRIMITIVE_RESTART);
		glBindVertexArray(cubeVao);
		glUseProgram(cubeProgram);
		glUniformMatrix4fv(glGetUniformLocation(cubeProgram, "objMat"), 1, GL_FALSE, glm::value_ptr(objMat));
		glUniformMatrix4fv(glGetUniformLocation(cubeProgram, "mv_Mat"), 1, GL_FALSE, glm::value_ptr(RenderVars::_modelView));
		glUniformMatrix4fv(glGetUniformLocation(cubeProgram, "mvpMat"), 1, GL_FALSE, glm::value_ptr(RenderVars::_MVP));
		glUniform4f(glGetUniformLocation(cubeProgram, "color"), 0.1f, 1.f, 1.f, 0.f);
		glDrawElements(GL_TRIANGLE_STRIP, numVerts, GL_UNSIGNED_BYTE, 0);

		glUseProgram(0);
		glBindVertexArray(0);
		glDisable(GL_PRIMITIVE_RESTART);
	}


}
////////////////////////////////////////////////// MyModel
namespace Wheel {
	GLuint modelVao;
	GLuint modelVbo[3];
	GLuint modelShaders[2];
	GLuint modelProgram;
	glm::mat4 objMat = glm::mat4(1.f);

	const char* model_vertShader =
		"#version 330\n\
	in vec3 in_Position;\n\
	in vec3 in_Normal;\n\
	uniform vec3 lPos;\n\
	uniform vec3 lPos2;\n\
	out vec3 lDir;\n\
	out vec3 lDir2;\n\
	out vec4 vert_Normal;\n\
	uniform mat4 objMat;\n\
	uniform mat4 mv_Mat;\n\
	uniform mat4 mvpMat;\n\
	void main() {\n\
		gl_Position = mvpMat * objMat * vec4(in_Position, 1.0);\n\
		vert_Normal = mv_Mat * objMat * vec4(in_Normal, 0.0);\n\
		lDir = normalize(lPos - gl_Position.xyz );\n\
		lDir2 = normalize(lPos2 - gl_Position.xyz );\n\
	}";


	const char* model_fragShader =
		"#version 330\n\
in vec4 vert_Normal;\n\
in vec3 lDir;\n\
in vec3 lDir2;\n\
out vec4 out_Color;\n\
uniform mat4 mv_Mat;\n\
uniform vec4 color;\n\
uniform vec4 color2;\n\
uniform vec4 ambient;\n\
void main() {\n\
		float ToonShading = 0.0f;\n\
		ToonShading = dot(lDir,normalize(vert_Normal.xyz));\n\
		if(ToonShading < 0.2) ToonShading = 0; \n\
		if(ToonShading >= 0.2 && ToonShading < 0.4) ToonShading = 0.2f; \n\
		if(ToonShading >= 0.4 && ToonShading < 0.5) ToonShading = 0.4f; \n\
		if(ToonShading >= 0.5f) ToonShading = 1; \n\
	out_Color = vec4(ambient.rgb, 1.0) + vec4(color.xyz * dot(vert_Normal, mv_Mat*vec4(ToonShading * 64 , ToonShading* 64, ToonShading* 64 , 0.0)) , 1.0 )\n\
	+ vec4(color2.xyz * dot(vert_Normal, mv_Mat*vec4(lDir2.x * 128 , lDir2.y * 128 , lDir2.z * 128 , 0.0)) , 1.0 );\n\
}";

	void setupModel() {
		glGenVertexArrays(1, &modelVao);
		glBindVertexArray(modelVao);
		glGenBuffers(3, modelVbo);

		glBindBuffer(GL_ARRAY_BUFFER, modelVbo[0]);

		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);
		glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, modelVbo[1]);

		glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);
		glVertexAttribPointer((GLuint)1, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(1);

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		modelShaders[0] = compileShader(model_vertShader, GL_VERTEX_SHADER, "cubeVert");
		modelShaders[1] = compileShader(model_fragShader, GL_FRAGMENT_SHADER, "cubeFrag");

		modelProgram = glCreateProgram();
		glAttachShader(modelProgram, modelShaders[0]);
		glAttachShader(modelProgram, modelShaders[1]);
		glBindAttribLocation(modelProgram, 0, "in_Position");
		glBindAttribLocation(modelProgram, 1, "in_Normal");
		linkProgram(modelProgram);
	}
	void cleanupModel() {

		glDeleteBuffers(2, modelVbo);
		glDeleteVertexArrays(1, &modelVao);

		glDeleteProgram(modelProgram);
		glDeleteShader(modelShaders[0]);
		glDeleteShader(modelShaders[1]);
	}
	void updateModel(const glm::mat4& transform) {
		objMat = transform;
	}
	void drawModel() {

		glBindVertexArray(modelVao);
		glUseProgram(modelProgram);
		glUniformMatrix4fv(glGetUniformLocation(modelProgram, "objMat"), 1, GL_FALSE, glm::value_ptr(objMat));
		glUniformMatrix4fv(glGetUniformLocation(modelProgram, "mv_Mat"), 1, GL_FALSE, glm::value_ptr(RenderVars::_modelView));
		glUniformMatrix4fv(glGetUniformLocation(modelProgram, "mvpMat"), 1, GL_FALSE, glm::value_ptr(RenderVars::_MVP));
		glUniform3f(glGetUniformLocation(modelProgram, "lPos"), lightPosition.x, lightPosition.y, lightPosition.z);
		glUniform3f(glGetUniformLocation(modelProgram, "lPos2"), lightPosition2.x, lightPosition2.y, lightPosition2.z);
		glUniform4f(glGetUniformLocation(modelProgram, "color"), Color.x, Color.y, Color.z, 0.f);
		glUniform4f(glGetUniformLocation(modelProgram, "color2"), Color2.x, Color2.y, Color2.z, 0.f);
		glUniform4f(glGetUniformLocation(modelProgram, "ambient"), 0.0f, 0.0f, 0.2f, 0.f);

		glStencilFunc(GL_ALWAYS, 1, 0xF);
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
		glStencilMask(0xF);
		glDepthMask(GL_FALSE);
		glClear(GL_STENCIL_BUFFER_BIT);
		glDrawArrays(GL_TRIANGLES, 0, vertices.size());
		glStencilFunc(GL_EQUAL, 1, 0xF);
		glStencilMask(0x00);
		glDepthMask(GL_TRUE);
		glDisable(GL_STENCIL_TEST);

		/*
		The stencil nomes he aconseguit fer que la roda quedi darrere de las potes en qualsevol angle.
		*/

		glUseProgram(0);
		glBindVertexArray(0);

	}


}

namespace Wheel_Feet {
	GLuint modelVao;
	GLuint modelVbo[3];
	GLuint modelShaders[2];
	GLuint modelProgram;
	glm::mat4 objMat = glm::mat4(1.f);

	const char* model_vertShader =
		"#version 330\n\
	in vec3 in_Position;\n\
	in vec3 in_Normal;\n\
	uniform vec3 lPos;\n\
	uniform vec3 lPos2;\n\
	out vec3 lDir;\n\
	out vec3 lDir2;\n\
	out vec4 vert_Normal;\n\
	uniform mat4 objMat;\n\
	uniform mat4 mv_Mat;\n\
	uniform mat4 mvpMat;\n\
	void main() {\n\
		gl_Position = mvpMat * objMat * vec4(in_Position, 1.0);\n\
		vert_Normal = mv_Mat * objMat * vec4(in_Normal, 0.0);\n\
		lDir = normalize(lPos - gl_Position.xyz );\n\
		lDir2 = normalize(lPos2 - gl_Position.xyz );\n\
	}";


	const char* model_fragShader =
		"#version 330\n\
		in vec4 vert_Normal;\n\
		in vec3 lDir;\n\
		in vec3 lDir2;\n\
		out vec4 out_Color;\n\
		uniform mat4 mv_Mat;\n\
		uniform vec4 color;\n\
		uniform vec4 color2;\n\
		uniform vec4 ambient;\n\
		void main() {\n\
		float ToonShading = 0.0f;\n\
		ToonShading = dot(lDir,normalize(vert_Normal.xyz));\n\
		if(ToonShading < 0.2) ToonShading = 0; \n\
		if(ToonShading >= 0.2 && ToonShading < 0.4) ToonShading = 0.2f; \n\
		if(ToonShading >= 0.4 && ToonShading < 0.5) ToonShading = 0.4f; \n\
		if(ToonShading >= 0.5f) ToonShading = 1; \n\
		out_Color = vec4(vec3(gl_FragCoord.z), 1.0);\n\
		}";



	/*
	out_Color = vec4(ambient.rgb, 1.0) + vec4(color.xyz * dot(vert_Normal, mv_Mat*vec4(ToonShading * 64 , ToonShading* 64, ToonShading* 64 , 0.0)) , 1.0 )\n\
	+ vec4(color2.xyz * dot(vert_Normal, mv_Mat*vec4(lDir2.x * 128 , lDir2.y * 128 , lDir2.z * 128 , 0.0)) , 1.0 );\n\
	*/
	void setupModel() {
		glGenVertexArrays(1, &modelVao);
		glBindVertexArray(modelVao);
		glGenBuffers(3, modelVbo);

		glBindBuffer(GL_ARRAY_BUFFER, modelVbo[0]);

		glBufferData(GL_ARRAY_BUFFER, verticesWheel_Feet.size() * sizeof(glm::vec3), &verticesWheel_Feet[0], GL_STATIC_DRAW);
		glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, modelVbo[1]);

		glBufferData(GL_ARRAY_BUFFER, normalsWheel_Feet.size() * sizeof(glm::vec3), &normalsWheel_Feet[0], GL_STATIC_DRAW);
		glVertexAttribPointer((GLuint)1, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(1);

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		modelShaders[0] = compileShader(model_vertShader, GL_VERTEX_SHADER, "cubeVert");
		modelShaders[1] = compileShader(model_fragShader, GL_FRAGMENT_SHADER, "cubeFrag");

		modelProgram = glCreateProgram();
		glAttachShader(modelProgram, modelShaders[0]);
		glAttachShader(modelProgram, modelShaders[1]);
		glBindAttribLocation(modelProgram, 0, "in_Position");
		glBindAttribLocation(modelProgram, 1, "in_Normal");

		linkProgram(modelProgram);
	}
	void cleanupModel() {

		glDeleteBuffers(2, modelVbo);
		glDeleteVertexArrays(1, &modelVao);

		glDeleteProgram(modelProgram);
		glDeleteShader(modelShaders[0]);
		glDeleteShader(modelShaders[1]);
	}
	void updateModel(const glm::mat4& transform) {
		objMat = transform;
	}
	void drawModel() {

		glBindVertexArray(modelVao);
		glUseProgram(modelProgram);
		glUniformMatrix4fv(glGetUniformLocation(modelProgram, "objMat"), 1, GL_FALSE, glm::value_ptr(objMat));
		glUniformMatrix4fv(glGetUniformLocation(modelProgram, "mv_Mat"), 1, GL_FALSE, glm::value_ptr(RenderVars::_modelView));
		glUniformMatrix4fv(glGetUniformLocation(modelProgram, "mvpMat"), 1, GL_FALSE, glm::value_ptr(RenderVars::_MVP));
		glUniform3f(glGetUniformLocation(modelProgram, "lPos"), lightPosition.x, lightPosition.y, lightPosition.z);
		glUniform3f(glGetUniformLocation(modelProgram, "lPos2"), lightPosition2.x, lightPosition2.y, lightPosition2.z);
		glUniform4f(glGetUniformLocation(modelProgram, "color"), Color.x, Color.y, Color.z, 0.f);
		glUniform4f(glGetUniformLocation(modelProgram, "color2"), Color2.x, Color2.y, Color2.z, 0.f);
		glUniform4f(glGetUniformLocation(modelProgram, "ambient"), 0.0f, 0.0f, 0.2f, 0.f);

		glDrawArrays(GL_TRIANGLES, 0, 100000);


		glUseProgram(0);
		glBindVertexArray(0);

	}


}

namespace Trump {
	GLuint modelVao;
	GLuint modelVbo[3];
	GLuint modelShaders[2];
	GLuint modelProgram;
	glm::mat4 objMat = glm::mat4(1.f);

	const char* model_vertShader =
		"#version 330\n\
	in vec3 in_Position;\n\
	in vec3 in_Normal;\n\
	uniform vec3 lPos;\n\
	uniform vec3 lPos2;\n\
	out vec3 lDir;\n\
	out vec3 lDir2;\n\
	out vec4 vert_Normal;\n\
	uniform mat4 objMat;\n\
	uniform mat4 mv_Mat;\n\
	uniform mat4 mvpMat;\n\
	void main() {\n\
		gl_Position = mvpMat * objMat * vec4(in_Position, 1.0);\n\
		vert_Normal = mv_Mat * objMat * vec4(in_Normal, 0.0);\n\
		lDir = normalize(lPos - gl_Position.xyz );\n\
		lDir2 = normalize(lPos2 - gl_Position.xyz );\n\
	}";


	const char* model_fragShader =
		"#version 330\n\
in vec4 vert_Normal;\n\
in vec3 lDir;\n\
in vec3 lDir2;\n\
out vec4 out_Color;\n\
uniform mat4 mv_Mat;\n\
uniform vec4 color;\n\
uniform vec4 color2;\n\
uniform vec4 ambient;\n\
void main() {\n\
		float ToonShading = 0.0f;\n\
		ToonShading = dot(lDir,normalize(vert_Normal.xyz));\n\
		if(ToonShading < 0.2) ToonShading = 0; \n\
		if(ToonShading >= 0.2 && ToonShading < 0.4) ToonShading = 0.2f; \n\
		if(ToonShading >= 0.4 && ToonShading < 0.5) ToonShading = 0.4f; \n\
		if(ToonShading >= 0.5f) ToonShading = 1; \n\
	out_Color = vec4(ambient.rgb, 1.0) + vec4(color.xyz * dot(vert_Normal, mv_Mat*vec4(ToonShading * 64 , ToonShading* 64, ToonShading* 64 , 0.0)) , 1.0 )\n\
	+ vec4(color2.xyz * dot(vert_Normal, mv_Mat*vec4(lDir2.x * 128 , lDir2.y * 128 , lDir2.z * 128 , 0.0)) , 1.0 );\n\
}";

	void setupModel() {
		glGenVertexArrays(1, &modelVao);
		glBindVertexArray(modelVao);
		glGenBuffers(3, modelVbo);

		glBindBuffer(GL_ARRAY_BUFFER, modelVbo[0]);

		glBufferData(GL_ARRAY_BUFFER, verticesTrump.size() * sizeof(glm::vec3), &verticesTrump[0], GL_STATIC_DRAW);
		glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, modelVbo[1]);

		glBufferData(GL_ARRAY_BUFFER, normalsTrump.size() * sizeof(glm::vec3), &normalsTrump[0], GL_STATIC_DRAW);
		glVertexAttribPointer((GLuint)1, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(1);

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		modelShaders[0] = compileShader(model_vertShader, GL_VERTEX_SHADER, "cubeVert");
		modelShaders[1] = compileShader(model_fragShader, GL_FRAGMENT_SHADER, "cubeFrag");

		modelProgram = glCreateProgram();
		glAttachShader(modelProgram, modelShaders[0]);
		glAttachShader(modelProgram, modelShaders[1]);
		glBindAttribLocation(modelProgram, 0, "in_Position");
		glBindAttribLocation(modelProgram, 1, "in_Normal");
		linkProgram(modelProgram);
	}
	void cleanupModel() {

		glDeleteBuffers(2, modelVbo);
		glDeleteVertexArrays(1, &modelVao);

		glDeleteProgram(modelProgram);
		glDeleteShader(modelShaders[0]);
		glDeleteShader(modelShaders[1]);
	}
	void updateModel(const glm::mat4& transform) {
		objMat = transform;
	}
	void drawModel() {

		glBindVertexArray(modelVao);
		glUseProgram(modelProgram);
		glUniformMatrix4fv(glGetUniformLocation(modelProgram, "objMat"), 1, GL_FALSE, glm::value_ptr(objMat));
		glUniformMatrix4fv(glGetUniformLocation(modelProgram, "mv_Mat"), 1, GL_FALSE, glm::value_ptr(RenderVars::_modelView));
		glUniformMatrix4fv(glGetUniformLocation(modelProgram, "mvpMat"), 1, GL_FALSE, glm::value_ptr(RenderVars::_MVP));
		glUniform3f(glGetUniformLocation(modelProgram, "lPos"), lightPosition.x, lightPosition.y, lightPosition.z);
		glUniform3f(glGetUniformLocation(modelProgram, "lPos2"), lightPosition2.x, lightPosition2.y, lightPosition2.z);
		glUniform4f(glGetUniformLocation(modelProgram, "color"), Color.x, Color.y, Color.z, 0.f);
		glUniform4f(glGetUniformLocation(modelProgram, "color2"), Color2.x, Color2.y, Color2.z, 0.f);
		glUniform4f(glGetUniformLocation(modelProgram, "ambient"), 0.0f, 0.0f, 0.2f, 0.f);

		glDrawArrays(GL_TRIANGLES, 0, 100000);


		glUseProgram(0);
		glBindVertexArray(0);

	}


}

namespace Chicken {
	GLuint modelVao;
	GLuint modelVbo[3];
	GLuint modelShaders[2];
	GLuint modelProgram;
	glm::mat4 objMat = glm::mat4(1.f);

	const char* model_vertShader =
		"#version 330\n\
	in vec3 in_Position;\n\
	in vec3 in_Normal;\n\
	uniform vec3 lPos;\n\
	uniform vec3 lPos2;\n\
	out vec3 lDir;\n\
	out vec3 lDir2;\n\
	out vec4 vert_Normal;\n\
	uniform mat4 objMat;\n\
	uniform mat4 mv_Mat;\n\
	uniform mat4 mvpMat;\n\
	void main() {\n\
		gl_Position = mvpMat * objMat * vec4(in_Position, 1.0);\n\
		vert_Normal = mv_Mat * objMat * vec4(in_Normal, 0.0);\n\
		lDir = normalize(lPos - gl_Position.xyz );\n\
		lDir2 = normalize(lPos2 - gl_Position.xyz );\n\
	}";


	const char* model_fragShader =
		"#version 330\n\
in vec4 vert_Normal;\n\
in vec3 lDir;\n\
in vec3 lDir2;\n\
out vec4 out_Color;\n\
uniform mat4 mv_Mat;\n\
uniform vec4 color;\n\
uniform vec4 color2;\n\
uniform vec4 ambient;\n\
uniform float ToonShading;\n\
uniform float ToonShading2;\n\
uniform float ToonShadingLEVEL;\n\
void main() {\n\
		ToonShading = dot(lDir,normalize(vert_Normal.xyz));\n\
		ToonShading2 =  dot(lDir2,normalize(vert_Normal.xyz));\n\
	if(ToonShadingLEVEL == 0) {\n\
	out_Color = vec4(ambient.rgb, 1.0) + vec4(color.xyz * dot(vert_Normal, mv_Mat*vec4(ToonShading * 128 , ToonShading* 128, \n\
		ToonShading* 128 , 0.0)) , 1.0 )\n\
	+ vec4(color2.xyz * dot(vert_Normal, mv_Mat*vec4(lDir2.x * 128 , lDir2.y * 128 , lDir2.z * 128 , 0.0)) , 1.0 );\n\
	}\n\
	if(ToonShadingLEVEL == 1) {\n\
	out_Color = vec4(ambient.rgb, 1.0) + vec4(color.xyz * dot(vert_Normal, mv_Mat*vec4(ToonShading * 128 , ToonShading* 128, \n\
		ToonShading* 128 , 0.0)) , 1.0 )\n\
	+ vec4(color2.xyz * dot(vert_Normal, mv_Mat*vec4(lDir2.x * 128 , lDir2.y * 128 , lDir2.z * 128 , 0.0)) , 1.0 );\n\
	}\n\
	if(ToonShadingLEVEL == 2) {\n\
	out_Color = vec4(ambient.rgb, 1.0) + vec4(color2.xyz * dot(vert_Normal, mv_Mat*vec4(ToonShading2 * 128 , ToonShading2 * 128 ,\n\
		ToonShading2 * 128 , 0.0)) , 1.0 );\n\
	}\n\
}";

	void setupModel() {
		glGenVertexArrays(1, &modelVao);
		glBindVertexArray(modelVao);
		glGenBuffers(3, modelVbo);

		glBindBuffer(GL_ARRAY_BUFFER, modelVbo[0]);

		glBufferData(GL_ARRAY_BUFFER, verticesChicken.size() * sizeof(glm::vec3), &verticesChicken[0], GL_STATIC_DRAW);
		glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, modelVbo[1]);

		glBufferData(GL_ARRAY_BUFFER, normalsChicken.size() * sizeof(glm::vec3), &normalsChicken[0], GL_STATIC_DRAW);
		glVertexAttribPointer((GLuint)1, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(1);

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		modelShaders[0] = compileShader(model_vertShader, GL_VERTEX_SHADER, "cubeVert");
		modelShaders[1] = compileShader(model_fragShader, GL_FRAGMENT_SHADER, "cubeFrag");

		modelProgram = glCreateProgram();
		glAttachShader(modelProgram, modelShaders[0]);
		glAttachShader(modelProgram, modelShaders[1]);
		glBindAttribLocation(modelProgram, 0, "in_Position");
		glBindAttribLocation(modelProgram, 1, "in_Normal");
		linkProgram(modelProgram);
	}
	void cleanupModel() {

		glDeleteBuffers(2, modelVbo);
		glDeleteVertexArrays(1, &modelVao);

		glDeleteProgram(modelProgram);
		glDeleteShader(modelShaders[0]);
		glDeleteShader(modelShaders[1]);
	}
	void updateModel(const glm::mat4& transform) {
		objMat = transform;
	}
	void drawModel() {

		glBindVertexArray(modelVao);
		glUseProgram(modelProgram);
		glUniformMatrix4fv(glGetUniformLocation(modelProgram, "objMat"), 1, GL_FALSE, glm::value_ptr(objMat));
		glUniformMatrix4fv(glGetUniformLocation(modelProgram, "mv_Mat"), 1, GL_FALSE, glm::value_ptr(RenderVars::_modelView));
		glUniformMatrix4fv(glGetUniformLocation(modelProgram, "mvpMat"), 1, GL_FALSE, glm::value_ptr(RenderVars::_MVP));
		glUniform3f(glGetUniformLocation(modelProgram, "lPos"), lightPosition.x, lightPosition.y, lightPosition.z);
		glUniform3f(glGetUniformLocation(modelProgram, "lPos2"), lightPosition2.x, lightPosition2.y, lightPosition2.z);
		glUniform4f(glGetUniformLocation(modelProgram, "color"), Color.x, Color.y, Color.z, 0.f);
		glUniform4f(glGetUniformLocation(modelProgram, "color2"), Color2.x, Color2.y, Color2.z, 0.f);
		glUniform4f(glGetUniformLocation(modelProgram, "ambient"), 0.0f, 0.0f, 0.2f, 0.f);
		glUniform1f(glGetUniformLocation(modelProgram, "ToonShading"), ToonShading);
		glUniform1f(glGetUniformLocation(modelProgram, "ToonShading2"), ToonShading2);
		glUniform1f(glGetUniformLocation(modelProgram, "ToonShadingLEVEL"), ToonShadingLEVEL);

		glDrawArrays(GL_TRIANGLES, 0, 100000);

		glUseProgram(0);
		glBindVertexArray(0);

	}


}

////////////////////////////////////////////////// MyModel
namespace Cabins {
	GLuint modelVao;
	GLuint modelVbo[3];
	GLuint modelShaders[2];
	GLuint modelProgram;
	glm::mat4 objMat = glm::mat4(1.f);

	const char* model_vertShader =
		"#version 330\n\
	in vec3 in_Position;\n\
	in vec3 in_Normal;\n\
	uniform vec3 lPos;\n\
	uniform vec3 lPos2;\n\
	out vec3 lDir;\n\
	out vec3 lDir2;\n\
	out vec4 vert_Normal;\n\
	uniform mat4 objMat;\n\
	uniform mat4 mv_Mat;\n\
	uniform mat4 mvpMat;\n\
	void main() {\n\
		gl_Position = mvpMat * objMat * vec4(in_Position, 1.0);\n\
		vert_Normal = mv_Mat * objMat * vec4(in_Normal, 0.0);\n\
		lDir = normalize(lPos - gl_Position.xyz );\n\
		lDir2 = normalize(lPos2 - gl_Position.xyz );\n\
	}";


	const char* model_fragShader =
		"#version 330\n\
in vec4 vert_Normal;\n\
in vec3 lDir;\n\
in vec3 lDir2;\n\
out vec4 out_Color;\n\
uniform mat4 mv_Mat;\n\
uniform vec4 color;\n\
uniform vec4 color2;\n\
uniform vec4 ambient;\n\
void main() {\n\
		float ToonShading = 0.0f;\n\
		float ToonShading2 = 0.0f;\n\
		ToonShading = dot(lDir,normalize(vert_Normal.xyz));\n\
		if(ToonShading < 0.2) ToonShading = 0.0f; \n\
		if(ToonShading >= 0.2 && ToonShading < 0.4) ToonShading = 0.2f; \n\
		if(ToonShading >= 0.4 && ToonShading < 0.5) ToonShading = 0.4f; \n\
		if(ToonShading >= 0.5f) ToonShading = 1.0f; \n\
		ToonShading2 = dot(lDir,normalize(vert_Normal.xyz));\n\
	out_Color = vec4(ambient.rgb, 1.0) + vec4(color.xyz * dot(vert_Normal, mv_Mat*vec4(	ToonShading * 64 , ToonShading * 64 , ToonShading* 64 , 0.0)) , 1.0 )\n\
	+ vec4(color2.xyz * dot(vert_Normal, mv_Mat*vec4(ToonShading2 * 64, ToonShading2* 64 , ToonShading2* 64  , 0.0)) , 1.0 );\n\
}";


	/*
	if (ToonShading2 < 0.2) ToonShading2 = 0; \n\
	if (ToonShading2 >= 0.2 && ToonShading2 < 0.4) ToonShading2 = 0.2f; \n\
	if (ToonShading2 >= 0.4 && ToonShading2 < 0.5) ToonShading2 = 0.4f; \n\
	if (ToonShading2 >= 0.5f) ToonShading2 = 1; \n\
	lDir.x
	lDir.y
	lDir.z

	float ToonShading = 0.0f;\n\
	ToonShading = dot(lDir,normalize(vert_Normal.xyz));\n\
	ToonShading2 = dot(lDir,normalize(vert_Normal.xyz));\n\
	if(ToonShading < 0.2) ToonShading = 0.0f; \n\
	if(ToonShading >= 0.2 && ToonShading < 0.4) ToonShading = 0.2f; \n\
	if(ToonShading >= 0.4 && ToonShading < 0.5) ToonShading = 0.4f; \n\
	if(ToonShading >= 0.5f) ToonShading = 1.0f; \n\

	*/
	void setupModel() {
		glGenVertexArrays(1, &modelVao);
		glBindVertexArray(modelVao);
		glGenBuffers(3, modelVbo);

		glBindBuffer(GL_ARRAY_BUFFER, modelVbo[0]);

		glBufferData(GL_ARRAY_BUFFER, verticesCabins.size() * sizeof(glm::vec3), &verticesCabins[0], GL_STATIC_DRAW);
		glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, modelVbo[1]);

		glBufferData(GL_ARRAY_BUFFER, normalsCabins.size() * sizeof(glm::vec3), &normalsCabins[0], GL_STATIC_DRAW);
		glVertexAttribPointer((GLuint)1, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(1);

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		modelShaders[0] = compileShader(model_vertShader, GL_VERTEX_SHADER, "cubeVert");
		modelShaders[1] = compileShader(model_fragShader, GL_FRAGMENT_SHADER, "cubeFrag");

		modelProgram = glCreateProgram();
		glAttachShader(modelProgram, modelShaders[0]);
		glAttachShader(modelProgram, modelShaders[1]);
		glBindAttribLocation(modelProgram, 0, "in_Position");
		glBindAttribLocation(modelProgram, 1, "in_Normal");
		linkProgram(modelProgram);
	}
	void cleanupModel() {

		glDeleteBuffers(2, modelVbo);
		glDeleteVertexArrays(1, &modelVao);

		glDeleteProgram(modelProgram);
		glDeleteShader(modelShaders[0]);
		glDeleteShader(modelShaders[1]);
	}
	void updateModel(const glm::mat4& transform) {
		objMat = transform;
	}
	void drawModel() {

		glBindVertexArray(modelVao);
		glUseProgram(modelProgram);
		glUniformMatrix4fv(glGetUniformLocation(modelProgram, "objMat"), 1, GL_FALSE, glm::value_ptr(objMat));
		glUniformMatrix4fv(glGetUniformLocation(modelProgram, "mv_Mat"), 1, GL_FALSE, glm::value_ptr(RenderVars::_modelView));
		glUniformMatrix4fv(glGetUniformLocation(modelProgram, "mvpMat"), 1, GL_FALSE, glm::value_ptr(RenderVars::_MVP));
		glUniform3f(glGetUniformLocation(modelProgram, "lPos"), lightPosition.x, lightPosition.y, lightPosition.z);
		glUniform3f(glGetUniformLocation(modelProgram, "lPos2"), lightPosition2.x, lightPosition2.y, lightPosition2.z);
		glUniform4f(glGetUniformLocation(modelProgram, "color"), Color.x, Color.y, Color.z, 0.f);
		glUniform4f(glGetUniformLocation(modelProgram, "color2"), Color2.x, Color2.y, Color2.z, 0.f);
		glUniform4f(glGetUniformLocation(modelProgram, "ambient"), 0.0f, 0.0f, 0.2f, 0.f);

		//3 exercises OPTIMIZACION
		glDrawArrays(GL_TRIANGLES, 0, verticesCabins.size());
		/*glDrawArraysInstanced(GL_TRIANGLES, 0, verticesCabins.size(), 4);*/
		//glMultiDrawArraysIndirect(GL_TRIANGLES, 0, verticesCabins.size(), 0);
		//glMultiDrawArraysIndirect hem peta i no se el perque.

		glUseProgram(0);
		glBindVertexArray(0);

	}


}


//
//namespace Trump {
//	GLuint cubeVao;
//	GLuint cubeVbo[3];
//	GLuint cubeShaders[2];
//	GLuint cubeProgram;
//	glm::mat4 objMat = glm::mat4(1.f);
//
//	extern const float halfW = 0.5f;
//	int numVerts = 24 + 6; // 4 vertex/face * 6 faces + 6 PRIMITIVE RESTART
//
//						   //   4---------7
//						   //  /|        /|
//						   // / |       / |
//						   //5---------6  |
//						   //|  0------|--3
//						   //| /       | /
//						   //|/        |/
//						   //1---------2
//	glm::vec3 verts[] = {
//		glm::vec3(-halfW, -halfW, -halfW),
//		glm::vec3(-halfW, -halfW,  halfW),
//		glm::vec3(halfW, -halfW,  halfW),
//		glm::vec3(halfW, -halfW, -halfW),
//		glm::vec3(-halfW,  halfW, -halfW),
//		glm::vec3(-halfW,  halfW,  halfW),
//		glm::vec3(halfW,  halfW,  halfW),
//		glm::vec3(halfW,  halfW, -halfW)
//	};
//	glm::vec3 norms[] = {
//		glm::vec3(0.f, -1.f,  0.f),
//		glm::vec3(0.f,  1.f,  0.f),
//		glm::vec3(-1.f,  0.f,  0.f),
//		glm::vec3(1.f,  0.f,  0.f),
//		glm::vec3(0.f,  0.f, -1.f),
//		glm::vec3(0.f,  0.f,  1.f)
//	};
//
//	glm::vec3 cubeVerts[] = {
//		verts[1], verts[0], verts[2], verts[3],
//		verts[5], verts[6], verts[4], verts[7],
//		verts[1], verts[5], verts[0], verts[4],
//		verts[2], verts[3], verts[6], verts[7],
//		verts[0], verts[4], verts[3], verts[7],
//		verts[1], verts[2], verts[5], verts[6]
//	};
//	glm::vec3 cubeNorms[] = {
//		norms[0], norms[0], norms[0], norms[0],
//		norms[1], norms[1], norms[1], norms[1],
//		norms[2], norms[2], norms[2], norms[2],
//		norms[3], norms[3], norms[3], norms[3],
//		norms[4], norms[4], norms[4], norms[4],
//		norms[5], norms[5], norms[5], norms[5]
//	};
//	GLubyte cubeIdx[] = {
//		0, 1, 2, 3, UCHAR_MAX,
//		4, 5, 6, 7, UCHAR_MAX,
//		8, 9, 10, 11, UCHAR_MAX,
//		12, 13, 14, 15, UCHAR_MAX,
//		16, 17, 18, 19, UCHAR_MAX,
//		20, 21, 22, 23, UCHAR_MAX
//	};
//
//
//	const char* cube_vertShader =
//		"#version 330\n\
//	in vec3 in_Position;\n\
//	in vec3 in_Normal;\n\
//	out vec4 vert_Normal;\n\
//	uniform mat4 objMat;\n\
//	uniform mat4 mv_Mat;\n\
//	uniform mat4 mvpMat;\n\
//	out vec3 pos;\n\
//	void main() {\n\
//		gl_Position = mvpMat * objMat * vec4(in_Position, 1.0);\n\
//		vert_Normal = mv_Mat * objMat * vec4(in_Normal, 0.0);\n\
//		pos = in_Position;\n\
//	}";
//
//	const char* cube_fragShader =
//		"#version 330\n\
//in vec4 vert_Normal;\n\
//out vec4 out_Color;\n\
//uniform mat4 mv_Mat;\n\
//uniform vec4 color;\n\
//uniform vec3 lightPosition;\n\
//uniform vec3 ambient;\n\
//in vec3 pos;\n\
//void main() {\n\
//	float dot_product = max(dot(normalize(lightPosition), normalize(vert_Normal.xyz)), 0.0);\n\
//	out_Color = dot_product * color * 4.0f;\n\
//}";
//
//
//	void setupCube() {
//		glGenVertexArrays(1, &cubeVao);
//		glBindVertexArray(cubeVao);
//		glGenBuffers(2, cubeVbo);
//
//		glBindBuffer(GL_ARRAY_BUFFER, cubeVbo[0]);
//		/*	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVerts), cubeVerts, GL_STATIC_DRAW);*/
//		glBufferData(GL_ARRAY_BUFFER, vertices2.size() * sizeof(vec3), &vertices2[0], GL_STATIC_DRAW);
//		glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, 0);
//		glEnableVertexAttribArray(0);
//
//		glBindBuffer(GL_ARRAY_BUFFER, cubeVbo[1]);
//		/*glBufferData(GL_ARRAY_BUFFER, sizeof(cubeNorms), cubeNorms, GL_STATIC_DRAW);*/
//		glBufferData(GL_ARRAY_BUFFER, normals2.size() * sizeof(vec3), &normals2[0], GL_STATIC_DRAW);
//		glVertexAttribPointer((GLuint)1, 3, GL_FLOAT, GL_FALSE, 0, 0);
//		glEnableVertexAttribArray(1);
//
//		//glPrimitiveRestartIndex(UCHAR_MAX);
//		//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeVbo[2]);
//		//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeIdx), cubeIdx, GL_STATIC_DRAW);
//
//		glBindVertexArray(0);
//		glBindBuffer(GL_ARRAY_BUFFER, 0);
//		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
//
//		cubeShaders[0] = compileShader(cube_vertShader, GL_VERTEX_SHADER, "cubeVert");
//		cubeShaders[1] = compileShader(cube_fragShader, GL_FRAGMENT_SHADER, "cubeFrag");
//
//		cubeProgram = glCreateProgram();
//		glAttachShader(cubeProgram, cubeShaders[0]);
//		glAttachShader(cubeProgram, cubeShaders[1]);
//		glBindAttribLocation(cubeProgram, 0, "in_Position");
//		glBindAttribLocation(cubeProgram, 1, "in_Normal");
//		glBindAttribLocation(cubeProgram, 2, "in_Color");
//		linkProgram(cubeProgram);
//	}
//	void cleanupCube() {
//		/*glDeleteBuffers(3, cubeVbo);*/
//		glDeleteVertexArrays(1, &cubeVao);
//
//		glDeleteProgram(cubeProgram);
//		glDeleteShader(cubeShaders[0]);
//		glDeleteShader(cubeShaders[1]);
//	}
//	void updateCube(const glm::mat4& transform) {
//		objMat = transform;
//	}
//	void drawCube() {
//		/*	glEnable(GL_PRIMITIVE_RESTART);*/
//		glBindVertexArray(cubeVao);
//		glUseProgram(cubeProgram);
//		glUniformMatrix4fv(glGetUniformLocation(cubeProgram, "objMat"), 1, GL_FALSE, glm::value_ptr(objMat));
//		glUniformMatrix4fv(glGetUniformLocation(cubeProgram, "mv_Mat"), 1, GL_FALSE, glm::value_ptr(RenderVars::_modelView));
//		glUniformMatrix4fv(glGetUniformLocation(cubeProgram, "mvpMat"), 1, GL_FALSE, glm::value_ptr(RenderVars::_MVP));
//		glUniform4f(glGetUniformLocation(cubeProgram, "color"), .2f, 0, 0, 0.f);
//		glUniform3f(glGetUniformLocation(cubeProgram, "lightPosition"), lightPosition.x, lightPosition.y, lightPosition.z);
//		glUniform4f(glGetUniformLocation(cubeProgram, "ambient"), 0.4f, 0.4f, 0.4f, 1.f);
//		/*glDrawElements(GL_TRIANGLE_STRIP, numVerts, GL_UNSIGNED_BYTE, 0);*/
//		glDrawArrays(GL_TRIANGLES, 0, 100000);
//
//		glUseProgram(0);
//		glBindVertexArray(0);
//		/*glDisable(GL_PRIMITIVE_RESTART);*/
//	}
//
//
//
//}
//
//namespace Chicken {
//	GLuint cubeVao;
//	GLuint cubeVbo[3];
//	GLuint cubeShaders[2];
//	GLuint cubeProgram;
//	glm::mat4 objMat = glm::mat4(1.f);
//
//	extern const float halfW = 0.5f;
//	int numVerts = 24 + 6; // 4 vertex/face * 6 faces + 6 PRIMITIVE RESTART
//
//						   //   4---------7
//						   //  /|        /|
//						   // / |       / |
//						   //5---------6  |
//						   //|  0------|--3
//						   //| /       | /
//						   //|/        |/
//						   //1---------2
//	glm::vec3 verts[] = {
//		glm::vec3(-halfW, -halfW, -halfW),
//		glm::vec3(-halfW, -halfW,  halfW),
//		glm::vec3(halfW, -halfW,  halfW),
//		glm::vec3(halfW, -halfW, -halfW),
//		glm::vec3(-halfW,  halfW, -halfW),
//		glm::vec3(-halfW,  halfW,  halfW),
//		glm::vec3(halfW,  halfW,  halfW),
//		glm::vec3(halfW,  halfW, -halfW)
//	};
//	glm::vec3 norms[] = {
//		glm::vec3(0.f, -1.f,  0.f),
//		glm::vec3(0.f,  1.f,  0.f),
//		glm::vec3(-1.f,  0.f,  0.f),
//		glm::vec3(1.f,  0.f,  0.f),
//		glm::vec3(0.f,  0.f, -1.f),
//		glm::vec3(0.f,  0.f,  1.f)
//	};
//
//	glm::vec3 cubeVerts[] = {
//		verts[1], verts[0], verts[2], verts[3],
//		verts[5], verts[6], verts[4], verts[7],
//		verts[1], verts[5], verts[0], verts[4],
//		verts[2], verts[3], verts[6], verts[7],
//		verts[0], verts[4], verts[3], verts[7],
//		verts[1], verts[2], verts[5], verts[6]
//	};
//	glm::vec3 cubeNorms[] = {
//		norms[0], norms[0], norms[0], norms[0],
//		norms[1], norms[1], norms[1], norms[1],
//		norms[2], norms[2], norms[2], norms[2],
//		norms[3], norms[3], norms[3], norms[3],
//		norms[4], norms[4], norms[4], norms[4],
//		norms[5], norms[5], norms[5], norms[5]
//	};
//	GLubyte cubeIdx[] = {
//		0, 1, 2, 3, UCHAR_MAX,
//		4, 5, 6, 7, UCHAR_MAX,
//		8, 9, 10, 11, UCHAR_MAX,
//		12, 13, 14, 15, UCHAR_MAX,
//		16, 17, 18, 19, UCHAR_MAX,
//		20, 21, 22, 23, UCHAR_MAX
//	};
//
//
//	const char* cube_vertShader =
//		"#version 330\n\
//	in vec3 in_Position;\n\
//	in vec3 in_Normal;\n\
//	out vec4 vert_Normal;\n\
//	uniform mat4 objMat;\n\
//	uniform mat4 mv_Mat;\n\
//	uniform mat4 mvpMat;\n\
//	out vec3 pos;\n\
//	void main() {\n\
//		gl_Position = mvpMat * objMat * vec4(in_Position, 1.0);\n\
//		vert_Normal = mv_Mat * objMat * vec4(in_Normal, 0.0);\n\
//		pos = in_Position;\n\
//	}";
//
//	const char* cube_fragShader =
//		"#version 330\n\
//in vec4 vert_Normal;\n\
//out vec4 out_Color;\n\
//uniform mat4 mv_Mat;\n\
//uniform vec4 color;\n\
//uniform vec3 lightPosition;\n\
//uniform vec3 ambient;\n\
//in vec3 pos;\n\
//void main() {\n\
//	float dot_product = max(dot(normalize(lightPosition), normalize(vert_Normal.xyz)), 0.0);\n\
//	out_Color = dot_product * color * 4.0f;\n\
//}";
//
//
//	void setupCube() {
//		glGenVertexArrays(1, &cubeVao);
//		glBindVertexArray(cubeVao);
//		glGenBuffers(2, cubeVbo);
//
//		glBindBuffer(GL_ARRAY_BUFFER, cubeVbo[0]);
//		/*	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVerts), cubeVerts, GL_STATIC_DRAW);*/
//		glBufferData(GL_ARRAY_BUFFER, vertices3.size() * sizeof(vec3), &vertices3[0], GL_STATIC_DRAW);
//		glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, 0);
//		glEnableVertexAttribArray(0);
//
//		glBindBuffer(GL_ARRAY_BUFFER, cubeVbo[1]);
//		/*glBufferData(GL_ARRAY_BUFFER, sizeof(cubeNorms), cubeNorms, GL_STATIC_DRAW);*/
//		glBufferData(GL_ARRAY_BUFFER, normals3.size() * sizeof(vec3), &normals3[0], GL_STATIC_DRAW);
//		glVertexAttribPointer((GLuint)1, 3, GL_FLOAT, GL_FALSE, 0, 0);
//		glEnableVertexAttribArray(1);
//
//		//glPrimitiveRestartIndex(UCHAR_MAX);
//		//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeVbo[2]);
//		//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeIdx), cubeIdx, GL_STATIC_DRAW);
//
//		glBindVertexArray(0);
//		glBindBuffer(GL_ARRAY_BUFFER, 0);
//		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
//
//		cubeShaders[0] = compileShader(cube_vertShader, GL_VERTEX_SHADER, "cubeVert");
//		cubeShaders[1] = compileShader(cube_fragShader, GL_FRAGMENT_SHADER, "cubeFrag");
//
//		cubeProgram = glCreateProgram();
//		glAttachShader(cubeProgram, cubeShaders[0]);
//		glAttachShader(cubeProgram, cubeShaders[1]);
//		glBindAttribLocation(cubeProgram, 0, "in_Position");
//		glBindAttribLocation(cubeProgram, 1, "in_Normal");
//		glBindAttribLocation(cubeProgram, 2, "in_Color");
//		linkProgram(cubeProgram);
//	}
//	void cleanupCube() {
//		/*glDeleteBuffers(3, cubeVbo);*/
//		glDeleteVertexArrays(1, &cubeVao);
//
//		glDeleteProgram(cubeProgram);
//		glDeleteShader(cubeShaders[0]);
//		glDeleteShader(cubeShaders[1]);
//	}
//	void updateCube(const glm::mat4& transform) {
//		objMat = transform;
//	}
//	void drawCube() {
//		/*	glEnable(GL_PRIMITIVE_RESTART);*/
//		glBindVertexArray(cubeVao);
//		glUseProgram(cubeProgram);
//		glUniformMatrix4fv(glGetUniformLocation(cubeProgram, "objMat"), 1, GL_FALSE, glm::value_ptr(objMat));
//		glUniformMatrix4fv(glGetUniformLocation(cubeProgram, "mv_Mat"), 1, GL_FALSE, glm::value_ptr(RenderVars::_modelView));
//		glUniformMatrix4fv(glGetUniformLocation(cubeProgram, "mvpMat"), 1, GL_FALSE, glm::value_ptr(RenderVars::_MVP));
//		glUniform4f(glGetUniformLocation(cubeProgram, "color"), 0, .2f, 0, 0.f);
//		glUniform3f(glGetUniformLocation(cubeProgram, "lightPosition"), lightPosition.x, lightPosition.y, lightPosition.z);
//		glUniform4f(glGetUniformLocation(cubeProgram, "ambient"), 0.4f, 0.4f, 0.4f, 1.f);
//		/*glDrawElements(GL_TRIANGLE_STRIP, numVerts, GL_UNSIGNED_BYTE, 0);*/
//		glDrawArrays(GL_TRIANGLES, 0, 100000);
//
//		glUseProgram(0);
//		glBindVertexArray(0);
//		/*glDisable(GL_PRIMITIVE_RESTART);*/
//	}
//
//
//}
//
//namespace Wheel {
//	GLuint cubeVao;
//	GLuint cubeVbo[3];
//	GLuint cubeShaders[2];
//	GLuint cubeProgram;
//	glm::mat4 objMat = glm::mat4(1.f);
//
//	extern const float halfW = 0.5f;
//	int numVerts = 24 + 6; // 4 vertex/face * 6 faces + 6 PRIMITIVE RESTART
//
//						   //   4---------7
//						   //  /|        /|
//						   // / |       / |
//						   //5---------6  |
//						   //|  0------|--3
//						   //| /       | /
//						   //|/        |/
//						   //1---------2
//	glm::vec3 verts[] = {
//		glm::vec3(-halfW, -halfW, -halfW),
//		glm::vec3(-halfW, -halfW,  halfW),
//		glm::vec3(halfW, -halfW,  halfW),
//		glm::vec3(halfW, -halfW, -halfW),
//		glm::vec3(-halfW,  halfW, -halfW),
//		glm::vec3(-halfW,  halfW,  halfW),
//		glm::vec3(halfW,  halfW,  halfW),
//		glm::vec3(halfW,  halfW, -halfW)
//	};
//	glm::vec3 norms[] = {
//		glm::vec3(0.f, -1.f,  0.f),
//		glm::vec3(0.f,  1.f,  0.f),
//		glm::vec3(-1.f,  0.f,  0.f),
//		glm::vec3(1.f,  0.f,  0.f),
//		glm::vec3(0.f,  0.f, -1.f),
//		glm::vec3(0.f,  0.f,  1.f)
//	};
//
//	glm::vec3 cubeVerts[] = {
//		verts[1], verts[0], verts[2], verts[3],
//		verts[5], verts[6], verts[4], verts[7],
//		verts[1], verts[5], verts[0], verts[4],
//		verts[2], verts[3], verts[6], verts[7],
//		verts[0], verts[4], verts[3], verts[7],
//		verts[1], verts[2], verts[5], verts[6]
//	};
//	glm::vec3 cubeNorms[] = {
//		norms[0], norms[0], norms[0], norms[0],
//		norms[1], norms[1], norms[1], norms[1],
//		norms[2], norms[2], norms[2], norms[2],
//		norms[3], norms[3], norms[3], norms[3],
//		norms[4], norms[4], norms[4], norms[4],
//		norms[5], norms[5], norms[5], norms[5]
//	};
//	GLubyte cubeIdx[] = {
//		0, 1, 2, 3, UCHAR_MAX,
//		4, 5, 6, 7, UCHAR_MAX,
//		8, 9, 10, 11, UCHAR_MAX,
//		12, 13, 14, 15, UCHAR_MAX,
//		16, 17, 18, 19, UCHAR_MAX,
//		20, 21, 22, 23, UCHAR_MAX
//	};
//
//	//const char* cube_vertShader =
//	//	"#version 330\n\
//	//in vec3 in_Position;\n\
//	//in vec3 in_Normal;\n\
//	//in vec3 in_Color;\n\
//	//out vec3 vert_Position;\n\
//	//out vec3 vert_Color;\n\
//	//out vec3 vert_Normal;\n\
//	//uniform mat4 model;\n\
//	//uniform mat4 view;\n\
//	//uniform mat4 projection;\n\
//	//void main() {\n\
//	//	vert_Position = vec4(model*vec4(in_Position,1.f)).xyz;\n\
//	//	vert_Color = in_Color;\n\
//	//	vert_Normal = mat3(model) * vertex_Normal;\n\
//	//	gl_Position = projection * view * model * vec4(vert_Position,1.f);\n\
//	//}";
//
//	//const char* cube_fragShader =
//	//	"#version 330\n\
//	//	in vec4 vert_Normal;\n\
//	//	in vec3 vert_Color;\n\
//	//	in vec3 vert_Position;\n\
//	//	out vec4 out_Color;\n\
//	//	uniform mat4 view;\n\
//	//	uniform vec4 color;\n\
//	//	uniform vec3 lightPos;\n\
//	//	void main() {\n\
//	//		vec3 ambientLight = vec3(0.1f,0.1f,0.1f);\n\
//	//		vec3 posToLightDirVec = normalize(vert_Position - lightPos);\n\
//	//		vec3 diffuseColor = vec3(1.f,1.f,1.f);\n\
//	//		float diffuse = clamp(dot(posToLightDirVec, vert_Normal),0,1);\n\
//	//		vec3 diffuseFinal = diffuseColor * diffuse;\n\
//	//		out_Color = vec4(vert_Color,1.f) * (vec4(ambientLight,1.f) + vec4(diffuseFinal,1.f));\n\
//	//	}";
//
//
//	const char* cube_vertShader =
//		"#version 330\n\
//	in vec3 in_Position;\n\
//	in vec3 in_Normal;\n\
//	out vec4 vert_Normal;\n\
//	uniform mat4 objMat;\n\
//	uniform mat4 mv_Mat;\n\
//	uniform mat4 mvpMat;\n\
//	out vec3 pos;\n\
//	void main() {\n\
//		gl_Position = mvpMat * objMat * vec4(in_Position, 1.0);\n\
//		vert_Normal = mv_Mat * objMat * vec4(in_Normal, 0.0);\n\
//		pos = in_Position;\n\
//	}";
//
//	const char* cube_fragShader =
//		"#version 330\n\
//in vec4 vert_Normal;\n\
//out vec4 out_Color;\n\
//uniform mat4 mv_Mat;\n\
//uniform vec4 color;\n\
//uniform vec3 lightPosition;\n\
//uniform vec3 ambient;\n\
//in vec3 pos;\n\
//void main() {\n\
//	float dot_product = max(dot(normalize(lightPosition), normalize(vert_Normal.xyz)), 0.0);\n\
//	out_Color = dot_product * color * 4.0f;\n\
//}";
//
//
//	void setupCube() {
//		glGenVertexArrays(1, &cubeVao);
//		glBindVertexArray(cubeVao);
//		glGenBuffers(2, cubeVbo);
//
//		glBindBuffer(GL_ARRAY_BUFFER, cubeVbo[0]);
//		/*	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVerts), cubeVerts, GL_STATIC_DRAW);*/
//		glBufferData(GL_ARRAY_BUFFER, vertices4.size() * sizeof(vec3), &vertices4[0], GL_STATIC_DRAW);
//		glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, 0);
//		glEnableVertexAttribArray(0);
//
//		glBindBuffer(GL_ARRAY_BUFFER, cubeVbo[1]);
//		/*glBufferData(GL_ARRAY_BUFFER, sizeof(cubeNorms), cubeNorms, GL_STATIC_DRAW);*/
//		glBufferData(GL_ARRAY_BUFFER, normals4.size() * sizeof(vec3), &normals4[0], GL_STATIC_DRAW);
//		glVertexAttribPointer((GLuint)1, 3, GL_FLOAT, GL_FALSE, 0, 0);
//		glEnableVertexAttribArray(1);
//
//		//glPrimitiveRestartIndex(UCHAR_MAX);
//		//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeVbo[2]);
//		//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeIdx), cubeIdx, GL_STATIC_DRAW);
//
//		glBindVertexArray(0);
//		glBindBuffer(GL_ARRAY_BUFFER, 0);
//		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
//
//		cubeShaders[0] = compileShader(cube_vertShader, GL_VERTEX_SHADER, "cubeVert");
//		cubeShaders[1] = compileShader(cube_fragShader, GL_FRAGMENT_SHADER, "cubeFrag");
//
//		cubeProgram = glCreateProgram();
//		glAttachShader(cubeProgram, cubeShaders[0]);
//		glAttachShader(cubeProgram, cubeShaders[1]);
//		glBindAttribLocation(cubeProgram, 0, "in_Position");		
//		glBindAttribLocation(cubeProgram, 1, "in_Normal");
//		glBindAttribLocation(cubeProgram, 2, "in_Color"); 
//		linkProgram(cubeProgram);
//	}
//	void cleanupCube() {
//		/*glDeleteBuffers(3, cubeVbo);*/
//		glDeleteVertexArrays(1, &cubeVao);
//
//		glDeleteProgram(cubeProgram);
//		glDeleteShader(cubeShaders[0]);
//		glDeleteShader(cubeShaders[1]);
//	}
//	void updateCube(const glm::mat4& transform) {
//		objMat = transform;
//	}
//	void drawCube() {
//		/*	glEnable(GL_PRIMITIVE_RESTART);*/
//		glBindVertexArray(cubeVao);
//		glUseProgram(cubeProgram);
//		glUniformMatrix4fv(glGetUniformLocation(cubeProgram, "objMat"), 1, GL_FALSE, glm::value_ptr(objMat));
//		glUniformMatrix4fv(glGetUniformLocation(cubeProgram, "mv_Mat"), 1, GL_FALSE, glm::value_ptr(RenderVars::_modelView));
//		glUniformMatrix4fv(glGetUniformLocation(cubeProgram, "mvpMat"), 1, GL_FALSE, glm::value_ptr(RenderVars::_MVP));
//		glUniform4f(glGetUniformLocation(cubeProgram, "color"), .1f, .1f, .5f, 0.f);
//		glUniform3f(glGetUniformLocation(cubeProgram, "lightPosition"), lightPosition.x, lightPosition.y, lightPosition.z);
//		glUniform4f(glGetUniformLocation(cubeProgram, "ambient"), 0.4f, 0.4f, 0.4f, 1.f);
//		/*glDrawElements(GL_TRIANGLE_STRIP, numVerts, GL_UNSIGNED_BYTE, 0);*/
//		glDrawArrays(GL_TRIANGLES, 0, 100000);
//
//		glUseProgram(0);
//		glBindVertexArray(0);
//		/*glDisable(GL_PRIMITIVE_RESTART);*/
//	}
//
//
//}
//
//namespace Wheel_Feet {
//	GLuint cubeVao;
//	GLuint cubeVbo[3];
//	GLuint cubeShaders[2];
//	GLuint cubeProgram;
//	glm::mat4 objMat = glm::mat4(1.f);
//
//	extern const float halfW = 0.5f;
//	int numVerts = 24 + 6; // 4 vertex/face * 6 faces + 6 PRIMITIVE RESTART
//
//						   //   4---------7
//						   //  /|        /|
//						   // / |       / |
//						   //5---------6  |
//						   //|  0------|--3
//						   //| /       | /
//						   //|/        |/
//						   //1---------2
//	glm::vec3 verts[] = {
//		glm::vec3(-halfW, -halfW, -halfW),
//		glm::vec3(-halfW, -halfW,  halfW),
//		glm::vec3(halfW, -halfW,  halfW),
//		glm::vec3(halfW, -halfW, -halfW),
//		glm::vec3(-halfW,  halfW, -halfW),
//		glm::vec3(-halfW,  halfW,  halfW),
//		glm::vec3(halfW,  halfW,  halfW),
//		glm::vec3(halfW,  halfW, -halfW)
//	};
//	glm::vec3 norms[] = {
//		glm::vec3(0.f, -1.f,  0.f),
//		glm::vec3(0.f,  1.f,  0.f),
//		glm::vec3(-1.f,  0.f,  0.f),
//		glm::vec3(1.f,  0.f,  0.f),
//		glm::vec3(0.f,  0.f, -1.f),
//		glm::vec3(0.f,  0.f,  1.f)
//	};
//
//	glm::vec3 cubeVerts[] = {
//		verts[1], verts[0], verts[2], verts[3],
//		verts[5], verts[6], verts[4], verts[7],
//		verts[1], verts[5], verts[0], verts[4],
//		verts[2], verts[3], verts[6], verts[7],
//		verts[0], verts[4], verts[3], verts[7],
//		verts[1], verts[2], verts[5], verts[6]
//	};
//	glm::vec3 cubeNorms[] = {
//		norms[0], norms[0], norms[0], norms[0],
//		norms[1], norms[1], norms[1], norms[1],
//		norms[2], norms[2], norms[2], norms[2],
//		norms[3], norms[3], norms[3], norms[3],
//		norms[4], norms[4], norms[4], norms[4],
//		norms[5], norms[5], norms[5], norms[5]
//	};
//	GLubyte cubeIdx[] = {
//		0, 1, 2, 3, UCHAR_MAX,
//		4, 5, 6, 7, UCHAR_MAX,
//		8, 9, 10, 11, UCHAR_MAX,
//		12, 13, 14, 15, UCHAR_MAX,
//		16, 17, 18, 19, UCHAR_MAX,
//		20, 21, 22, 23, UCHAR_MAX
//	};
//
//	//const char* cube_vertShader =
//	//	"#version 330\n\
//		//in vec3 in_Position;\n\
//	//in vec3 in_Normal;\n\
//	//in vec3 in_Color;\n\
//	//out vec3 vert_Position;\n\
//	//out vec3 vert_Color;\n\
//	//out vec3 vert_Normal;\n\
//	//uniform mat4 model;\n\
//	//uniform mat4 view;\n\
//	//uniform mat4 projection;\n\
//	//void main() {\n\
//	//	vert_Position = vec4(model*vec4(in_Position,1.f)).xyz;\n\
//	//	vert_Color = in_Color;\n\
//	//	vert_Normal = mat3(model) * vertex_Normal;\n\
//	//	gl_Position = projection * view * model * vec4(vert_Position,1.f);\n\
//	//}";
//
////const char* cube_fragShader =
////	"#version 330\n\
//	//	in vec4 vert_Normal;\n\
//	//	in vec3 vert_Color;\n\
//	//	in vec3 vert_Position;\n\
//	//	out vec4 out_Color;\n\
//	//	uniform mat4 view;\n\
//	//	uniform vec4 color;\n\
//	//	uniform vec3 lightPos;\n\
//	//	void main() {\n\
//	//		vec3 ambientLight = vec3(0.1f,0.1f,0.1f);\n\
//	//		vec3 posToLightDirVec = normalize(vert_Position - lightPos);\n\
//	//		vec3 diffuseColor = vec3(1.f,1.f,1.f);\n\
//	//		float diffuse = clamp(dot(posToLightDirVec, vert_Normal),0,1);\n\
//	//		vec3 diffuseFinal = diffuseColor * diffuse;\n\
//	//		out_Color = vec4(vert_Color,1.f) * (vec4(ambientLight,1.f) + vec4(diffuseFinal,1.f));\n\
//	//	}";
//
//
//	const char* cube_vertShader =
//		"#version 330\n\
//	in vec3 in_Position;\n\
//	in vec3 in_Normal;\n\
//	out vec4 vert_Normal;\n\
//	uniform mat4 objMat;\n\
//	uniform mat4 mv_Mat;\n\
//	uniform mat4 mvpMat;\n\
//	out vec3 pos;\n\
//	void main() {\n\
//		gl_Position = mvpMat * objMat * vec4(in_Position, 1.0);\n\
//		vert_Normal = mv_Mat * objMat * vec4(in_Normal, 0.0);\n\
//		pos = in_Position;\n\
//	}";
//
//	const char* cube_fragShader =
//		"#version 330\n\
//in vec4 vert_Normal;\n\
//out vec4 out_Color;\n\
//uniform mat4 mv_Mat;\n\
//uniform vec4 color;\n\
//uniform vec3 lightPosition;\n\
//uniform vec3 ambient;\n\
//in vec3 pos;\n\
//void main() {\n\
//	float dot_product = max(dot(normalize(lightPosition), normalize(vert_Normal.xyz)), 0.0);\n\
//	out_Color = dot_product * color * 4.0f;\n\
//}";
//
//
//	void setupCube() {
//		glGenVertexArrays(1, &cubeVao);
//		glBindVertexArray(cubeVao);
//		glGenBuffers(2, cubeVbo);
//
//		glBindBuffer(GL_ARRAY_BUFFER, cubeVbo[0]);
//		/*	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVerts), cubeVerts, GL_STATIC_DRAW);*/
//		glBufferData(GL_ARRAY_BUFFER, vertices5.size() * sizeof(vec3), &vertices5[0], GL_STATIC_DRAW);
//		glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, 0);
//		glEnableVertexAttribArray(0);
//
//		glBindBuffer(GL_ARRAY_BUFFER, cubeVbo[1]);
//		/*glBufferData(GL_ARRAY_BUFFER, sizeof(cubeNorms), cubeNorms, GL_STATIC_DRAW);*/
//		glBufferData(GL_ARRAY_BUFFER, normals5.size() * sizeof(vec3), &normals5[0], GL_STATIC_DRAW);
//		glVertexAttribPointer((GLuint)1, 3, GL_FLOAT, GL_FALSE, 0, 0);
//		glEnableVertexAttribArray(1);
//
//		//glPrimitiveRestartIndex(UCHAR_MAX);
//		//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeVbo[2]);
//		//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeIdx), cubeIdx, GL_STATIC_DRAW);
//
//		glBindVertexArray(0);
//		glBindBuffer(GL_ARRAY_BUFFER, 0);
//		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
//
//		cubeShaders[0] = compileShader(cube_vertShader, GL_VERTEX_SHADER, "cubeVert");
//		cubeShaders[1] = compileShader(cube_fragShader, GL_FRAGMENT_SHADER, "cubeFrag");
//
//		cubeProgram = glCreateProgram();
//		glAttachShader(cubeProgram, cubeShaders[0]);
//		glAttachShader(cubeProgram, cubeShaders[1]);
//		glBindAttribLocation(cubeProgram, 0, "in_Position");
//		glBindAttribLocation(cubeProgram, 1, "in_Normal");
//		glBindAttribLocation(cubeProgram, 2, "in_Color");
//		linkProgram(cubeProgram);
//	}
//	void cleanupCube() {
//		/*glDeleteBuffers(3, cubeVbo);*/
//		glDeleteVertexArrays(1, &cubeVao);
//
//		glDeleteProgram(cubeProgram);
//		glDeleteShader(cubeShaders[0]);
//		glDeleteShader(cubeShaders[1]);
//	}
//	void updateCube(const glm::mat4& transform) {
//		objMat = transform;
//	}
//	void drawCube() {
//		/*	glEnable(GL_PRIMITIVE_RESTART);*/
//		glBindVertexArray(cubeVao);
//		glUseProgram(cubeProgram);
//		glUniformMatrix4fv(glGetUniformLocation(cubeProgram, "objMat"), 1, GL_FALSE, glm::value_ptr(objMat));
//		glUniformMatrix4fv(glGetUniformLocation(cubeProgram, "mv_Mat"), 1, GL_FALSE, glm::value_ptr(RenderVars::_modelView));
//		glUniformMatrix4fv(glGetUniformLocation(cubeProgram, "mvpMat"), 1, GL_FALSE, glm::value_ptr(RenderVars::_MVP));
//		glUniform4f(glGetUniformLocation(cubeProgram, "color"), 1.f, 0.f, 1.f, 0.f);
//		glUniform3f(glGetUniformLocation(cubeProgram, "lightPosition"), lightPosition.x, lightPosition.y, lightPosition.z);
//		glUniform4f(glGetUniformLocation(cubeProgram, "ambient"), 0.4f, 0.4f, 0.4f, 1.f);
//		/*glDrawElements(GL_TRIANGLE_STRIP, numVerts, GL_UNSIGNED_BYTE, 0);*/
//		glDrawArrays(GL_TRIANGLES, 0, 100000);
//
//		glUseProgram(0);
//		glBindVertexArray(0);
//		/*glDisable(GL_PRIMITIVE_RESTART);*/
//	}
//
//
//}

//Cube
//
//namespace Cube {
//	GLuint cubeVao;
//	GLuint cubeVbo[3];
//	GLuint cubeShaders[2];
//	GLuint cubeProgram;
//	glm::mat4 objMat = glm::mat4(1.f);
//
//	extern const float halfW = 0.5f;
//	int numVerts = 24 + 6; // 4 vertex/face * 6 faces + 6 PRIMITIVE RESTART
//
//						   //   4---------7
//						   //  /|        /|
//						   // / |       / |
//						   //5---------6  |
//						   //|  0------|--3
//						   //| /       | /
//						   //|/        |/
//						   //1---------2
//	glm::vec3 verts[] = {
//		glm::vec3(-halfW, -halfW, -halfW),
//		glm::vec3(-halfW, -halfW,  halfW),
//		glm::vec3(halfW, -halfW,  halfW),
//		glm::vec3(halfW, -halfW, -halfW),
//		glm::vec3(-halfW,  halfW, -halfW),
//		glm::vec3(-halfW,  halfW,  halfW),
//		glm::vec3(halfW,  halfW,  halfW),
//		glm::vec3(halfW,  halfW, -halfW)
//	};
//	glm::vec3 norms[] = {
//		glm::vec3(0.f, -1.f,  0.f),
//		glm::vec3(0.f,  1.f,  0.f),
//		glm::vec3(-1.f,  0.f,  0.f),
//		glm::vec3(1.f,  0.f,  0.f),
//		glm::vec3(0.f,  0.f, -1.f),
//		glm::vec3(0.f,  0.f,  1.f)
//	};
//
//	glm::vec3 cubeVerts[] = {
//		verts[1], verts[0], verts[2], verts[3],
//		verts[5], verts[6], verts[4], verts[7],
//		verts[1], verts[5], verts[0], verts[4],
//		verts[2], verts[3], verts[6], verts[7],
//		verts[0], verts[4], verts[3], verts[7],
//		verts[1], verts[2], verts[5], verts[6]
//	};
//	glm::vec3 cubeNorms[] = {
//		norms[0], norms[0], norms[0], norms[0],
//		norms[1], norms[1], norms[1], norms[1],
//		norms[2], norms[2], norms[2], norms[2],
//		norms[3], norms[3], norms[3], norms[3],
//		norms[4], norms[4], norms[4], norms[4],
//		norms[5], norms[5], norms[5], norms[5]
//	};
//	GLubyte cubeIdx[] = {
//		0, 1, 2, 3, UCHAR_MAX,
//		4, 5, 6, 7, UCHAR_MAX,
//		8, 9, 10, 11, UCHAR_MAX,
//		12, 13, 14, 15, UCHAR_MAX,
//		16, 17, 18, 19, UCHAR_MAX,
//		20, 21, 22, 23, UCHAR_MAX
//	};
//
//	const char* cube_vertShader =
//		"#version 330\n\
//	in vec3 in_Position;\n\
//	in vec3 in_Normal;\n\
//	out vec4 vert_Normal;\n\
//	uniform mat4 objMat;\n\
//	uniform mat4 mv_Mat;\n\
//	uniform mat4 mvpMat;\n\
//	void main() {\n\
//		gl_Position = mvpMat * objMat * vec4(in_Position, 1.0);\n\
//		vert_Normal = mv_Mat * objMat * vec4(in_Normal, 0.0);\n\
//	}";
//
//
//	const char* cube_fragShader =
//		"#version 330\n\
//in vec4 vert_Normal;\n\
//out vec4 out_Color;\n\
//uniform mat4 mv_Mat;\n\
//uniform vec4 color;\n\
//void main() {\n\
//	out_Color = vec4(color.xyz * dot(vert_Normal, mv_Mat*vec4(0.0, 1.0, 0.0, 0.0)) + color.xyz * 0.3, 1.0 );\n\
//}";
//	void setupCube() {
//		glGenVertexArrays(1, &cubeVao);
//		glBindVertexArray(cubeVao);
//		glGenBuffers(3, cubeVbo);
//
//		glBindBuffer(GL_ARRAY_BUFFER, cubeVbo[0]);
//		glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVerts), cubeVerts, GL_STATIC_DRAW);
//		glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, 0);
//		glEnableVertexAttribArray(0);
//
//		glBindBuffer(GL_ARRAY_BUFFER, cubeVbo[1]);
//		glBufferData(GL_ARRAY_BUFFER, sizeof(cubeNorms), cubeNorms, GL_STATIC_DRAW);
//		glVertexAttribPointer((GLuint)1, 3, GL_FLOAT, GL_FALSE, 0, 0);
//		glEnableVertexAttribArray(1);
//
//		glPrimitiveRestartIndex(UCHAR_MAX);
//		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeVbo[2]);
//		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeIdx), cubeIdx, GL_STATIC_DRAW);
//
//		glBindVertexArray(0);
//		glBindBuffer(GL_ARRAY_BUFFER, 0);
//		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
//
//		cubeShaders[0] = compileShader(cube_vertShader, GL_VERTEX_SHADER, "cubeVert");
//		cubeShaders[1] = compileShader(cube_fragShader, GL_FRAGMENT_SHADER, "cubeFrag");
//
//		cubeProgram = glCreateProgram();
//		glAttachShader(cubeProgram, cubeShaders[0]);
//		glAttachShader(cubeProgram, cubeShaders[1]);
//		glBindAttribLocation(cubeProgram, 0, "in_Position");
//		glBindAttribLocation(cubeProgram, 1, "in_Normal");
//		linkProgram(cubeProgram);
//	}
//	void cleanupCube() {
//		glDeleteBuffers(3, cubeVbo);
//		glDeleteVertexArrays(1, &cubeVao);
//
//		glDeleteProgram(cubeProgram);
//		glDeleteShader(cubeShaders[0]);
//		glDeleteShader(cubeShaders[1]);
//	}
//	void updateCube(const glm::mat4& transform) {
//		objMat = transform;
//	}
//	void drawCube() {
//		glEnable(GL_PRIMITIVE_RESTART);
//		glBindVertexArray(cubeVao);
//		glUseProgram(cubeProgram);
//		glUniformMatrix4fv(glGetUniformLocation(cubeProgram, "objMat"), 1, GL_FALSE, glm::value_ptr(objMat));
//		glUniformMatrix4fv(glGetUniformLocation(cubeProgram, "mv_Mat"), 1, GL_FALSE, glm::value_ptr(RenderVars::_modelView));
//		glUniformMatrix4fv(glGetUniformLocation(cubeProgram, "mvpMat"), 1, GL_FALSE, glm::value_ptr(RenderVars::_MVP));
//		glUniform4f(glGetUniformLocation(cubeProgram, "color"), 0.1f, 1.f, 1.f, 0.f);
//		glDrawElements(GL_TRIANGLE_STRIP, numVerts, GL_UNSIGNED_BYTE, 0);
//
//		glUseProgram(0);
//		glBindVertexArray(0);
//		glDisable(GL_PRIMITIVE_RESTART);
//	}
//
//
//}
//

////////////////////////////////////////////////// SPHERE
namespace Sphere {
	GLuint sphereVao;
	GLuint sphereVbo;
	GLuint sphereShaders[3];
	GLuint sphereProgram;
	float radius;

	const char* sphere_vertShader =
		"#version 330\n\
in vec3 in_Position;\n\
uniform mat4 mv_Mat;\n\
void main() {\n\
	gl_Position = mv_Mat * vec4(in_Position, 1.0);\n\
}";
	const char* sphere_geomShader =
		"#version 330\n\
layout(points) in;\n\
layout(triangle_strip, max_vertices = 4) out;\n\
out vec4 eyePos;\n\
out vec4 centerEyePos;\n\
uniform mat4 projMat;\n\
uniform float radius;\n\
vec4 nu_verts[4];\n\
void main() {\n\
	vec3 n = normalize(-gl_in[0].gl_Position.xyz);\n\
	vec3 up = vec3(0.0, 1.0, 0.0);\n\
	vec3 u = normalize(cross(up, n));\n\
	vec3 v = normalize(cross(n, u));\n\
	nu_verts[0] = vec4(-radius*u - radius*v, 0.0); \n\
	nu_verts[1] = vec4( radius*u - radius*v, 0.0); \n\
	nu_verts[2] = vec4(-radius*u + radius*v, 0.0); \n\
	nu_verts[3] = vec4( radius*u + radius*v, 0.0); \n\
	centerEyePos = gl_in[0].gl_Position;\n\
	for (int i = 0; i < 4; ++i) {\n\
		eyePos = (gl_in[0].gl_Position + nu_verts[i]);\n\
		gl_Position = projMat * eyePos;\n\
		EmitVertex();\n\
	}\n\
	EndPrimitive();\n\
}";
	const char* sphere_fragShader_flatColor =
		"#version 330\n\
in vec4 eyePos;\n\
in vec4 centerEyePos;\n\
out vec4 out_Color;\n\
uniform mat4 projMat;\n\
uniform mat4 mv_Mat;\n\
uniform vec4 color;\n\
uniform float radius;\n\
void main() {\n\
	vec4 diff = eyePos - centerEyePos;\n\
	float distSq2C = dot(diff, diff);\n\
	if (distSq2C > (radius*radius)) discard;\n\
	float h = sqrt(radius*radius - distSq2C);\n\
	vec4 nuEyePos = vec4(eyePos.xy, eyePos.z + h, 1.0);\n\
	vec4 nuPos = projMat * nuEyePos;\n\
	gl_FragDepth = ((nuPos.z / nuPos.w) + 1) * 0.5;\n\
	vec3 normal = normalize(nuEyePos - centerEyePos).xyz;\n\
	out_Color = vec4(color.xyz * dot(normal, (mv_Mat*vec4(0.0, 1.0, 0.0, 0.0)).xyz) + color.xyz * 0.3, 1.0 );\n\
}";

	bool shadersCreated = false;
	void createSphereShaderAndProgram() {
		if (shadersCreated) return;

		sphereShaders[0] = compileShader(sphere_vertShader, GL_VERTEX_SHADER, "sphereVert");
		sphereShaders[1] = compileShader(sphere_geomShader, GL_GEOMETRY_SHADER, "sphereGeom");
		sphereShaders[2] = compileShader(sphere_fragShader_flatColor, GL_FRAGMENT_SHADER, "sphereFrag");

		sphereProgram = glCreateProgram();
		glAttachShader(sphereProgram, sphereShaders[0]);
		glAttachShader(sphereProgram, sphereShaders[1]);
		glAttachShader(sphereProgram, sphereShaders[2]);
		glBindAttribLocation(sphereProgram, 0, "in_Position");
		linkProgram(sphereProgram);

		shadersCreated = true;
	}
	void cleanupSphereShaderAndProgram() {
		if (!shadersCreated) return;
		glDeleteProgram(sphereProgram);
		glDeleteShader(sphereShaders[0]);
		glDeleteShader(sphereShaders[1]);
		glDeleteShader(sphereShaders[2]);
		shadersCreated = false;
	}

	void setupSphere(glm::vec3 pos, float radius) {
		Sphere::radius = radius;
		glGenVertexArrays(1, &sphereVao);
		glBindVertexArray(sphereVao);
		glGenBuffers(1, &sphereVbo);

		glBindBuffer(GL_ARRAY_BUFFER, sphereVbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3, &pos, GL_DYNAMIC_DRAW);
		glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(0);

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		createSphereShaderAndProgram();
	}
	void cleanupSphere() {
		glDeleteBuffers(1, &sphereVbo);
		glDeleteVertexArrays(1, &sphereVao);

		cleanupSphereShaderAndProgram();
	}
	void updateSphere(glm::vec3 pos, float radius) {
		glBindBuffer(GL_ARRAY_BUFFER, sphereVbo);
		float* buff = (float*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
		buff[0] = pos.x;
		buff[1] = pos.y;
		buff[2] = pos.z;
		glUnmapBuffer(GL_ARRAY_BUFFER);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		Sphere::radius = radius;
	}
	void drawSphere() {
		glBindVertexArray(sphereVao);
		glUseProgram(sphereProgram);
		glUniformMatrix4fv(glGetUniformLocation(sphereProgram, "mvpMat"), 1, GL_FALSE, glm::value_ptr(RV::_MVP));
		glUniformMatrix4fv(glGetUniformLocation(sphereProgram, "mv_Mat"), 1, GL_FALSE, glm::value_ptr(RV::_modelView));
		glUniformMatrix4fv(glGetUniformLocation(sphereProgram, "projMat"), 1, GL_FALSE, glm::value_ptr(RV::_projection));
		glUniform4f(glGetUniformLocation(sphereProgram, "color"), 0.6f, 0.1f, 0.1f, 1.f);
		glUniform1f(glGetUniformLocation(sphereProgram, "radius"), Sphere::radius);
		glDrawArrays(GL_POINTS, 0, 1);

		glUseProgram(0);
		glBindVertexArray(0);
	}
}


////////////////////////////////////////////////// MyModel
namespace Cabeza_Trump {
	GLuint modelVao;
	GLuint modelVbo[3];
	GLuint modelShaders[2];
	GLuint modelProgram;
	glm::mat4 objMat = glm::mat4(1.f);

	const char* model_vertShader =
		"#version 330\n\
	in vec3 in_Position;\n\
	in vec3 in_Normal;\n\
	uniform vec3 lPos;\n\
	uniform vec3 lPos2;\n\
	out vec3 lDir;\n\
	out vec3 lDir2;\n\
	out vec4 vert_Normal;\n\
	uniform mat4 objMat;\n\
	uniform mat4 mv_Mat;\n\
	uniform mat4 mvpMat;\n\
	void main() {\n\
		gl_Position = mvpMat * objMat * vec4(in_Position, 1.0);\n\
		vert_Normal = mv_Mat * objMat * vec4(in_Normal, 0.0);\n\
		lDir = normalize(lPos - gl_Position.xyz );\n\
		lDir2 = normalize(lPos2 - gl_Position.xyz );\n\
	}";


	const char* model_fragShader =
		"#version 330\n\
in vec4 vert_Normal;\n\
in vec3 lDir;\n\
in vec3 lDir2;\n\
out vec4 out_Color;\n\
uniform mat4 mv_Mat;\n\
uniform vec4 color;\n\
uniform vec4 color2;\n\
uniform vec4 ambient;\n\
void main() {\n\
		float ToonShading = 0.0f;\n\
		float ToonShading2 = 0.0f;\n\
		ToonShading = dot(lDir,normalize(vert_Normal.xyz));\n\
		if(ToonShading < 0.2) ToonShading = 0.0f; \n\
		if(ToonShading >= 0.2 && ToonShading < 0.4) ToonShading = 0.2f; \n\
		if(ToonShading >= 0.4 && ToonShading < 0.5) ToonShading = 0.4f; \n\
		if(ToonShading >= 0.5f) ToonShading = 1.0f; \n\
		ToonShading2 = dot(lDir,normalize(vert_Normal.xyz));\n\
	out_Color = vec4(ambient.rgb, 1.0) + vec4(color.xyz * dot(vert_Normal, mv_Mat*vec4(	ToonShading * 64 , ToonShading * 64 , ToonShading* 64 , 0.0)) , 1.0 )\n\
	+ vec4(color2.xyz * dot(vert_Normal, mv_Mat*vec4(ToonShading2 * 64, ToonShading2* 64 , ToonShading2* 64  , 0.0)) , 1.0 );\n\
}";


	/*
	if (ToonShading2 < 0.2) ToonShading2 = 0; \n\
	if (ToonShading2 >= 0.2 && ToonShading2 < 0.4) ToonShading2 = 0.2f; \n\
	if (ToonShading2 >= 0.4 && ToonShading2 < 0.5) ToonShading2 = 0.4f; \n\
	if (ToonShading2 >= 0.5f) ToonShading2 = 1; \n\
	lDir.x
	lDir.y
	lDir.z

	float ToonShading = 0.0f;\n\
	ToonShading = dot(lDir,normalize(vert_Normal.xyz));\n\
	ToonShading2 = dot(lDir,normalize(vert_Normal.xyz));\n\
	if(ToonShading < 0.2) ToonShading = 0.0f; \n\
	if(ToonShading >= 0.2 && ToonShading < 0.4) ToonShading = 0.2f; \n\
	if(ToonShading >= 0.4 && ToonShading < 0.5) ToonShading = 0.4f; \n\
	if(ToonShading >= 0.5f) ToonShading = 1.0f; \n\

	*/
	void setupModel() {
		glGenVertexArrays(1, &modelVao);
		glBindVertexArray(modelVao);
		glGenBuffers(3, modelVbo);

		glBindBuffer(GL_ARRAY_BUFFER, modelVbo[0]);

		glBufferData(GL_ARRAY_BUFFER, verticesCabeza.size() * sizeof(glm::vec3), &verticesCabeza[0], GL_STATIC_DRAW);
		glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, modelVbo[1]);

		glBufferData(GL_ARRAY_BUFFER, normalsCabeza.size() * sizeof(glm::vec3), &normalsCabeza[0], GL_STATIC_DRAW);
		glVertexAttribPointer((GLuint)1, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(1);

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		modelShaders[0] = compileShader(model_vertShader, GL_VERTEX_SHADER, "cubeVert");
		modelShaders[1] = compileShader(model_fragShader, GL_FRAGMENT_SHADER, "cubeFrag");

		modelProgram = glCreateProgram();
		glAttachShader(modelProgram, modelShaders[0]);
		glAttachShader(modelProgram, modelShaders[1]);
		glBindAttribLocation(modelProgram, 0, "in_Position");
		glBindAttribLocation(modelProgram, 1, "in_Normal");
		linkProgram(modelProgram);
	}
	void cleanupModel() {

		glDeleteBuffers(2, modelVbo);
		glDeleteVertexArrays(1, &modelVao);

		glDeleteProgram(modelProgram);
		glDeleteShader(modelShaders[0]);
		glDeleteShader(modelShaders[1]);
	}
	void updateModel(const glm::mat4& transform) {
		objMat = transform;
	}
	void drawModel() {

		glBindVertexArray(modelVao);
		glUseProgram(modelProgram);
		glUniformMatrix4fv(glGetUniformLocation(modelProgram, "objMat"), 1, GL_FALSE, glm::value_ptr(objMat));
		glUniformMatrix4fv(glGetUniformLocation(modelProgram, "mv_Mat"), 1, GL_FALSE, glm::value_ptr(RenderVars::_modelView));
		glUniformMatrix4fv(glGetUniformLocation(modelProgram, "mvpMat"), 1, GL_FALSE, glm::value_ptr(RenderVars::_MVP));
		glUniform3f(glGetUniformLocation(modelProgram, "lPos"), lightPosition.x, lightPosition.y, lightPosition.z);
		glUniform3f(glGetUniformLocation(modelProgram, "lPos2"), lightPosition2.x, lightPosition2.y, lightPosition2.z);
		glUniform4f(glGetUniformLocation(modelProgram, "color"), Color.x, Color.y, Color.z, 0.f);
		glUniform4f(glGetUniformLocation(modelProgram, "color2"), Color2.x, Color2.y, Color2.z, 0.f);
		glUniform4f(glGetUniformLocation(modelProgram, "ambient"), 0.0f, 0.0f, 0.2f, 0.f);

		//3 exercises OPTIMIZACION
		glDrawArrays(GL_TRIANGLES, 0, 10000);
		//glDrawArraysInstanced(GL_TRIANGLES, 0, 10000, 4);
		//glMultiDrawArraysIndirect(GL_TRIANGLES, 0, 4, 0);

		glUseProgram(0);
		glBindVertexArray(0);

	}


}



