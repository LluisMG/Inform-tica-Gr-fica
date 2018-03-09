#include <GL\glew.h>
#include <glm\gtc\type_ptr.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <imgui\imgui.h>
#include <imgui\imgui_impl_sdl_gl3.h>

#include <cstdio>
#include <cassert>

#include "GL_framework.h"

//AA1 

//Preparar un mon amb objectes

//Ex 1) Camara en Ortogonal i moviment lateral

//Ex 2A) Camara en Perspectiva i fer un Zoom
//Ex 2B) Camara en Perspectiva i augmentar en FOV

//Ex 3) Camara en Perspectiva i fer Zoom i canvi de FOV a la vegada, fent un "Dolly"


//Variable per moure en l'eix X en l'ejercici

float movement = 0.f;
int exercici = 1;

int _width, _height;

namespace ImGui {
	void Render();
}
namespace Box {
	void mySetupCube();
	void myCleanupCube();
	void myDrawCube();
}
namespace Axis {
	void mySetupAxis();
	void myCleanupAxis();
	void myDrawAxis();
}

namespace Cube {
	void mySetupCube();
	void myCleanupCube();
	void myUpdateCube(const glm::mat4& transform);
	void myDrawCube();
	void myDraw2Cubes(double currentTime);
}
namespace RenderVars {
	float FOV = glm::radians(65.f);
	const float zNear = 1.f;
	const float zFar = 50.f;

	glm::mat4 my_projection;
	glm::mat4 my_modelView;
	glm::mat4 my_MVP;
	glm::mat4 my_inv_modelview;
	glm::vec4 my_cameraPoint;

	struct prevMouse {
		float lastx, lasty;
		MouseEvent::Button button = MouseEvent::Button::None;
		bool waspressed = false;
	} myPrevMouse;

	float myPanv[3] = { 0.f, -5.f, -15.f };
	float myRota[2] = { 0.f, 0.f };
}
namespace RV = RenderVars;

void myInitCode(int width, int height) {
	glViewport(0, 0, width, height);
	glClearColor(0.2f, 0.2f, 0.2f, 1.f);
	glClearDepth(1.f);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	//Gamara en Perspectiva
	//RV::my_projection = glm::perspective(RV::FOV, (float)width / (float)height, RV::zNear, RV::zFar);

	//Camara Ortogonal
	if (exercici == 1) {
		float scale = 100.f;
		RV::my_projection = glm::ortho(-(float)width/scale,(float)width/scale, -(float)height/scale,(float)height/scale, RV::zNear, RV::zFar);
	} else {
		RV::my_projection = glm::perspective(RV::FOV, (float)width / (float)height, RV::zNear, RV::zFar);
	}
	_width = width;
	_height = height;

	// Setup shaders & geometry
	Box::mySetupCube();
	Axis::mySetupAxis();
	Cube::mySetupCube();
}

