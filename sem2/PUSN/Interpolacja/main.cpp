#pragma once
#define STB_IMAGE_IMPLEMENTATION
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stb_image.h>
#include "processing.h"
#include "Shader.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include <glm/gtx/transform.hpp>
#include "Figure.h"
#include "Camera.h"
#include "Program.h"
#include "Cube.h"
#include "ImGuiFileDialog.h"

bool firstCall = true;
bool rotate = false;
glm::vec2 mousePosOld;
glm::vec3 lookAt;
std::shared_ptr<Program> program = {};

glm::vec3 ArbitraryRotate(glm::vec3 p, float angle, glm::vec3 axis)
{
	glm::quat quat = glm::angleAxis(angle, axis);
	return quat * p;
}

glm::quat EulerToQuat(float z1, float x, float z2)
{
	glm::vec3 xAxis = { 1,0,0 };
	glm::vec3 yAxis = { 0,1,0 };
	glm::vec3 zAxis = { 0,0,1 };

	z1 = z1 / 180.0f * M_PI;
	z2 = z2 / 180.0f * M_PI;
	x = x / 180.0f * M_PI;

	glm::quat finalQuat;
	finalQuat.x = 0.0f;
	finalQuat.y = 0.0f;
	finalQuat.z = 0.0f;
	finalQuat.w = 1.0f;
	glm::quat q;

	q = glm::angleAxis(z1, zAxis);
	xAxis = q * xAxis;
	yAxis = q * yAxis;
	zAxis = q * zAxis;
	finalQuat = q * finalQuat;

	q = glm::angleAxis(x, xAxis);
	xAxis = q * xAxis;
	yAxis = q * yAxis;
	zAxis = q * zAxis;
	finalQuat = q * finalQuat;

	q = glm::angleAxis(z2, zAxis);
	xAxis = q * xAxis;
	yAxis = q * yAxis;
	zAxis = q * zAxis;
	finalQuat = q * finalQuat;
	return finalQuat;
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

void window_size_callback(GLFWwindow* window, int width, int height) {
	program->current_width = width;
	program->current_height = height;
}

void GenerateCube()
{
	std::shared_ptr<Figure> f = std::make_shared<Cube>();
	f->Initialize(program);
	f->MoveTo(0.0f, 0.0f, 0.0f);
	f->Select();
	program->currentWindow->figures.push_back(f);
	program->cube = std::dynamic_pointer_cast<Cube>(f);
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
			ImGui::SliderFloat("Start X", &program->startX, -3.0f, 3.0f);
			ImGui::SliderFloat("Start Y", &program->startY, -3.0f, 3.0f);
			ImGui::SliderFloat("Start Z", &program->startZ, -3.0f, 3.0f);
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Start rot"))
		{
			if (!program->setQuats)
			{
				ImGui::SliderFloat("Start Z1", &program->startAngleZ1, -180.0f, 180.0f);
				ImGui::SliderFloat("Start X", &program->startAngleX, -180.0f, 180.0f);
				ImGui::SliderFloat("Start Z2", &program->startAngleZ2, -180.0f, 180.0f);
			}
			else
			{
				ImGui::SliderFloat("Start QuatX", &program->startQuatX, -1.0f, 1.0f);
				ImGui::SliderFloat("Start QuatY", &program->startQuatY, -1.0f, 1.0f);
				ImGui::SliderFloat("Start QuatZ", &program->startQuatZ, -1.0f, 1.0f);
				ImGui::SliderFloat("Start QuatW", &program->startQuatW, -1.0f, 1.0f);
			}
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("End pos"))
		{
			ImGui::SliderFloat("End X", &program->endX, -3.0f, 3.0f);
			ImGui::SliderFloat("End Y", &program->endY, -3.0f, 3.0f);
			ImGui::SliderFloat("End Z", &program->endZ, -3.0f, 3.0f);
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("End rot"))
		{
			if (!program->setQuats)
			{
				ImGui::SliderFloat("End Z1", &program->endAngleZ1, -180.0f, 180.0f);
				ImGui::SliderFloat("End X", &program->endAngleX, -180.0f, 180.0f);
				ImGui::SliderFloat("End Z2", &program->endAngleZ2, -180.0f, 180.0f);
			}
			else
			{
				ImGui::SliderFloat("End QuatX", &program->endQuatX, -1.0f, 1.0f);
				ImGui::SliderFloat("End QuatY", &program->endQuatY, -1.0f, 1.0f);
				ImGui::SliderFloat("End QuatZ", &program->endQuatZ, -1.0f, 1.0f);
				ImGui::SliderFloat("End QuatW", &program->endQuatW, -1.0f, 1.0f);
			}
			ImGui::TreePop();
		}

		if (ImGui::Button("Simulate"))
		{
			program->simulating = true;
			program->t = 0;

			if (program->setQuats)
			{
				//TODO Quats to euler
			}
			else
			{
				auto quatStart = EulerToQuat(program->startAngleZ1, program->startAngleX, program->startAngleZ2);
				auto quatEnd = EulerToQuat(program->endAngleZ1, program->endAngleX, program->endAngleZ2);

				program->startQuatX = quatStart.x;
				program->startQuatY = quatStart.y;
				program->startQuatZ = quatStart.z;
				program->startQuatW = quatStart.w;

				program->endQuatX = quatEnd.x;
				program->endQuatY = quatEnd.y;
				program->endQuatZ = quatEnd.z;
				program->endQuatW = quatEnd.w;
			}

			{
				auto start = program->startAngleZ1;
				auto end = program->endAngleZ1;
				if (end < start) end += 360.0f;

				auto diff = end - start;
				if (diff > 180.0f) diff = -(360.0f - diff);
				program->diffAngleZ1 = diff;
			}
			{
				auto start = program->startAngleX;
				auto end = program->endAngleX;
				if (end < start) end += 360.0f;

				auto diff = end - start;
				if (diff > 180.0f) diff = -(360.0f - diff);
				program->diffAngleX = diff;
			}
			{
				auto start = program->startAngleZ2;
				auto end = program->endAngleZ2;
				if (end < start) end += 360.0f;

				auto diff = end - start;
				if (diff > 180.0f) diff = -(360.0f - diff);
				program->diffAngleZ2 = diff;
			}
			{
				auto start = program->startQuatX;
				auto end = program->endQuatX;
				if (end < start) end += 2.0f;

				auto diff = end - start;
				if (diff > 1.0f) diff = -(2.0f - diff);
				program->diffQuatX = diff;
			}
			{
				auto start = program->startQuatY;
				auto end = program->endQuatY;
				if (end < start) end += 2.0f;

				auto diff = end - start;
				if (diff > 1.0f) diff = -(2.0f - diff);
				program->diffQuatY = diff;
			}
			{
				auto start = program->startQuatZ;
				auto end = program->endQuatZ;
				if (end < start) end += 2.0f;

				auto diff = end - start;
				if (diff > 1.0f) diff = -(2.0f - diff);
				program->diffQuatZ = diff;
			}
			{
				auto start = program->startQuatW;
				auto end = program->endQuatW;
				if (end < start) end += 2.0f;

				auto diff = end - start;
				if (diff > 1.0f) diff = -(2.0f - diff);
				program->diffQuatW = diff;
			}
		}
	}
	ImGui::SliderFloat("Simulation speed", &program->simSpeed, 0.0f, 10.0f);

	ImGui::End();

	program->startAngleZ1 = program->startAngleZ1 < -180.0f ? -180.0f : program->startAngleZ1;
	program->startAngleX = program->startAngleX < -180.0f ? -180.0f : program->startAngleX;
	program->startAngleZ2 = program->startAngleZ2 < -180.0f ? -180.0f : program->startAngleZ2;
	program->startAngleZ1 = program->startAngleZ1 > 180.0f ? 180.0f : program->startAngleZ1;
	program->startAngleX = program->startAngleX > 180.0f ? 180.0f : program->startAngleX;
	program->startAngleZ2 = program->startAngleZ2 > 180.0f ? 180.0f : program->startAngleZ2;

	program->endAngleZ1 = program->endAngleZ1 < -180.0f ? -180.0f : program->endAngleZ1;
	program->endAngleX = program->endAngleX < -180.0f ? -180.0f : program->endAngleX;
	program->endAngleZ2 = program->endAngleZ2 < -180.0f ? -180.0f : program->endAngleZ2;
	program->endAngleZ1 = program->endAngleZ1 > 180.0f ? 180.0f : program->endAngleZ1;
	program->endAngleX = program->endAngleX > 180.0f ? 180.0f : program->endAngleX;
	program->endAngleZ2 = program->endAngleZ2 > 180.0f ? 180.0f : program->endAngleZ2;

	program->startQuatX = program->startQuatX < -1.0f ? -1.0f : program->startQuatX;
	program->startQuatY = program->startQuatY < -1.0f ? -1.0f : program->startQuatY;
	program->startQuatZ = program->startQuatZ < -1.0f ? -1.0f : program->startQuatZ;
	program->startQuatW = program->startQuatW < -1.0f ? -1.0f : program->startQuatW;

	program->startQuatX = program->startQuatX > 1.0f ? 1.0f : program->startQuatX;
	program->startQuatY = program->startQuatY > 1.0f ? 1.0f : program->startQuatY;
	program->startQuatZ = program->startQuatZ > 1.0f ? 1.0f : program->startQuatZ;
	program->startQuatW = program->startQuatW > 1.0f ? 1.0f : program->startQuatW;

	program->endQuatX = program->endQuatX < -1.0f ? -1.0f : program->endQuatX;
	program->endQuatY = program->endQuatY < -1.0f ? -1.0f : program->endQuatY;
	program->endQuatZ = program->endQuatZ < -1.0f ? -1.0f : program->endQuatZ;
	program->endQuatW = program->endQuatW < -1.0f ? -1.0f : program->endQuatW;

	program->endQuatX = program->endQuatX > 1.0f ? 1.0f : program->endQuatX;
	program->endQuatY = program->endQuatY > 1.0f ? 1.0f : program->endQuatY;
	program->endQuatZ = program->endQuatZ > 1.0f ? 1.0f : program->endQuatZ;
	program->endQuatW = program->endQuatW > 1.0f ? 1.0f : program->endQuatW;

	if (program->startQuatX == 0.0f && program->startQuatY == 0.0f && program->startQuatZ == 0.0f && program->startQuatW == 0.0f)
		program->startQuatW = 1.0f;
	if (program->endQuatX == 0.0f && program->endQuatY == 0.0f && program->endQuatZ == 0.0f && program->endQuatW == 0.0f)
		program->endQuatW = 1.0f;


	glm::quat quat;
	quat.x = program->startQuatX;
	quat.y = program->startQuatY;
	quat.z = program->startQuatZ;
	quat.w = program->startQuatW;
	quat = glm::normalize(quat);
	program->startQuatX = quat.x;
	program->startQuatY = quat.y;
	program->startQuatZ = quat.z;
	program->startQuatW = quat.w;

	quat.x = program->endQuatX;
	quat.y = program->endQuatY;
	quat.z = program->endQuatZ;
	quat.w = program->endQuatW;
	quat = glm::normalize(quat);
	program->endQuatX = quat.x;
	program->endQuatY = quat.y;
	program->endQuatZ = quat.z;
	program->endQuatW = quat.w;
}

