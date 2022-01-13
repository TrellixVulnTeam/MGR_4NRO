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
#include "Cube.h"
#include "Puma.h"
#include "Cursor.h"
#include "ImGuiFileDialog.h"
#include "Obstacle.h"
#include "Parameters.h"
#include <queue>

bool firstCall = true;
bool rotate = false;
glm::vec2 mousePosOld;
glm::vec3 lookAt;
std::shared_ptr<Program> program = {};
double lastTime;

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void window_size_callback(GLFWwindow* window, int width, int height);

float GetAngleDiff(float start, float end)
{
	if (end < start) end += 2 * M_PI;

	auto diff = end - start;
	if (diff > M_PI) diff = -(2 * M_PI - diff);
	return diff;
}

void GetDiffs()
{
	program->diffParams->a1 = GetAngleDiff(program->startParams->a1, program->endParams->a1);
	program->diffParams->a2 = GetAngleDiff(program->startParams->a2, program->endParams->a2);
	program->diffParams->a3 = GetAngleDiff(program->startParams->a3, program->endParams->a3);
	program->diffParams->a4 = GetAngleDiff(program->startParams->a4, program->endParams->a4);
	program->diffParams->a5 = GetAngleDiff(program->startParams->a5, program->endParams->a5);
	program->diffParams->q2 = program->endParams->q2 - program->startParams->q2;
}

glm::vec3 ArbitraryRotate(glm::vec3 p, float angle, glm::vec3 axis)
{
	glm::quat quat = glm::angleAxis(angle, axis);
	return quat * p;
}

void Simulate()
{
	auto time = glfwGetTime();
	program->t = (time - lastTime) / (program->simTime);
	if (program->t >= 1.0f)
	{
		program->t = 1.0f;
		program->simulating = false;
		return;
	}


	program->wind1->puma->params->Set(program->startParams, program->t, program->diffParams);

	auto pos = program->startPos + program->t * (program->endPos - program->startPos);
	auto quat = glm::normalize(glm::slerp(program->startQuat, program->endQuat, program->t));
	auto cur = program->wind2->puma->GetParams();
	auto res = program->wind2->puma->InverseKinematics(pos, quat);
	if (cur->Distance(res.first) < cur->Distance(res.second))
		program->wind2->puma->SetParams(res.first);
	else
		program->wind2->puma->SetParams(res.second);
	//program->wind2->figures[0]->SetRotation(quat);

}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	auto wind = program->wind1;
	if (!wind->focused)
		wind = program->wind2;
	if (ImGui::GetIO().WantCaptureMouse) return;
	glm::vec2 mousePos = { xpos,ypos };
	glm::vec2 diff = mousePos - mousePosOld;
	double xDiff = (double)diff.x / wind->current_width;
	double yDiff = (double)diff.y / wind->current_height;
	int lShiftState = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT);
	int lAltState = glfwGetKey(window, GLFW_KEY_LEFT_ALT);
	int rAltState = glfwGetKey(window, GLFW_KEY_RIGHT_ALT);


	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
	{
		if (lShiftState == GLFW_PRESS)
		{
			double xAngle = 2 * yDiff;
			double yAngle = 2 * xDiff;

			glm::vec3 camVec = program->currentWindow->cam->pos - lookAt;
			camVec = ArbitraryRotate(camVec, xAngle, program->currentWindow->cam->right);
			program->currentWindow->cam->LookAt(lookAt + camVec, ArbitraryRotate(program->currentWindow->cam->front, xAngle, program->currentWindow->cam->right), ArbitraryRotate(program->currentWindow->cam->up, xAngle, program->currentWindow->cam->right));


			camVec = program->currentWindow->cam->pos - lookAt;
			camVec = ArbitraryRotate(camVec, yAngle, program->currentWindow->cam->up);
			program->currentWindow->cam->LookAt(lookAt + camVec, ArbitraryRotate(program->currentWindow->cam->front, yAngle, program->currentWindow->cam->up), program->currentWindow->cam->up);
		}
	}
	mousePosOld = mousePos;
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