void myRenderCode(double currentTime) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	RV::my_modelView = glm::mat4(1.f);
	RV::my_modelView = glm::translate(RV::my_modelView, glm::vec3(RV::myPanv[0], RV::myPanv[1], RV::myPanv[2]));
	RV::my_modelView = glm::rotate(RV::my_modelView, RV::myRota[1], glm::vec3(1.f, 0.f, 0.f));
	RV::my_modelView = glm::rotate(RV::my_modelView, RV::myRota[0], glm::vec3(0.f, 1.f, 0.f));

	//RV::my_modelView = glm::lookAt(glm::vec3(-3.f, 5.f, 15.f), glm::vec3(move, 1.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	RV::my_MVP = RV::my_projection * RV::my_modelView;

	// render code
	Box::myDrawCube();
	Axis::myDrawAxis();

	//Movimeinto de Camara Aleatorio
	//RV::panv[0] = sin(currentTime)*10;
	//RV::panv[1] = -cos(currentTime);

	if (exercici == 1) {
		//Moure la camara en l'eix X
		movement += .1f;
		if (movement > 10.f) {
			movement -= 10.f;
		}
	}
	
	RV::myPanv[0] = movement;

	//MyFirstShader::myRenderCode(currentTime);

	//Cube::drawCube();
	Cube::myDraw2Cubes(currentTime);

	ImGuiIO& io = ImGui::GetIO();
	if (!io.WantCaptureKeyboard) {
		if (ImGui::IsKeyDown('1')) {
			exercici = 1;
			movement = 0;
			myInitCode(_width, _height);
		}
		if (ImGui::IsKeyDown('2')) {
			exercici = 2;
			movement = 0;
			myInitCode(_width, _height);
		}
		if (ImGui::IsKeyDown('3')) {

		}

		if (exercici == 2 && ImGui::IsKeyDown('z')) { //Zoom in
			RV::myPanv[2]++;
		}
		if (exercici == 2 && ImGui::IsKeyDown('x')) { //Zoom out
			RV::myPanv[2]--;
		}
		if (exercici == 2 && ImGui::IsKeyDown('f')) { //FOV ++
			RV::FOV += glm::radians(1.f);;
			RV::my_projection = glm::perspective(RV::FOV, (float)_width / (float)_height, RV::zNear, RV::zFar);
		}
		if (exercici == 2 && ImGui::IsKeyDown('g')) { //FOV --
			RV::FOV -= glm::radians(1.f);;
			RV::my_projection = glm::perspective(RV::FOV, (float)_width / (float)_height, RV::zNear, RV::zFar);
		}
	}

	ImGui::Render();
}

void myCleanupCode() {
	Box::myCleanupCube();
	Axis::myCleanupAxis();
	Cube::myCleanupCube();
}


//////////////////////////////////// COMPILE AND LINK
GLuint myCompileShader(const char* shaderStr, GLenum shaderType, const char* name = "") {
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
void myLinkProgram(GLuint program) {
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

////////////////////////////////////////////////// BOX
namespace Box {
	GLuint myCubeVao;
	GLuint myCubeVbo[2];
	GLuint myCubeShaders[2];
	GLuint myCubeProgram;

	float myCubeVerts[] = {
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
	GLubyte myCubeIdx[] = {
		1, 0, 2, 3, // Floor - TriangleStrip
		0, 1, 5, 4, // Wall - Lines
		1, 2, 6, 5, // Wall - Lines
		2, 3, 7, 6, // Wall - Lines
		3, 0, 4, 7  // Wall - Lines
	};

	const char* myVertShader_xform =
		"#version 330\n\
in vec3 in_Position;\n\
uniform mat4 mvpMat;\n\
void main() {\n\
	gl_Position = mvpMat * vec4(in_Position, 1.0);\n\
}";
	const char* myFragShader_flatColor =
		"#version 330\n\
out vec4 out_Color;\n\
uniform vec4 color;\n\
void main() {\n\
	out_Color = color;\n\
}";

	void mySetupCube() {
		glGenVertexArrays(1, &myCubeVao);
		glBindVertexArray(myCubeVao);
		glGenBuffers(2, myCubeVbo);

		glBindBuffer(GL_ARRAY_BUFFER, myCubeVbo[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 24, myCubeVerts, GL_STATIC_DRAW);
		glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, myCubeVbo[1]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLubyte) * 20, myCubeIdx, GL_STATIC_DRAW);

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		myCubeShaders[0] = myCompileShader(myVertShader_xform, GL_VERTEX_SHADER, "cubeVert");
		myCubeShaders[1] = myCompileShader(myFragShader_flatColor, GL_FRAGMENT_SHADER, "cubeFrag");

		myCubeProgram = glCreateProgram();
		glAttachShader(myCubeProgram, myCubeShaders[0]);
		glAttachShader(myCubeProgram, myCubeShaders[1]);
		glBindAttribLocation(myCubeProgram, 0, "in_Position");
		myLinkProgram(myCubeProgram);
	}
	void myCleanupCube() {
		glDeleteBuffers(2, myCubeVbo);
		glDeleteVertexArrays(1, &myCubeVao);

		glDeleteProgram(myCubeProgram);
		glDeleteShader(myCubeShaders[0]);
		glDeleteShader(myCubeShaders[1]);
	}
	void myDrawCube() {
		glBindVertexArray(myCubeVao);
		glUseProgram(myCubeProgram);
		glUniformMatrix4fv(glGetUniformLocation(myCubeProgram, "mvpMat"), 1, GL_FALSE, glm::value_ptr(RV::my_MVP));
		// FLOOR
		glUniform4f(glGetUniformLocation(myCubeProgram, "color"), 0.6f, 0.6f, 0.6f, 1.f);
		glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_BYTE, 0);
		// WALLS
		glUniform4f(glGetUniformLocation(myCubeProgram, "color"), 0.f, 0.f, 0.f, 1.f);
		glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_BYTE, (void*)(sizeof(GLubyte) * 4));
		glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_BYTE, (void*)(sizeof(GLubyte) * 8));
		glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_BYTE, (void*)(sizeof(GLubyte) * 12));
		glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_BYTE, (void*)(sizeof(GLubyte) * 16));

		glUseProgram(0);
		glBindVertexArray(0);
	}
}

