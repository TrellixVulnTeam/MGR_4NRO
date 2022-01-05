#pragma once
#define STB_IMAGE_IMPLEMENTATION
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
#include "Duck.h"
#include "ImGuiFileDialog.h"

bool firstCall = true;
bool rotate = false;
glm::vec2 mousePosOld;
glm::vec3 lookAt;
std::shared_ptr<Program> program = {};
double lastTime;
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void window_focus_callback(GLFWwindow* window, int focused);
void window_size_callback(GLFWwindow* window, int width, int height);

void CleanUp(bool all = false)
{
	while (program->wind1->figures.size() > 1) program->wind1->figures.erase(program->wind1->figures.begin() + 1);
	while (program->wind2->figures.size() > 1) program->wind2->figures.erase(program->wind2->figures.begin() + 1);
	if (all)
	{
		while (program->wind1->figures.size() > 0) program->wind1->figures.erase(program->wind1->figures.begin());
		while (program->wind2->figures.size() > 0) program->wind2->figures.erase(program->wind2->figures.begin());
	}
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

void RecalcParams()
{
	if (program->setQuats)
	{
		program->startAngle = QuatToEuler(program->startQuat);
		program->endAngle = QuatToEuler(program->endQuat);
	}
	else
	{
		auto quatStart = EulerToQuat(program->startAngle);
		auto quatEnd = EulerToQuat(program->endAngle);

		program->startQuat = quatStart;
		program->endQuat = quatEnd;
	}

	{
		auto start = program->startAngle.x;
		auto end = program->endAngle.x;
		if (end < start) end += 360.0f;

		auto diff = end - start;
		if (diff > 180.0f) diff = -(360.0f - diff);
		program->diffAngle.x = diff;
	}
	{
		auto start = program->startAngle.y;
		auto end = program->endAngle.y;
		if (end < start) end += 360.0f;

		auto diff = end - start;
		if (diff > 180.0f) diff = -(360.0f - diff);
		program->diffAngle.y = diff;
	}
	{
		auto start = program->startAngle.z;
		auto end = program->endAngle.z;
		if (end < start) end += 360.0f;

		auto diff = end - start;
		if (diff > 180.0f) diff = -(360.0f - diff);
		program->diffAngle.z = diff;
	}
	{
		if (glm::length2(program->startQuat - program->endQuat) > glm::length2(program->startQuat + program->endQuat))
			program->endQuat = -program->endQuat;
	}
}

void GenerateCube()
{
	glfwMakeContextCurrent(program->currentWindow->window);
	std::shared_ptr<Figure> f = std::make_shared<Duck>();
	f->Initialize(program);
	f->MoveTo(0.0f, 0.0f, 0.0f);
	f->Select();
	program->currentWindow->figures.push_back(f);
}
void GenerateTex()
{
	glfwMakeContextCurrent(program->currentWindow->window);
	program->currentWindow->lightShader->use();
	glGenTextures(1, &program->currentWindow->colorTexture);
	glBindTexture(GL_TEXTURE_2D, program->currentWindow->colorTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load and generate the texture
	int width, height, nrChannels;
	unsigned char* data = stbi_load("resources/duck/ducktex.jpg", &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);
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

	if (!program->simulating)
	{
		ImGui::Checkbox("Set quaternions", &program->setQuats);

		ImGui::Checkbox("Spherical interpolation", &program->spherical);

		if (ImGui::TreeNode("Start pos"))
		{
			ImGui::SliderFloat("Start X", &program->startPos.x, -30.0f, 30.0f);
			ImGui::SliderFloat("Start Y", &program->startPos.y, -30.0f, 30.0f);
			ImGui::SliderFloat("Start Z", &program->startPos.z, -30.0f, 30.0f);
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Start rot"))
		{
			if (!program->setQuats)
			{
				ImGui::SliderFloat("Start X", &program->startAngle.x, -180.0f, 180.0f);
				ImGui::SliderFloat("Start Y", &program->startAngle.y, -180.0f, 180.0f);
				ImGui::SliderFloat("Start Z", &program->startAngle.z, -180.0f, 180.0f);
			}
			else
			{
				ImGui::SliderFloat("Start QuatX", &program->startQuat.x, -1.0f, 1.0f);
				ImGui::SliderFloat("Start QuatY", &program->startQuat.y, -1.0f, 1.0f);
				ImGui::SliderFloat("Start QuatZ", &program->startQuat.z, -1.0f, 1.0f);
				ImGui::SliderFloat("Start QuatW", &program->startQuat.w, -1.0f, 1.0f);
			}
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("End pos"))
		{
			ImGui::SliderFloat("End X", &program->endPos.x, -30.0f, 30.0f);
			ImGui::SliderFloat("End Y", &program->endPos.y, -30.0f, 30.0f);
			ImGui::SliderFloat("End Z", &program->endPos.z, -30.0f, 30.0f);
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("End rot"))
		{
			if (!program->setQuats)
			{
				ImGui::SliderFloat("End X", &program->endAngle.x, -180.0f, 180.0f);
				ImGui::SliderFloat("End Y", &program->endAngle.y, -180.0f, 180.0f);
				ImGui::SliderFloat("End Z", &program->endAngle.z, -180.0f, 180.0f);
			}
			else
			{
				ImGui::SliderFloat("End QuatX", &program->endQuat.x, -1.0f, 1.0f);
				ImGui::SliderFloat("End QuatY", &program->endQuat.y, -1.0f, 1.0f);
				ImGui::SliderFloat("End QuatZ", &program->endQuat.z, -1.0f, 1.0f);
				ImGui::SliderFloat("End QuatW", &program->endQuat.w, -1.0f, 1.0f);
			}
			ImGui::TreePop();
		}

		ImGui::SliderInt("Frames", &program->frames, 1.0f, 50.0f);
		if (ImGui::Button("Generate frames"))
		{
			CleanUp(true);
			RecalcParams();
			float t_diff = 1.0f / (program->frames - 1);
			for (int i = 0; i < program->frames; ++i)
			{
				float t = i * t_diff;
				auto pos = program->startPos + t * (program->endPos - program->startPos);

#pragma region fig1
				program->currentWindow = program->wind1;
				std::shared_ptr<Figure> fig1 = std::make_shared<Duck>();
				fig1->Initialize(program);
				fig1->Select();
				program->wind1->figures.push_back(fig1);
				fig1->MoveTo(pos.x, pos.y, pos.z);

				glm::vec3 angles = program->startAngle + t * program->diffAngle;

				fig1->SetRotation(EulerToQuat(angles));
#pragma endregion
#pragma region fig2
				program->currentWindow = program->wind2;
				std::shared_ptr<Figure> fig2 = std::make_shared<Duck>();
				fig2->Initialize(program);
				fig2->Select();
				program->wind2->figures.push_back(fig2);
				fig2->MoveTo(pos.x, pos.y, pos.z);
				if (program->spherical)
				{
					glm::quat quat = glm::normalize(glm::slerp(program->startQuat, program->endQuat, t));
					fig2->SetRotation(quat);
				}
				else
				{
					glm::quat quat = glm::normalize(glm::lerp(program->startQuat, program->endQuat, t));
					fig2->SetRotation(quat);
				}
#pragma endregion

			}
		}
		if (ImGui::Button("CleanUp"))
		{
			CleanUp();
		}
		if (ImGui::Button("Simulate"))
		{
			program->simulating = true;
			program->t = 0;
			lastTime = glfwGetTime();
			RecalcParams();
		}
	}
	ImGui::SliderFloat("Simulation time", &program->simTime, 1.0f, 10.0f);

	ImGui::End();

#pragma region valuesCheck
	if (program->simTime < 1.0f)program->simTime = 1.0f;
	if (program->simTime > 10.0f)program->simTime = 10.0f;

	program->startAngle.x = program->startAngle.x < -180.0f ? -180.0f : program->startAngle.x;
	program->startAngle.y = program->startAngle.y < -180.0f ? -180.0f : program->startAngle.y;
	program->startAngle.z = program->startAngle.z < -180.0f ? -180.0f : program->startAngle.z;
	program->startAngle.x = program->startAngle.x > 180.0f ? 180.0f : program->startAngle.x;
	program->startAngle.y = program->startAngle.y > 180.0f ? 180.0f : program->startAngle.y;
	program->startAngle.z = program->startAngle.z > 180.0f ? 180.0f : program->startAngle.z;

	program->endAngle.x = program->endAngle.x < -180.0f ? -180.0f : program->endAngle.x;
	program->endAngle.y = program->endAngle.y < -180.0f ? -180.0f : program->endAngle.y;
	program->endAngle.z = program->endAngle.z < -180.0f ? -180.0f : program->endAngle.z;
	program->endAngle.x = program->endAngle.x > 180.0f ? 180.0f : program->endAngle.x;
	program->endAngle.y = program->endAngle.y > 180.0f ? 180.0f : program->endAngle.y;
	program->endAngle.z = program->endAngle.z > 180.0f ? 180.0f : program->endAngle.z;

	program->startQuat.x = program->startQuat.x < -1.0f ? -1.0f : program->startQuat.x;
	program->startQuat.y = program->startQuat.y < -1.0f ? -1.0f : program->startQuat.y;
	program->startQuat.z = program->startQuat.z < -1.0f ? -1.0f : program->startQuat.z;
	program->startQuat.w = program->startQuat.w < -1.0f ? -1.0f : program->startQuat.w;

	program->startQuat.x = program->startQuat.x > 1.0f ? 1.0f : program->startQuat.x;
	program->startQuat.y = program->startQuat.y > 1.0f ? 1.0f : program->startQuat.y;
	program->startQuat.z = program->startQuat.z > 1.0f ? 1.0f : program->startQuat.z;
	program->startQuat.w = program->startQuat.w > 1.0f ? 1.0f : program->startQuat.w;

	program->endQuat.x = program->endQuat.x < -1.0f ? -1.0f : program->endQuat.x;
	program->endQuat.y = program->endQuat.y < -1.0f ? -1.0f : program->endQuat.y;
	program->endQuat.z = program->endQuat.z < -1.0f ? -1.0f : program->endQuat.z;
	program->endQuat.w = program->endQuat.w < -1.0f ? -1.0f : program->endQuat.w;

	program->endQuat.x = program->endQuat.x > 1.0f ? 1.0f : program->endQuat.x;
	program->endQuat.y = program->endQuat.y > 1.0f ? 1.0f : program->endQuat.y;
	program->endQuat.z = program->endQuat.z > 1.0f ? 1.0f : program->endQuat.z;
	program->endQuat.w = program->endQuat.w > 1.0f ? 1.0f : program->endQuat.w;

	if (program->startQuat.x == 0.0f && program->startQuat.y == 0.0f && program->startQuat.z == 0.0f && program->startQuat.w == 0.0f)
		program->startQuat.w = 1.0f;
	if (program->endQuat.x == 0.0f && program->endQuat.y == 0.0f && program->endQuat.z == 0.0f && program->endQuat.w == 0.0f)
		program->endQuat.w = 1.0f;
#pragma endregion

	program->startQuat = glm::normalize(program->startQuat);
	program->endQuat = glm::normalize(program->endQuat);
}

void Simulate()
{
	auto time = glfwGetTime();
	program->t = (time - lastTime) / (program->simTime);
	if (program->t >= 1.0f)
	{
		program->t = 1.0f;
		program->simulating = false;
	}

	auto pos = program->startPos + program->t * (program->endPos - program->startPos);


	program->wind1->figures[0]->MoveTo(pos.x, pos.y, pos.z);

	glm::vec3 angles = program->startAngle + program->t * program->diffAngle;
	program->wind1->figures[0]->SetRotation(EulerToQuat(angles));

	program->wind2->figures[0]->MoveTo(pos.x, pos.y, pos.z);
	if (program->spherical)
	{
		glm::quat quat = glm::normalize(glm::slerp(program->startQuat, program->endQuat, program->t));
		program->wind2->figures[0]->SetRotation(quat);
	}
	else
	{
		glm::quat quat = glm::normalize(glm::lerp(program->startQuat, program->endQuat, program->t));
		program->wind2->figures[0]->SetRotation(quat);
	}
}

void SetPositions()
{
	program->wind1->figures[0]->MoveTo(program->startPos.x, program->startPos.y, program->startPos.z);
	if (program->setQuats)
	{
		program->wind1->figures[0]->SetRotation(program->startQuat);
	}
	else
	{
		program->wind1->figures[0]->SetRotation(EulerToQuat(program->startAngle));
	}
	int ind = program->wind2->figures.size() - 1;
	program->wind2->figures[ind]->MoveTo(program->endPos.x, program->endPos.y, program->endPos.z);
	if (program->setQuats)
	{
		program->wind2->figures[ind]->SetRotation(program->endQuat);
	}
	else
	{
		program->wind2->figures[ind]->SetRotation(EulerToQuat(program->endAngle));
	}
}

void DrawScene(float c)
{
	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// make sure we clear the framebuffer's content
	glClearColor(c, 0.4f, 0.4f, 1.0f);
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

	GLFWwindow* window = glfwCreateWindow(DEFAULT_WIDTH, DEFAULT_HEIGHT, "Euler Sim", NULL, NULL);
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
	program->wind1 = std::make_shared<Window>();
	program->wind2 = std::make_shared<Window>();
	program->wind1->window = window;
	program->current_width = DEFAULT_WIDTH;
	program->current_height = DEFAULT_HEIGHT;
	//program->shader = new Shader("shaders/vertexShader.vs", "shaders/fragShader.fs",nullptr);
	glViewport(0, 0, DEFAULT_WIDTH, DEFAULT_HEIGHT);
	//	glfwSetInputMode(window, GLFW_STICKY_MOUSE_BUTTONS, GLFW_TRUE);
	glfwSetWindowFocusCallback(window, window_focus_callback);
	glfwSetFramebufferSizeCallback(window, proc.framebuffer_size_callback);
	glfwSetWindowSizeCallback(window, window_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	/*
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetKeyCallback(window, key_callback);
*/
	program->wind1->shader = std::make_shared<Shader>("shaders/vertexShader.vs"
		, "shaders/fragShader.fs"
		, nullptr);

	program->wind1->lightShader = std::make_shared<Shader>("shaders/lightVertexShader.vs"
		, "shaders/lightFragShader.fs"
		, nullptr);

	GLFWwindow* window2 = glfwCreateWindow(DEFAULT_WIDTH, DEFAULT_HEIGHT, "Quat Sim", NULL, NULL);
	program->wind2->window = window2;

	if (window2 == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window2);
	program->currentWindow = program->wind2;
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	glViewport(0, 0, DEFAULT_WIDTH, DEFAULT_HEIGHT);
	//	glfwSetInputMode(window, GLFW_STICKY_MOUSE_BUTTONS, GLFW_TRUE);
	glfwSetWindowFocusCallback(window2, window_focus_callback);
	glfwSetFramebufferSizeCallback(window2, proc.framebuffer_size_callback);
	glfwSetWindowSizeCallback(window2, window_size_callback);
	glfwSetCursorPosCallback(window2, mouse_callback);
	/*	glfwSetScrollCallback(window2, scroll_callback);
		glfwSetMouseButtonCallback(window2, mouse_button_callback);
		glfwSetKeyCallback(window2, key_callback);
		*/

	glEnable(GL_LINE_SMOOTH);
	program->wind2->shader = std::make_shared<Shader>("shaders/vertexShader.vs"
		, "shaders/fragShader.fs"
		, nullptr);

	program->wind2->lightShader = std::make_shared<Shader>("shaders/lightVertexShader.vs"
		, "shaders/lightFragShader.fs"
		, nullptr);

	glfwMakeContextCurrent(window);
	program->cam = std::make_shared<Camera>();
	//	program->cam = new Camera();
	program->cam->LookAt({ 0,0,30 }, { 0,0,-1 }, { 0,1,0 });


	program->currentWindow = program->wind1;
	GenerateTex();
	GenerateCube();
	program->currentWindow = program->wind2;
	GenerateTex();
	GenerateCube();

	glfwMakeContextCurrent(window);
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
			SetPositions();
		}
		float aspect = (float)program->current_width / (float)program->current_height;
		program->cam->SetPerspective(aspect);

		glfwMakeContextCurrent(window);
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		//ImGui::ShowDemoWindow();
		program->currentWindow = program->wind1;
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



void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (ImGui::GetIO().WantCaptureMouse) return;
	glm::vec2 mousePos = { xpos,ypos };
	glm::vec2 diff = mousePos - mousePosOld;
	double xDiff = (double)diff.x / program->current_width;
	double yDiff = (double)diff.y / program->current_height;
	int lShiftState = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT);
	int lAltState = glfwGetKey(window, GLFW_KEY_LEFT_ALT);
	int rAltState = glfwGetKey(window, GLFW_KEY_RIGHT_ALT);
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
	{
		if (lAltState == GLFW_PRESS)
		{
			glm::vec4 lRayStart_NDC(
				((float)mousePosOld.x / (float)program->current_width - 0.5f) * 2.0f,
				-((float)mousePosOld.y / (float)program->current_height - 0.5f) * 2.0f,
				-1.0,
				1.0f
			);
			glm::vec4 lRayEnd_NDC(
				((float)mousePosOld.x / (float)program->current_width - 0.5f) * 2.0f,
				-((float)mousePosOld.y / (float)program->current_height - 0.5f) * 2.0f,
				0.0,
				1.0f
			);

			glm::vec4 lRayStart_camera = program->cam->GetInvProjectionMatrix() * lRayStart_NDC;    lRayStart_camera /= -lRayStart_camera.w;
			glm::vec4 lRayStart_world = program->cam->GetInvViewportMatrix() * lRayStart_camera; lRayStart_world /= lRayStart_world.w;
			glm::vec4 lRayEnd_camera = program->cam->GetInvProjectionMatrix() * lRayEnd_NDC;      lRayEnd_camera /= -lRayEnd_camera.w;
			glm::vec4 lRayEnd_world = program->cam->GetInvViewportMatrix() * lRayEnd_camera;   lRayEnd_world /= lRayEnd_world.w;
			glm::vec3 lRayDir_world(lRayEnd_world - lRayStart_world);
			lRayDir_world = glm::normalize(lRayDir_world);

			float minLength = 9999.0f;
		}
		else
		{
			for (int i = 0; i < program->currentWindow->figures.size(); ++i)
			{
				if (program->currentWindow->figures[i]->CanMove())
				{
					program->currentWindow->figures[i]->MoveVec(40 * xDiff, program->cam->right);
					if (rAltState == GLFW_PRESS)
						program->currentWindow->figures[i]->MoveVec(-40 * yDiff, program->cam->front);
					else
						program->currentWindow->figures[i]->MoveVec(-40 * yDiff, program->cam->up);
				}
			}
		}
	}

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
	{
		if (lShiftState == GLFW_PRESS)
		{
			double xAngle = 2 * yDiff;
			double yAngle = 2 * xDiff;

			glm::vec3 camVec = program->cam->pos - lookAt;
			camVec = ArbitraryRotate(camVec, xAngle, program->cam->right);
			program->cam->LookAt(lookAt + camVec, ArbitraryRotate(program->cam->front, xAngle, program->cam->right), ArbitraryRotate(program->cam->up, xAngle, program->cam->right));


			camVec = program->cam->pos - lookAt;
			camVec = ArbitraryRotate(camVec, yAngle, program->cam->up);
			program->cam->LookAt(lookAt + camVec, ArbitraryRotate(program->cam->front, yAngle, program->cam->up), program->cam->up);
		}
		else
		{
			float xAngle = 2 * yDiff;
			float yAngle = 2 * xDiff;

			for (int i = 0; i < program->currentWindow->figures.size(); ++i)
			{
				if (program->currentWindow->figures[i]->CanMove())
				{
					if (rotate)
					{
						program->currentWindow->figures[i]->RotateAround(glm::vec3(0.0f, 0.0f, 0.0f), program->cam->up, yAngle);
						program->currentWindow->figures[i]->RotateAround(glm::vec3(0.0f, 0.0f, 0.0f), program->cam->right, xAngle);
					}
				}
			}
		}
	}
	mousePosOld = mousePos;
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



void window_size_callback(GLFWwindow* window, int width, int height) {
	program->current_width = width;
	program->current_height = height;
}