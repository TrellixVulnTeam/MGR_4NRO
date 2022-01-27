#pragma once
#define STB_IMAGE_IMPLEMENTATION
#define BITMAP_SIZE 360
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>
#include <stb_image.h>
#include "processing.h"
#include "Shader.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "Figure.h"
#include "Camera.h"
#include "Program.h"
#include "Tool.h"
#include "ImGuiFileDialog.h"
#include <queue>
#include <implot.h>



bool firstCall = true;
bool rotate = false;
double lastTime, initTime;
glm::vec2 mousePosOld;
glm::vec3 lookAt;
std::shared_ptr<Program> program = {};
std::vector<glm::ivec3> path = {};

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void window_size_callback(GLFWwindow* window, int width, int height);
double last_x, last_xdt;


void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	if (ImGui::GetIO().WantCaptureMouse) return;
	auto& figures = program->currentWindow->figures;
	for (int i = 0; i < figures.size(); ++i)
	{
		if (figures[i]->GetSelected() && figures[i]->CanMove())
		{
			if (yoffset >= 1)
			{
				figures[i]->ScaleAround({ 0.0f,0.0f, 0.0f }, 1.1f);
			}
			if (yoffset <= -1)
			{
				figures[i]->ScaleAround({ 0.0f,0.0f, 0.0f }, 0.9f);
			}
		}
	}
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	float camMove = 0.04f;
	auto pos = program->cam->pos;
	if (key == GLFW_KEY_A) pos.x -= camMove;
	if (key == GLFW_KEY_D) pos.x += camMove;
	if (key == GLFW_KEY_S) pos.y -= camMove;
	if (key == GLFW_KEY_W) pos.y += camMove;

	program->cam->LookAt(pos, program->cam->front, program->cam->up);
}



void Simulate()
{
	auto time = lastTime + 0.01;// glfwGetTime() - initTime;
	auto diff = time - lastTime;
	program->currentWindow->tool->angle += diff * (double)program->omega;


	double x = program->currentWindow->tool->pos_now;
	double xdt = (x - last_x)/diff;
	double xddt = (xdt-last_xdt)/diff;
	program->AddValues(time, x, xdt, xddt);

	lastTime = time;
	last_x = x;
	last_xdt = xdt;
}

glm::vec3 ArbitraryRotate(glm::vec3 p, float angle, glm::vec3 axis)
{
	glm::quat quat = glm::angleAxis(angle, axis);
	return quat * p;
}

glm::quat EulerToQuat(glm::vec3 euler)
{
	float c = M_PI / 180.0f;
	return glm::normalize(glm::quat(c * euler));
}

glm::vec3 QuatToEuler(glm::quat quat)
{
	float c = 180.0f / M_PI;
	return c * glm::eulerAngles(quat);
}

void RenderGui()
{
	if (firstCall)
	{
		ImGui::SetNextWindowSize(ImVec2(250, 400), 0);
		firstCall = false;
	}

	int to_delete = -1;
	ImGui::Begin("Menu");
	ImGui::Checkbox("simulate", &program->simulating);
	ImGui::SliderFloat("R", &program->currentWindow->tool->r, 0.01f, 10.0f);
	ImGui::SliderFloat("L", &program->currentWindow->tool->l, 0.01f, 10.0f);
	ImGui::SliderFloat("omega", &program->omega, 0.01f, 10.0f);
	ImGui::SliderFloat("block_size", &program->currentWindow->tool->block_size, 0.01f, 10.0f);
	if (ImPlot::BeginPlot("Wykresy 1")) {
		static float history = 10.0f;
		static ImPlotAxisFlags flags = ImPlotAxisFlags_NoTickLabels;

		ImPlot::SetupAxes(NULL, NULL, flags, flags);
		ImPlot::SetupAxisLimits(ImAxis_X1, lastTime - history, lastTime, ImGuiCond_Always);
		ImPlot::SetupAxisLimits(ImAxis_Y1, -1.2, 1.2);
		ImPlot::SetNextFillStyle(IMPLOT_AUTO_COL, 0.5f);
		ImPlot::PlotLine("x(t)", &program->_y.Data[0].x, &program->_y.Data[0].y, program->_y.Data.size(), program->_y.Offset, 2 * sizeof(float));
		ImPlot::PlotLine("x'(t)", &program->_dy.Data[0].x, &program->_dy.Data[0].y, program->_dy.Data.size(), program->_dy.Offset, 2 * sizeof(float));
		ImPlot::PlotLine("x''(t)", &program->_ddy.Data[0].x, &program->_ddy.Data[0].y, program->_ddy.Data.size(), program->_ddy.Offset, 2 * sizeof(float));
		ImPlot::EndPlot();
	}
	if (ImPlot::BeginPlot("Wykresy 2")) {
		static float history = 10.0f;
		static ImPlotAxisFlags flags = ImPlotAxisFlags_NoTickLabels;

		ImPlot::SetupAxes(NULL, NULL, flags, flags);
		ImPlot::SetupAxisLimits(ImAxis_X1, -1.2, 1.2);
		ImPlot::SetupAxisLimits(ImAxis_Y1, -1.2, 1.2);
		ImPlot::SetNextFillStyle(IMPLOT_AUTO_COL, 0.5f);
		ImPlot::PlotLine("x'(x)", &program->_state.Data[0].x, &program->_state.Data[0].y, program->_state.Data.size(), program->_state.Offset, 2 * sizeof(float));
		ImPlot::EndPlot();
	}

	if (!program->simulating) {
		if (program->error.length() > 0)
		{
			if (!ImGui::IsPopupOpen("Error"))
				ImGui::OpenPopup("Error");
			if (ImGui::BeginPopupModal("Error", NULL))
			{
				ImGui::Text(program->error.c_str());
				if (ImGui::Button("Close"))
				{
					ImGui::CloseCurrentPopup();
					program->error = "";
				}
				ImGui::EndPopup();
			}
		}

	}
	ImGui::SliderFloat("Simulation time", &program->simTime, 1.0f, 10.0f);

	ImGui::End();
}