////////////////////////////////////////////////// AXIS
namespace Axis {
	GLuint myAxisVao;
	GLuint myAxisVbo[3];
	GLuint myAxisShader[2];
	GLuint myAxisProgram;

	float myAxisVerts[] = {
		0.0, 0.0, 0.0,
		1.0, 0.0, 0.0,
		0.0, 0.0, 0.0,
		0.0, 1.0, 0.0,
		0.0, 0.0, 0.0,
		0.0, 0.0, 1.0
	};
	float myAxisColors[] = {
		1.0, 0.0, 0.0, 1.0,
		1.0, 0.0, 0.0, 1.0,
		0.0, 1.0, 0.0, 1.0,
		0.0, 1.0, 0.0, 1.0,
		0.0, 0.0, 1.0, 1.0,
		0.0, 0.0, 1.0, 1.0
	};
	GLubyte myAxisIdx[] = {
		0, 1,
		2, 3,
		4, 5
	};
	const char* myAxis_vertShader =
		"#version 330\n\
in vec3 in_Position;\n\
in vec4 in_Color;\n\
out vec4 vert_color;\n\
uniform mat4 mvpMat;\n\
void main() {\n\
	vert_color = in_Color;\n\
	gl_Position = mvpMat * vec4(in_Position, 1.0);\n\
}";
	const char* myAxis_fragShader =
		"#version 330\n\
in vec4 vert_color;\n\
out vec4 out_Color;\n\
void main() {\n\
	out_Color = vert_color;\n\
}";

	void mySetupAxis() {
		glGenVertexArrays(1, &myAxisVao);
		glBindVertexArray(myAxisVao);
		glGenBuffers(3, myAxisVbo);

		glBindBuffer(GL_ARRAY_BUFFER, myAxisVbo[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 24, myAxisVerts, GL_STATIC_DRAW);
		glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, myAxisVbo[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 24, myAxisColors, GL_STATIC_DRAW);
		glVertexAttribPointer((GLuint)1, 4, GL_FLOAT, false, 0, 0);
		glEnableVertexAttribArray(1);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, myAxisVbo[2]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLubyte) * 6, myAxisIdx, GL_STATIC_DRAW);

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		myAxisShader[0] = myCompileShader(myAxis_vertShader, GL_VERTEX_SHADER, "AxisVert");
		myAxisShader[1] = myCompileShader(myAxis_fragShader, GL_FRAGMENT_SHADER, "AxisFrag");