void Normalize(glm::quat& q)
{
	q.x = q.x < -1.0f ? -1.0f : q.x;
	q.y = q.y < -1.0f ? -1.0f : q.y;
	q.z = q.z < -1.0f ? -1.0f : q.z;
	q.w = q.w < -1.0f ? -1.0f : q.w;
	q.x = q.x > 1.0f ? 1.0f : q.x;
	q.y = q.y > 1.0f ? 1.0f : q.y;
	q.z = q.z > 1.0f ? 1.0f : q.z;
	q.w = q.w > 1.0f ? 1.0f : q.w;

	if (q.x == 0.0f && q.y == 0.0f && q.z == 0.0f && q.w == 0.0f)
		q.w = 1.0f;

	q = glm::normalize(q);
}
void Normalize()
{
	Normalize(program->q1);
	Normalize(program->q2);
}

void window_focus_callback(GLFWwindow* window, int focused)
{
	if (focused)
	{
		bool w1 = (window == program->wind1->window);
		program->wind1->focused = w1;
		program->wind2->focused = !w1;
	}
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

		ImGui::Checkbox("Normalize quats live", &program->normalize_live);

		if (ImGui::TreeNode("Start pos"))
		{
			ImGui::SliderFloat("Start X", &program->p1.x, -4.0f, 4.0f);
			ImGui::SliderFloat("Start Y", &program->p1.y, -4.0f, 4.0f);
			ImGui::SliderFloat("Start Z", &program->p1.z, -4.0f, 4.0f);
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Start rot"))
		{
			ImGui::SliderFloat("X", &program->q1.x, -1.0f, 1.0f);
			ImGui::SliderFloat("Y", &program->q1.y, -1.0f, 1.0f);
			ImGui::SliderFloat("Z", &program->q1.z, -1.0f, 1.0f);
			ImGui::SliderFloat("W", &program->q1.w, -1.0f, 1.0f);
			ImGui::TreePop();
		}


		if (ImGui::TreeNode("End pos"))
		{
			ImGui::SliderFloat("End X", &program->p2.x, -4.0f, 4.0f);
			ImGui::SliderFloat("End Y", &program->p2.y, -4.0f, 4.0f);
			ImGui::SliderFloat("End Z", &program->p2.z, -4.0f, 4.0f);
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("End rot"))
		{
			ImGui::SliderFloat("X", &program->q2.x, -1.0f, 1.0f);
			ImGui::SliderFloat("Y", &program->q2.y, -1.0f, 1.0f);
			ImGui::SliderFloat("Z", &program->q2.z, -1.0f, 1.0f);
			ImGui::SliderFloat("W", &program->q2.w, -1.0f, 1.0f);
			ImGui::TreePop();
		}

		if (program->normalize_live)
			Normalize();
		else
		{
			if (ImGui::Button("Normalize"))
				Normalize();
		}

		program->wind1->cursor->MoveToVec(program->p1);
		program->wind1->cursor->SetRotation(program->q1);
		program->wind2->cursor->MoveToVec(program->p2);
		program->wind2->cursor->SetRotation(program->q2);

		if (ImGui::Button("Inverse start v1"))
		{
			Normalize();
			program->wind1->puma->ForceConfiguration(program->p1, program->q1, 1);
		}
		if (ImGui::Button("Inverse start v2"))
		{
			Normalize();
			program->wind1->puma->ForceConfiguration(program->p1, program->q1, 2);
		}
		if (ImGui::Button("Inverse end v1"))
		{
			Normalize();
			program->wind2->puma->ForceConfiguration(program->p2, program->q2, 1);
		}
		if (ImGui::Button("Inverse end v2"))
		{
			Normalize();
			program->wind2->puma->ForceConfiguration(program->p2, program->q2, 2);
		}

		if (ImGui::Button("Set start pos"))
		{
			Normalize();
			program->startPos = program->p1;
			program->startQuat = program->q1;
			program->startParams = program->wind1->puma->GetParams();
		}
		if (ImGui::Button("Set end pos"))
		{
			Normalize();
			program->endPos = program->p2;
			program->endQuat = program->q2;
			program->endParams = program->wind2->puma->GetParams();
		}

		if (ImGui::Button("Show start pos"))
		{
			program->p1 = program->startPos;
			program->q1 = program->startQuat;
			program->wind1->puma->SetParams(program->startParams);
		}
		if (ImGui::Button("Show end pos"))
		{
			program->p2 = program->endPos;
			program->q2 = program->endQuat;
			program->wind2->puma->SetParams(program->endParams);
		}

	}

	ImGui::SliderFloat("Simulation time", &program->simTime, 1.0f, 10.0f);
	if (ImGui::Button("Simulate"))
	{
		program->simulating = true;
		program->t = 0;
		lastTime = glfwGetTime();
		GetDiffs();
	}
	ImGui::End();
}