void Simulate()
{
	if (program->simSpeed < 0.0f) program->simSpeed = 0.0f;

	if (program->t >= 1.0f)
	{
		program->t = 1.0f;
		program->simulating = false;
	}

	float xPos = program->startX + program->t * (program->endX - program->startX);
	float yPos = program->startY + program->t * (program->endY - program->startY);
	float zPos = program->startZ + program->t * (program->endZ - program->startZ);

	for (int i = 0; i < program->wind1->figures.size(); ++i)
	{
		program->wind1->figures[i]->MoveTo(xPos, yPos, zPos);

		float angleZ1 = program->startAngleZ1 + program->t * (program->endAngleZ1 - program->startAngleZ1);
		float angleX = program->startAngleX + program->t * (program->endAngleX - program->startAngleX);
		float angleZ2 = program->startAngleZ2 + program->t * (program->endAngleZ2 - program->startAngleZ2);

		program->wind1->figures[i]->SetRotation(EulerToQuat(angleZ1, angleX, angleZ2));

	}
	for (int i = 0; i < program->wind2->figures.size(); ++i)
	{
		program->wind2->figures[i]->MoveTo(xPos, yPos, zPos);
		if (program->spherical)
		{
			//TODO spherical interpolation
		}
		else
		{
			glm::quat quatStart;
			quatStart.x = program->startQuatX;
			quatStart.y = program->startQuatY;
			quatStart.z = program->startQuatZ;
			quatStart.w = program->startQuatW;
			glm::quat quatEnd;
			quatEnd.x = program->endQuatX;
			quatEnd.y = program->endQuatY;
			quatEnd.z = program->endQuatZ;
			quatEnd.w = program->endQuatW;


			glm::quat quat = quatStart + program->t * (quatEnd - quatStart);

			program->wind2->figures[i]->SetRotation(quat);
		}
	}

	program->t += program->simSpeed / 1000;
}