		myAxisProgram = glCreateProgram();
		glAttachShader(myAxisProgram, myAxisShader[0]);
		glAttachShader(myAxisProgram, myAxisShader[1]);
		glBindAttribLocation(myAxisProgram, 0, "in_Position");
		glBindAttribLocation(myAxisProgram, 1, "in_Color");
		myLinkProgram(myAxisProgram);
	}
	void myCleanupAxis() {
		glDeleteBuffers(3, myAxisVbo);
		glDeleteVertexArrays(1, &myAxisVao);

		glDeleteProgram(myAxisProgram);
		glDeleteShader(myAxisShader[0]);
		glDeleteShader(myAxisShader[1]);
	}
	void myDrawAxis() {
		glBindVertexArray(myAxisVao);
		glUseProgram(myAxisProgram);
		glUniformMatrix4fv(glGetUniformLocation(myAxisProgram, "mvpMat"), 1, GL_FALSE, glm::value_ptr(RV::my_MVP));
		glDrawElements(GL_LINES, 6, GL_UNSIGNED_BYTE, 0);

		glUseProgram(0);
		glBindVertexArray(0);
	}
}

////////////////////////////////////////////////// CUBE
namespace Cube {
	GLuint myCubeVao;
	GLuint myCubeVbo[3];
	GLuint myCubeShaders[2];
	GLuint myCubeProgram;
	glm::mat4 myObjMat = glm::mat4(1.f);
	glm::mat4 myObjMat2Cubes = glm::mat4(1.f);

	extern const float myHalfW = 0.5f;
	int myNumVerts = 24 + 6; // 4 vertex/face * 6 faces + 6 PRIMITIVE RESTART

						   //   4---------7
						   //  /|        /|
						   // / |       / |
						   //5---------6  |
						   //|  0------|--3
						   //| /       | /
						   //|/        |/
						   //1---------2
	glm::vec3 myVerts[] = {
		glm::vec3(-myHalfW, -myHalfW, -myHalfW),
		glm::vec3(-myHalfW, -myHalfW,  myHalfW),
		glm::vec3(myHalfW, -myHalfW,  myHalfW),
		glm::vec3(myHalfW, -myHalfW, -myHalfW),
		glm::vec3(-myHalfW,  myHalfW, -myHalfW),
		glm::vec3(-myHalfW,  myHalfW,  myHalfW),
		glm::vec3(myHalfW,  myHalfW,  myHalfW),
		glm::vec3(myHalfW,  myHalfW, -myHalfW)
	};
	glm::vec3 myNorms[] = {
		glm::vec3(0.f, -1.f,  0.f),
		glm::vec3(0.f,  1.f,  0.f),
		glm::vec3(-1.f,  0.f,  0.f),
		glm::vec3(1.f,  0.f,  0.f),
		glm::vec3(0.f,  0.f, -1.f),
		glm::vec3(0.f,  0.f,  1.f)
	};

	glm::vec3 myCubeVerts[] = {
		myVerts[1], myVerts[0], myVerts[2], myVerts[3],
		myVerts[5], myVerts[6], myVerts[4], myVerts[7],
		myVerts[1], myVerts[5], myVerts[0], myVerts[4],
		myVerts[2], myVerts[3], myVerts[6], myVerts[7],
		myVerts[0], myVerts[4], myVerts[3], myVerts[7],
		myVerts[1], myVerts[2], myVerts[5], myVerts[6]
	};
	glm::vec3 myCubeNorms[] = {
		myNorms[0], myNorms[0], myNorms[0], myNorms[0],
		myNorms[1], myNorms[1], myNorms[1], myNorms[1],
		myNorms[2], myNorms[2], myNorms[2], myNorms[2],
		myNorms[3], myNorms[3], myNorms[3], myNorms[3],
		myNorms[4], myNorms[4], myNorms[4], myNorms[4],
		myNorms[5], myNorms[5], myNorms[5], myNorms[5]
	};
	GLubyte myCubeIdx[] = {
		0, 1, 2, 3, UCHAR_MAX,
		4, 5, 6, 7, UCHAR_MAX,
		8, 9, 10, 11, UCHAR_MAX,
		12, 13, 14, 15, UCHAR_MAX,
		16, 17, 18, 19, UCHAR_MAX,
		20, 21, 22, 23, UCHAR_MAX
	};