void DrawScene()
{
	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// make sure we clear the framebuffer's content
	glClearColor(0.4f, 0.4f, 0.4f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glm::mat4 persp = program->cam->GetProjectionMatrix();
	glm::mat4 view = program->cam->GetViewportMatrix();

	program->currentWindow->shader->use();
	unsigned int perspLoc = glGetUniformLocation(program->currentWindow->shader->ID, "persp");
	unsigned int viewLoc = glGetUniformLocation(program->currentWindow->shader->ID, "view");
	glUniformMatrix4fv(perspLoc, 1, GL_FALSE, glm::value_ptr(persp));
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	program->currentWindow->lightShader->use();
	perspLoc = glGetUniformLocation(program->currentWindow->lightShader->ID, "persp");
	viewLoc = glGetUniformLocation(program->currentWindow->lightShader->ID, "view");
	glUniformMatrix4fv(perspLoc, 1, GL_FALSE, glm::value_ptr(persp));
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	program->currentWindow->shader2D->use();
	perspLoc = glGetUniformLocation(program->currentWindow->shader2D->ID, "persp");
	viewLoc = glGetUniformLocation(program->currentWindow->shader2D->ID, "view");
	glUniformMatrix4fv(perspLoc, 1, GL_FALSE, glm::value_ptr(persp));
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

	for (int i = 0; i < program->currentWindow->figures.size(); ++i)
	{
		program->currentWindow->figures[i]->RecalcFigure();
		program->currentWindow->figures[i]->Draw();
	}
}


int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	GLFWwindow* window = glfwCreateWindow(DEFAULT_WIDTH, DEFAULT_HEIGHT, "PUSN", NULL, NULL);
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
	program = std::make_shared<Program>();
	program->currentWindow = std::make_shared<Window>();
	program->currentWindow->window = window;
	program->current_width = DEFAULT_WIDTH;
	program->current_height = DEFAULT_HEIGHT;
	//program->shader = new Shader("shaders/vertexShader.vs", "shaders/fragShader.fs",nullptr);
	glViewport(0, 0, DEFAULT_WIDTH, DEFAULT_HEIGHT);
	//	glfwSetInputMode(window, GLFW_STICKY_MOUSE_BUTTONS, GLFW_TRUE);
	glfwSetFramebufferSizeCallback(window, proc.framebuffer_size_callback);
	glfwSetWindowSizeCallback(window, window_size_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetKeyCallback(window, key_callback);
	initTime = glfwGetTime();
	lastTime = glfwGetTime() - initTime;
	/*
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetKeyCallback(window, key_callback);
*/
	program->currentWindow->shader = std::make_shared<Shader>("shaders/vertexShader.vs"
		, "shaders/fragShader.fs"
		, nullptr);

	program->currentWindow->shader2D = std::make_shared<Shader>("shaders/vertexShader2d.vs"
		, "shaders/fragShader.fs"
		, nullptr);

	program->currentWindow->lightShader = std::make_shared<Shader>("shaders/lightVertexShader.vs"
		, "shaders/lightFragShader.fs"
		, nullptr);

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

	glEnable(GL_LINE_SMOOTH);

	program->cam = std::make_shared<Camera>();
	//	program->cam = new Camera();
	program->cam->LookAt({ 0,0,1 }, { 0,0,-1 }, { 0,1,0 });

	program->currentWindow->tool = std::make_shared<Tool>();
	program->currentWindow->tool->Initialize(program);
	program->currentWindow->figures.push_back(program->currentWindow->tool);
	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImPlot::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	// Setup Platform/Renderer bindings
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 130");
	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	while (!glfwWindowShouldClose(window))
	{
		if (program->simulating)
		{
			Simulate();
		}
		float aspect = (float)program->current_width / (float)program->current_height;
		program->cam->SetOrtographic(aspect);

		glfwMakeContextCurrent(window);
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		//ImGui::ShowDemoWindow();
		//ImPlot::ShowDemoWindow();
		proc.processInput(window);
		RenderGui();
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		DrawScene();
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);

		glfwPollEvents();
	}

	ImGui::DestroyContext();
	ImPlot::DestroyContext();
	glfwTerminate();
	return 0;
}


void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	double x_pos, y_pos;
	glfwGetCursorPos(window, &x_pos, &y_pos);
	int lCtrlState = glfwGetKey(window, GLFW_KEY_LEFT_CONTROL);
	int lAltState = glfwGetKey(window, GLFW_KEY_LEFT_ALT);

}

void window_size_callback(GLFWwindow* window, int width, int height) {
	program->current_width = width;
	program->current_height = height;
}