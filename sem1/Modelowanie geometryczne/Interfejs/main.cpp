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
#include "Figure.h"
#include "Torus.h"
#include "Point.h"
#include <vector>
#include "MiddlePoint.h"
#include "Cursor.h"
#define DEFAULT_WIDTH 1280
#define DEFAULT_HEIGHT 720

int current_width = DEFAULT_WIDTH;
int current_height = DEFAULT_HEIGHT;
bool firstCall = true;
bool rotate = false;
glm::vec2 mousePosOld;
std::vector<Figure*> figures;
MiddlePoint* mp;
Cursor* cur;


void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	glm::vec2 mousePos = { xpos,ypos };
	//glfwSetWindowTitle(window, (std::to_string(xpos) + std::string(" ") + std::to_string(ypos)).c_str());
	glm::vec2 diff = mousePos - mousePosOld;
	double xDiff = (double)diff.x / current_width;
	double yDiff = (double)diff.y / current_height;
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
	{
		for (int i = 0; i < figures.size(); ++i)
		{
			if (figures[i]->GetSelected())
			{
				figures[i]->Move(8 * xDiff, -8 * yDiff, 0.0f);
			}
		}
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

		for (int i = 0; i < figures.size(); ++i)
		{
			if (figures[i]->GetSelected())
			{
				if (rotate)
				{
					figures[i]->RotateAround(cur->GetPos(), xAngle, yAngle);
				}
				else
				{
					figures[i]->RotateAround(mp->GetPos(),xAngle, yAngle);
				}
			}
		}
	}
	mousePosOld = mousePos;
	//TODO: write mouse camera control
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	for (int i = 0; i < figures.size(); ++i)
	{
		if (figures[i]->GetSelected())
		{
			if (yoffset >= 1)
			{
				figures[i]->ScaleAround(cur->GetPos(),1.1f);
			}
			if (yoffset <= -1)
			{
				figures[i]->ScaleAround(cur->GetPos(), 0.9f);
			}
		}
	}
}

void window_size_callback(GLFWwindow* window, int width, int height) {
	current_width = width;
	current_height = height;
}


void RenderGui(Shader& shader)
{
	if (firstCall)
	{
		ImGui::SetNextWindowSize(ImVec2(200, 400), 0);
		firstCall = false;
	}

	int to_delete = -1;

	ImGui::Begin("Menu");
	ImGui::Checkbox("Rotate around cursor", &rotate);
	cur->GetGui(-1);
	if (ImGui::TreeNode("Adding"))
	{
		if (ImGui::Button("New Torus"))
		{
			Figure* f= new Torus(shader);
			f->Initialize();
			auto pos = cur->GetPos();
			f->MoveTo(pos.x, pos.y, pos.z);
			figures.push_back(f);
		}
		if (ImGui::Button("New Point"))
		{
			Figure* f= new Point(shader);
			f->Initialize();
			auto pos = cur->GetPos();
			f->MoveTo(pos.x, pos.y, pos.z);
			figures.push_back(f);
		}
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("Figures"))
	{
		for (int i = 0; i < figures.size(); ++i)
		{
			if (ImGui::TreeNode((std::to_string(i) + std::string(" - ") + figures[i]->name).c_str()))
			{
				if (figures[i]->GetGui(i))
				{
					to_delete = i;
				}
				ImGui::TreePop();
			}
			figures[i]->RecalcFigure();
		}
		ImGui::TreePop();
	}

	if (to_delete >= 0)
	{
		delete figures[to_delete];
		figures.erase(figures.begin() + to_delete);
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



	Shader shader("shaders/vertexShader.vs", "shaders/fragShader.fs");

	figures.push_back(new Torus(shader));
	figures[0]->Initialize();
	figures.push_back(new Torus(shader));
	figures[1]->Initialize();
	mp = new MiddlePoint(shader);
	mp->Initialize();
	cur = new Cursor(shader);
	cur->Initialize();

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

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

		unsigned int perspLoc = glGetUniformLocation(shader.ID, "persp");
		unsigned int viewLoc = glGetUniformLocation(shader.ID, "view");
		unsigned int transLoc = glGetUniformLocation(shader.ID, "transform");
		glUniformMatrix4fv(perspLoc, 1, GL_FALSE, glm::value_ptr(persp));
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

		mp->Reset();
		for (int i = 0; i < figures.size(); ++i)
		{
			figures[i]->Draw(transLoc);
			mp->Add(figures[i]);
		}

		mp->Draw(transLoc);
		cur->Draw(transLoc);

		RenderGui(shader);
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	for (int i = 0; i < figures.size(); ++i) delete figures[i];
	delete mp;
	return 0;
}

