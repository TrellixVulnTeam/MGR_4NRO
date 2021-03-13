#pragma once
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "processing.h"
#include "Shader.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include <glm/gtx/transform.hpp>
#define DEFAULT_WIDTH 1280
#define DEFAULT_HEIGHT 720

int current_width = DEFAULT_WIDTH;
int current_height = DEFAULT_HEIGHT;
bool firstCall = true;
glm::vec2 mousePosOld;
float scale = 1.0;
double xMove = 0.0;
double yMove = 0.0;
glm::mat4 myRotationMatrix = glm::mat4(1.0f);


void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	glm::vec2 mousePos = { xpos,ypos };
	//glfwSetWindowTitle(window, (std::to_string(xpos) + std::string(" ") + std::to_string(ypos)).c_str());
	glm::vec2 diff = mousePos - mousePosOld;
	double xDiff = (double)diff.x / current_width;
	double yDiff = (double)diff.y / current_height;
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
	{
		xMove += 8 * xDiff;
		yMove -= 8 * yDiff;
	}

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
	{
		glm::mat4 rotX = glm::mat4(1.0f);
		glm::mat4 rotY = glm::mat4(1.0f);

		double xAngle = 2*yDiff;
		double yAngle = 2*xDiff;

		rotX[1][1] = cos(xAngle);
		rotX[2][1] = -sin(xAngle);
		rotX[1][2] = sin(xAngle);
		rotX[2][2] = cos(xAngle);
		
		rotY[0][0] = cos(yAngle);
		rotY[2][0] = sin(yAngle);
		rotY[0][2] = -sin(yAngle);
		rotY[2][2] = cos(yAngle);


		myRotationMatrix = rotX * myRotationMatrix;
		myRotationMatrix = rotY * myRotationMatrix;
		glm::vec2 diff = mousePos - mousePosOld;
	}
	mousePosOld = mousePos;
	//TODO: write mouse camera control
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	if (yoffset >= 1)
	{
		if (scale <= 3)
			scale *= 1.1;
	}
	if (yoffset <= -1)
	{
		if (scale >= 0.3)
			scale /= 1.1;
	}
}

void window_size_callback(GLFWwindow* window, int width, int height) {
	current_width = width;
	current_height = height;
}


void RenderGui(Processing& proc)
{
	if (firstCall)
	{
		ImGui::SetNextWindowSize(ImVec2(200, 400), 0);
		firstCall = false;
	}
	ImGui::Begin("Menu");


	ImGui::SliderInt("n", &proc.n_new, 5, 50);
	ImGui::SliderInt("m", &proc.m_new, 5, 50);
	ImGui::SliderFloat("r", &proc.r_new, 0.1f, 5.0f);
	ImGui::SliderFloat("R", &proc.R_new, 0.3f, 10.0f);

	if (proc.CreateTorus())
	{
		glBufferData(GL_ARRAY_BUFFER, proc.vertices_s * sizeof(float), proc.vertices, GL_STATIC_DRAW);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, proc.indices_s * sizeof(unsigned int), proc.indices, GL_STATIC_DRAW);
	}

	ImGui::End();
}

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	GLFWwindow* window = glfwCreateWindow(DEFAULT_WIDTH, DEFAULT_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	Processing proc = Processing();

	glViewport(0, 0, DEFAULT_WIDTH, DEFAULT_HEIGHT);
	glfwSetFramebufferSizeCallback(window, proc.framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetWindowSizeCallback(window, window_size_callback);
	glfwSetScrollCallback(window, scroll_callback);

	Shader ourShader("shaders/vertexShader.vs", "shaders/fragShader.fs");

	unsigned int VBO;
	glGenBuffers(1, &VBO);
	unsigned int VAO;
	glGenVertexArrays(1, &VAO);
	unsigned int EBO;
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, proc.vertices_s * sizeof(float), proc.vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, proc.indices_s * sizeof(unsigned int), proc.indices, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);



	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	float radians = 1.0f;

	glm::mat4 projection = glm::mat4(0.0f);

	glm::mat4 view = glm::mat4(1.0f);
	view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
	glm::mat4 persp = glm::mat4(1.0f);
	float _near = 1;
	float _far = 5;
	float fov = 1.0f;
	float e = 1 / tan(fov / 2);
	float aspect = (float)800 / (float)600;
	persp[0][0] = e / aspect;
	persp[1][1] = e;
	persp[2][2] = (_far + _near) / (_far - _near);
	persp[2][3] = -((_far * _near * 2) / (_far - _near));
	persp[3][3] = 0;
	persp[3][2] = 1;

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	// Setup Platform/Renderer bindings
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 130");
	// Setup Dear ImGui style
	ImGui::StyleColorsDark();

	while (!glfwWindowShouldClose(window))
	{
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		proc.processInput(window);

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		radians += 0.010f;
		glm::mat4 myScaleMatrix = glm::mat4(1.0f);
		myScaleMatrix[0][0] = scale;
		myScaleMatrix[1][1] = scale;
		myScaleMatrix[2][2] = scale;

		glm::mat4 myTranslationMatrix = glm::mat4(1.0f);
		myTranslationMatrix[3][0] = xMove;
		myTranslationMatrix[3][1] = yMove;

		glm::mat4 trans = myTranslationMatrix
			* myRotationMatrix
			* myScaleMatrix;


		unsigned int perspLoc = glGetUniformLocation(ourShader.ID, "persp");
		unsigned int viewLoc = glGetUniformLocation(ourShader.ID, "view");
		unsigned int transLoc = glGetUniformLocation(ourShader.ID, "transform");
		glUniformMatrix4fv(perspLoc, 1, GL_FALSE, glm::value_ptr(persp));
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(transLoc, 1, GL_FALSE, glm::value_ptr(trans));

		ourShader.use();
		glBindVertexArray(VAO);
		glDrawElements(GL_LINES, proc.indices_s, GL_UNSIGNED_INT, 0);

		RenderGui(proc);
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}