	const char* myCube_vertShader =
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


	const char* myCube_fragShader =
		"#version 330\n\
in vec4 vert_Normal;\n\
out vec4 out_Color;\n\
uniform mat4 mv_Mat;\n\
uniform vec4 color;\n\
void main() {\n\
	out_Color = vec4(color.xyz * dot(vert_Normal, mv_Mat*vec4(0.0, 1.0, 0.0, 0.0)) + color.xyz * 0.3, 1.0 );\n\
}";

	void mySetupCube() {
		glGenVertexArrays(1, &myCubeVao);
		glBindVertexArray(myCubeVao);
		glGenBuffers(3, myCubeVbo);

		glBindBuffer(GL_ARRAY_BUFFER, myCubeVbo[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(myCubeVerts), myCubeVerts, GL_STATIC_DRAW);
		glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, myCubeVbo[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(myCubeNorms), myCubeNorms, GL_STATIC_DRAW);
		glVertexAttribPointer((GLuint)1, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(1);

		glPrimitiveRestartIndex(UCHAR_MAX);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, myCubeVbo[2]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(myCubeIdx), myCubeIdx, GL_STATIC_DRAW);

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		myCubeShaders[0] = myCompileShader(myCube_vertShader, GL_VERTEX_SHADER, "cubeVert");
		myCubeShaders[1] = myCompileShader(myCube_fragShader, GL_FRAGMENT_SHADER, "cubeFrag");

		myCubeProgram = glCreateProgram();
		glAttachShader(myCubeProgram, myCubeShaders[0]);
		glAttachShader(myCubeProgram, myCubeShaders[1]);
		glBindAttribLocation(myCubeProgram, 0, "in_Position");
		glBindAttribLocation(myCubeProgram, 1, "in_Normal");
		myLinkProgram(myCubeProgram);
	}
	void myCleanupCube() {
		glDeleteBuffers(3, myCubeVbo);
		glDeleteVertexArrays(1, &myCubeVao);

		glDeleteProgram(myCubeProgram);
		glDeleteShader(myCubeShaders[0]);
		glDeleteShader(myCubeShaders[1]);
	}
	void myUpdateCube(const glm::mat4& transform) {
		myObjMat = transform;
	}
	void myDrawCube() {
		glEnable(GL_PRIMITIVE_RESTART);
		glBindVertexArray(myCubeVao);
		glUseProgram(myCubeProgram);
		glUniformMatrix4fv(glGetUniformLocation(myCubeProgram, "objMat"), 1, GL_FALSE, glm::value_ptr(myObjMat));
		glUniformMatrix4fv(glGetUniformLocation(myCubeProgram, "mv_Mat"), 1, GL_FALSE, glm::value_ptr(RenderVars::my_modelView));
		glUniformMatrix4fv(glGetUniformLocation(myCubeProgram, "mvpMat"), 1, GL_FALSE, glm::value_ptr(RenderVars::my_MVP));
		glUniform4f(glGetUniformLocation(myCubeProgram, "color"), 0.1f, 1.f, 1.f, 0.f);
		glDrawElements(GL_TRIANGLE_STRIP, myNumVerts, GL_UNSIGNED_BYTE, 0);

		glUseProgram(0);
		glBindVertexArray(0);
		glDisable(GL_PRIMITIVE_RESTART);
	}
	void myDraw2Cubes(double currentTime) {

		glEnable(GL_PRIMITIVE_RESTART);
		glBindVertexArray(myCubeVao);
		glUseProgram(myCubeProgram);

		if (exercici == 1) {

			//CUB VERMELL
			glm::mat4 translacio_cub1 = glm::translate(glm::mat4(), glm::vec3(-2.0f, 4.f, 0.f));
			myObjMat = translacio_cub1;

			glUniformMatrix4fv(glGetUniformLocation(myCubeProgram, "objMat"), 1, GL_FALSE, glm::value_ptr(myObjMat));
			glUniformMatrix4fv(glGetUniformLocation(myCubeProgram, "mv_Mat"), 1, GL_FALSE, glm::value_ptr(RenderVars::my_modelView));
			glUniformMatrix4fv(glGetUniformLocation(myCubeProgram, "mvpMat"), 1, GL_FALSE, glm::value_ptr(RenderVars::my_MVP));
			glUniform4f(glGetUniformLocation(myCubeProgram, "color"), 1.f, 0.f, 0.f, 0.f);
			glDrawElements(GL_TRIANGLE_STRIP, myNumVerts, GL_UNSIGNED_BYTE, 0);

			//glm::mat4 escalar = glm::scale(glm::mat4(), glm::vec3(cos(currentTime)*.5f + 1.5f, cos(currentTime)*.5f + 1.5f, cos(currentTime)*.5f + 1.5f));
			//glm::mat4 rotacio = glm::rotate(glm::mat4(), float(currentTime) * 10, glm::vec3(0.f, 1.f, 0.f));
			//translacio = glm::translate(glm::mat4(), glm::vec3(2.f, cos(currentTime)*.5f + 4.5f, 0.f));

			//CUB QUE CANVIA DE COLOR
			glm::mat4 translacio_cub2 = glm::translate(glm::mat4(), glm::vec3(0.0f, 3.f, 0.f));
			glm::mat4 escalar_cub2 = glm::scale(glm::mat4(), glm::vec3(2.0f, 2.0f, 2.0f));
			myObjMat = escalar_cub2 * translacio_cub2;

			glUniformMatrix4fv(glGetUniformLocation(myCubeProgram, "objMat"), 1, GL_FALSE, glm::value_ptr(myObjMat));
			glUniform4f(glGetUniformLocation(myCubeProgram, "color"), cos(currentTime)*.5f + .5f, 0.f, 1.f, 1.f);
			glDrawElements(GL_TRIANGLE_STRIP, myNumVerts, GL_UNSIGNED_BYTE, 0);

			//CUB VERD QUE ES MOU
			glm::mat4 translacio_cub3 = glm::translate(glm::mat4(), glm::vec3(-3.0f, 9.f, 0.f));
			glm::mat4 escalar_cub3 = glm::scale(glm::mat4(), glm::vec3(0.5f, 0.5f, 0.5f));
			glm::mat4 rotacio_cub3 = glm::rotate(glm::mat4(), float(currentTime) * 10, glm::vec3(0.f, 1.f, 0.f));
			myObjMat = escalar_cub3 * rotacio_cub3 * translacio_cub3;

			glUniformMatrix4fv(glGetUniformLocation(myCubeProgram, "objMat"), 1, GL_FALSE, glm::value_ptr(myObjMat));
			glUniform4f(glGetUniformLocation(myCubeProgram, "color"), 0.0f, 1.0f, 0.0f, 1.0f);
			glDrawElements(GL_TRIANGLE_STRIP, myNumVerts, GL_UNSIGNED_BYTE, 0);

			//CUB FORA DE PANTALLA
			glm::mat4 translacio_cub4 = glm::translate(glm::mat4(), glm::vec3(-3.0f, 1.f, 0.0f));
			glm::mat4 escalar_cub4 = glm::scale(glm::mat4(), glm::vec3(3.0f, float(movement) / 5, 2.0f));
			myObjMat = escalar_cub4 * translacio_cub4;

			glUniformMatrix4fv(glGetUniformLocation(myCubeProgram, "objMat"), 1, GL_FALSE, glm::value_ptr(myObjMat));
			glUniform4f(glGetUniformLocation(myCubeProgram, "color"), 1.0f, 1.0f, 1.0f, 1.0f);
			glDrawElements(GL_TRIANGLE_STRIP, myNumVerts, GL_UNSIGNED_BYTE, 0);

			glUseProgram(0);
			glBindVertexArray(0);
			glDisable(GL_PRIMITIVE_RESTART);
		}
	}

}