void DrawScene(float c)
{
	auto time = glfwGetTime();

	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// make sure we clear the framebuffer's content
	glClearColor(c, 0.4f, 0.4f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glm::mat4 persp = program->currentWindow->cam->GetProjectionMatrix();
	glm::mat4 view = program->currentWindow->cam->GetViewportMatrix();

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

void GeneratePuma()
{
	glfwMakeContextCurrent(program->currentWindow->window);
	program->currentWindow->puma = std::make_shared<Puma>();
	program->currentWindow->puma->Initialize(program);
	program->currentWindow->figures.push_back(program->currentWindow->puma);

	program->currentWindow->cursor = std::make_shared<Cursor>();
	program->currentWindow->cursor->Initialize(program);
	program->currentWindow->figures.push_back(program->currentWindow->cursor);
}

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	GLFWwindow* window = glfwCreateWindow(DEFAULT_WIDTH, DEFAULT_HEIGHT, "Puma 1", NULL, NULL);
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
	program->wind1 = std::make_shared<Window>(1);
	program->wind2 = std::make_shared<Window>(2);
	program->wind1->window = window;
	program->wind1->current_width = DEFAULT_WIDTH;
	program->wind1->current_height = DEFAULT_HEIGHT;
	program->wind2->current_width = DEFAULT_WIDTH;
	program->wind2->current_height = DEFAULT_HEIGHT;
	//program->shader = new Shader("shaders/vertexShader.vs", "shaders/fragShader.fs",nullptr);
	glViewport(0, 0, DEFAULT_WIDTH, DEFAULT_HEIGHT);
	//	glfwSetInputMode(window, GLFW_STICKY_MOUSE_BUTTONS, GLFW_TRUE);
	glfwSetWindowFocusCallback(window, window_focus_callback);
	glfwSetFramebufferSizeCallback(window, proc.framebuffer_size_callback);
	glfwSetWindowSizeCallback(window, window_size_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	/*
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetKeyCallback(window, key_callback);
*/
	program->wind1->shader = std::make_shared<Shader>("shaders/vertexShader.vs"
		, "shaders/fragShader.fs"
		, nullptr);

	program->wind1->shader2D = std::make_shared<Shader>("shaders/vertexShader2d.vs"
		, "shaders/fragShader.fs"
		, nullptr);

	program->wind1->lightShader = std::make_shared<Shader>("shaders/lightVertexShader.vs"
		, "shaders/lightFragShader.fs"
		, nullptr);

	GLFWwindow* window2 = glfwCreateWindow(DEFAULT_WIDTH, DEFAULT_HEIGHT, "Puma 2", NULL, NULL);
	program->wind2->window = window2;

	if (window2 == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window2);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}
	glViewport(0, 0, DEFAULT_WIDTH, DEFAULT_HEIGHT);
	glfwSetWindowFocusCallback(window2, window_focus_callback);
	glfwSetFramebufferSizeCallback(window2, proc.framebuffer_size_callback);
	glfwSetWindowSizeCallback(window2, window_size_callback);
	glfwSetMouseButtonCallback(window2, mouse_button_callback);
	glfwSetCursorPosCallback(window2, mouse_callback);
	glEnable(GL_LINE_SMOOTH);

	program->wind2->shader = std::make_shared<Shader>("shaders/vertexShader.vs"
		, "shaders/fragShader.fs"
		, nullptr);

	program->wind2->shader2D = std::make_shared<Shader>("shaders/vertexShader2d.vs"
		, "shaders/fragShader.fs"
		, nullptr);

	program->wind2->lightShader = std::make_shared<Shader>("shaders/lightVertexShader.vs"
		, "shaders/lightFragShader.fs"
		, nullptr);


	program->diffParams = std::make_shared<Parameters>();
	program->startParams = std::make_shared<Parameters>();
	program->endParams = std::make_shared<Parameters>();
	glfwMakeContextCurrent(window2);

	program->currentWindow = program->wind1;
	program->currentWindow->cam = std::make_shared<Camera>();
	program->currentWindow->cam->LookAt({ 0,2,4 }, { 0,0,-1 }, { 0,1,0 });
	GeneratePuma();

	program->currentWindow = program->wind2;
	program->currentWindow->cam = std::make_shared<Camera>();
	program->currentWindow->cam->LookAt({ 0,2,4 }, { 0,0,-1 }, { 0,1,0 });
	GeneratePuma();

	program->p1 = { 2.0f,1.0f,-2.0f };
	//program->p1 = { 3.0f,0.0f,0.0f };
	program->p1 = { 2.0f,0.0f,0.0f };
	program->p2 = { -1.0f,-1.0f,1.0f };
	program->q1 = { 0.0f,1.0f,0.0f,0.0f };
	//	program->q1 = { 0.0f,0.0f,0.0f,1.0f };
	program->q2 = { 0.0f,0.0f,1.0f,0.0f };
	Normalize();
	program->wind1->puma->ForceConfiguration(program->p1, program->q1, 1);
	program->wind2->puma->ForceConfiguration(program->p2, program->q2, 1);

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	// Setup Platform/Renderer bindings
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 130");
	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	while (!glfwWindowShouldClose(window) && !glfwWindowShouldClose(window2))
	{
		if (program->simulating)
		{
			Simulate();
		}
		else
		{

		}

		glfwMakeContextCurrent(window);
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		//ImGui::ShowDemoWindow();
		program->currentWindow = program->wind1;
		float aspect = (float)program->currentWindow->current_width / (float)program->currentWindow->current_height;
		program->currentWindow->cam->SetPerspective(aspect);
		proc.processInput(window);
		RenderGui();
		program->currentWindow = program->wind1;
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		DrawScene(1.0f);
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		if (program->wind1->focused)
			glfwPollEvents();

		glfwSwapBuffers(window);

		glfwMakeContextCurrent(window2);
		program->currentWindow = program->wind2;
		aspect = (float)program->currentWindow->current_width / (float)program->currentWindow->current_height;
		program->currentWindow->cam->SetPerspective(aspect);
		proc.processInput(window2);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		DrawScene(0.0f);

		glfwSwapBuffers(window2);
		if (program->wind2->focused)
			glfwPollEvents();
		if (!program->wind1->focused && !program->wind2->focused)
			glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}



void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	double x_pos, y_pos;
	auto wind = program->wind1;
	if (!wind->focused)
		wind = program->wind2;
	glfwGetCursorPos(window, &x_pos, &y_pos);
	int lCtrlState = glfwGetKey(window, GLFW_KEY_LEFT_CONTROL);
	int lAltState = glfwGetKey(window, GLFW_KEY_LEFT_ALT);
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && lAltState == GLFW_PRESS)
	{
		if (program->edition && !program->selection)
		{
			float aspect = (float)wind->current_width / (float)wind->current_height;
			glm::vec2 mousePos = glm::vec2(((x_pos / wind->current_width) * 2.0f - 1.0f) * aspect, -((y_pos / wind->current_height) * 2.0f - 1.0f));

		}
	}

}

void window_size_callback(GLFWwindow* window, int width, int height) {
	auto wind = program->wind1;
	if (wind->window != window)
		wind = program->wind2;
	wind->current_width = width;
	wind->current_height = height;
}