void SetPositions()
{
	for (int i = 0; i < program->wind1->figures.size(); ++i)
	{
		program->wind1->figures[i]->MoveTo(program->startX, program->startY, program->startZ);
		if (program->setQuats)
		{
			glm::quat quat;
			quat.x = program->startQuatX;
			quat.y = program->startQuatY;
			quat.z = program->startQuatZ;
			quat.w = program->startQuatW;
			program->wind1->figures[i]->SetRotation(quat);
		}
		else
		{
			program->wind1->figures[i]->SetRotation(EulerToQuat(program->startAngleZ1, program->startAngleX, program->startAngleZ2));
		}
	}
	for (int i = 0; i < program->wind2->figures.size(); ++i)
	{
		program->wind2->figures[i]->MoveTo(program->endX, program->endY, program->endZ);
		if (program->setQuats)
		{
			glm::quat quat;
			quat.x = program->endQuatX;
			quat.y = program->endQuatY;
			quat.z = program->endQuatZ;
			quat.w = program->endQuatW;
			program->wind2->figures[i]->SetRotation(quat);
		}
		else
		{
			program->wind2->figures[i]->SetRotation(EulerToQuat(program->endAngleZ1, program->endAngleX, program->endAngleZ2));
		}
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
		, "shaders/lightGeometryShader.gs");

	program->wind1->bezierShader = std::make_shared<Shader>("shaders/bezierVertexShader.vs"
		, "shaders/fragShader.fs"
		, "shaders/bezierGeometryShader.gs");

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
		, "shaders/lightGeometryShader.gs");

	program->wind2->bezierShader = std::make_shared<Shader>("shaders/bezierVertexShader.vs"
		, "shaders/fragShader.fs"
		, "shaders/bezierGeometryShader.gs");
	glfwMakeContextCurrent(window);
	program->cam = std::make_shared<Camera>();
	//	program->cam = new Camera();
	program->cam->LookAt({ 0,0,10 }, { 0,0,-1 }, { 0,1,0 });

	program->currentWindow = program->wind1;
	GenerateCube();
	program->currentWindow = program->wind2;
	GenerateCube();

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	// Setup Platform/Renderer bindings
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 130");
	// Setup Dear ImGui style
	ImGui::StyleColorsDark();


	double lastTime = glfwGetTime();

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



