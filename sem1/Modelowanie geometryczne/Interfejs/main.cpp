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
#include "Camera.h"
#include "BezierCurve.h"
#include "Program.h"
#include "MiddlePoint.h"
#include "Cursor.h"
#define DEFAULT_WIDTH 1280
#define DEFAULT_HEIGHT 720


bool firstCall = true;
bool rotate = false;
glm::vec2 mousePosOld;
glm::vec3 lookAt;
Program* program;


glm::vec3 ArbitraryRotate(glm::vec3 p, float angle , glm::vec3 axis)
{
	glm::quat quat= glm::angleAxis(angle, axis);
	return quat*p;
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (ImGui::GetIO().WantCaptureMouse) return;
	glm::vec2 mousePos = { xpos,ypos };
	glm::vec2 diff = mousePos - mousePosOld;
	double xDiff = (double)diff.x / program->current_width;
	double yDiff = (double)diff.y / program->current_height;
	int lShiftState = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT);
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
	{
		if (lShiftState == GLFW_PRESS)
		{
			//TODO
			program->cur->ForceRecalcScreenPos();
		}
		else
		{
			for (int i = 0; i < program->figures.size(); ++i)
			{
				if (program->figures[i]->GetSelected() && program->figures[i]->CanMove())
				{
					program->figures[i]->MoveVec(8 * xDiff, program->cam->right);
					program->figures[i]->MoveVec(-8 * yDiff, program->cam->up);
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
			program->cur->ForceRecalcScreenPos();
		}
		else
		{
			float xAngle = 2 * yDiff;
			float yAngle = 2 * xDiff;

			for (int i = 0; i < program->figures.size(); ++i)
			{
				if (program->figures[i]->GetSelected() && program->figures[i]->CanMove())
				{
					if (rotate)
					{
						program->figures[i]->RotateAround(program->cur->GetPos(), program->cam->up, yAngle);
						program->figures[i]->RotateAround(program->cur->GetPos(), program->cam->right, xAngle);
					}
					else
					{
						program->figures[i]->RotateAround(program->mp->GetPos(), program->cam->up, yAngle);
						program->figures[i]->RotateAround(program->mp->GetPos(), program->cam->right, xAngle);
					}
				}
			}
		}
	}
	mousePosOld = mousePos;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_A && action == GLFW_PRESS)
	{
		int lCtrlState = glfwGetKey(window, GLFW_KEY_LEFT_CONTROL);
		if (lCtrlState == GLFW_PRESS)
		{
			for (int i = 0; i < program->figures.size(); ++i)
			{
				program->figures[i]->Select();
			}
		}
	}
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);
	int lCtrlState = glfwGetKey(window, GLFW_KEY_LEFT_CONTROL);
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && lCtrlState == GLFW_PRESS)
	{
		glm::vec4 lRayStart_NDC(
			((float)xpos / (float)program->current_width - 0.5f) * 2.0f,
			-((float)ypos / (float)program->current_height - 0.5f) * 2.0f,
			-1.0,
			1.0f
		);
		glm::vec4 lRayEnd_NDC(
			((float)xpos / (float)program->current_width - 0.5f) * 2.0f,
			-((float)ypos / (float)program->current_height - 0.5f) * 2.0f,
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
		int minInd = -1;

		for (int i = 0; i < program->figures.size(); ++i)
		{
			if (program->figures[i]->figureType == FigureType::Point)
			{
				glm::vec3 point = program->figures[i]->GetPos();
				glm::vec3 toPoint(point - glm::vec3(lRayStart_world));
				glm::vec3 crossp = glm::cross(lRayDir_world, toPoint);
				float length = glm::length(crossp);
				if (length < minLength && length < 0.1f)
				{
					minLength = length;
					minInd = i;
				}
			}
		}
		if (minInd > -1)
		{
			program->figures[minInd]->Select();
		}
	}
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS && lCtrlState == GLFW_PRESS)
	{
		for (int i = 0; i < program->figures.size(); ++i)
		{
			program->figures[i]->Unselect();
		}
	}

}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	if (ImGui::GetIO().WantCaptureMouse) return;
	int lShiftState = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT);
	if (lShiftState == GLFW_PRESS)
	{
		if (yoffset >= 1)
		{
			glm::mat3 scale = glm::mat3(0.9f);
			glm::vec3 camVec = program->cam->pos - lookAt;
			camVec = scale * camVec;
			program->cam->LookAt(lookAt + camVec, program->cam->front, program->cam->up);
			program->cur->ForceRecalcScreenPos();
		}
		if (yoffset <= -1)
		{
			glm::mat3 scale = glm::mat3(1.1f);
			glm::vec3 camVec = program->cam->pos - lookAt;
			camVec = scale * camVec;
			program->cam->LookAt(lookAt + camVec, program->cam->front, program->cam->up);
			program->cur->ForceRecalcScreenPos();
		}

	}
	else
	{
		for (int i = 0; i < program->figures.size(); ++i)
		{
			if (program->figures[i]->GetSelected() && program->figures[i]->CanMove())
			{
				if (yoffset >= 1)
				{
					if (rotate)
					{
						program->figures[i]->ScaleAround(program->cur->GetPos(), 1.1f);
					}
					else
					{
						program->figures[i]->ScaleAround(program->mp->GetPos(), 1.1f);
					}
				}
				if (yoffset <= -1)
				{
					if (rotate)
					{
						program->figures[i]->ScaleAround(program->cur->GetPos(), 0.9f);
					}
					else
					{
						program->figures[i]->ScaleAround(program->mp->GetPos(), 0.9f);
					}
				}
			}
		}
	}
}

void window_size_callback(GLFWwindow* window, int width, int height) {
	program->current_width = width;
	program->current_height = height;
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
	ImGui::Checkbox("Transformate around cursor", &rotate);
	program->cur->GetGui(-1, nullptr);
	if (ImGui::TreeNode("Adding"))
	{
		if (ImGui::Button("New Torus"))
		{
			Figure* f = new Torus();
			f->Initialize(program);
			auto pos = program->cur->GetPos();
			f->MoveTo(pos.x, pos.y, pos.z);
			program->figures.push_back(f);
		}
		if (ImGui::Button("New Point"))
		{
			Figure* f = new Point();
			f->Initialize(program);
			auto pos = program->cur->GetPos();
			f->MoveTo(pos.x, pos.y, pos.z);
			program->figures.push_back(f);
			for (int i = 0; i < program->figures.size(); ++i)
			{
				if (program->figures[i]->GetSelected() && program->figures[i]->figureType == FigureType::BezierCurve)
				{
					((BezierCurve*)program->figures[i])->AddPoint((Point*)f);
					((Point*)f)->AddParent(program->figures[i]);
				}
			}
		}
		if (ImGui::Button("New Bezier Curve"))
		{
			Figure* f = new BezierCurve();
			f->Initialize(program);
			program->figures.push_back(f);
			for (int i = 0; i < program->figures.size(); ++i)
			{
				if (program->figures[i]->GetSelected() && program->figures[i]->figureType == FigureType::Point)
				{
					((BezierCurve*)f)->AddPoint((Point*)program->figures[i]);
					((Point*)program->figures[i])->AddParent(f);
				}
			}
		}
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("Figures"))
	{
		for (int i = 0; i < program->figures.size(); ++i)
		{
			if (program->figures[i]->GetGui(i, nullptr))
			{
				to_delete = i;
			}
		}
		ImGui::TreePop();
	}

	if (to_delete >= 0)
	{
		delete program->figures[to_delete];
		program->figures.erase(program->figures.begin() + to_delete);
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
	program = new Program();
	program->current_width = DEFAULT_WIDTH;
	program->current_height = DEFAULT_HEIGHT;
	//program->shader = new Shader("shaders/vertexShader.vs", "shaders/fragShader.fs",nullptr);
	glViewport(0, 0, DEFAULT_WIDTH, DEFAULT_HEIGHT);
	//	glfwSetInputMode(window, GLFW_STICKY_MOUSE_BUTTONS, GLFW_TRUE);
	glfwSetFramebufferSizeCallback(window, proc.framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetWindowSizeCallback(window, window_size_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetKeyCallback(window, key_callback);
	glEnable(GL_LINE_SMOOTH); 

	program->shader= Shader("shaders/vertexShader.vs"
		, "shaders/fragShader.fs"
		, nullptr);
	program->bezierShader = Shader("shaders/bezierVertexShader.vs"
		, "shaders/fragShader.fs"
		, "shaders/bezierGeometryShader.gs");

	program->mp = new MiddlePoint();
	program->mp->Initialize(program);
	program->cur = new Cursor();
	program->cur->Initialize(program);

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	program->cam = new Camera();
	lookAt = { 0,0,0 };
	program->cam->LookAt({ 0,0,3 }, { 0,0,-1 }, { 0,1,0 });
	float aspect = (float)program->current_width / (float)program->current_height;
	program->cam->SetPerspective(aspect);
	glm::mat4 view = program->cam->GetViewportMatrix();
	glm::mat4 persp = program->cam->GetProjectionMatrix();
	glm::mat4 view2 = program->cam->GetViewportMatrix();
	glm::mat4 persp2 = program->cam->GetProjectionMatrix();
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

		//ImGui::ShowDemoWindow();
		proc.processInput(window);

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		program->shader.use();
		unsigned int perspLoc = glGetUniformLocation(program->shader.ID, "persp");
		unsigned int viewLoc = glGetUniformLocation(program->shader.ID, "view");
		persp = program->cam->GetProjectionMatrix();
		view = program->cam->GetViewportMatrix();
		glUniformMatrix4fv(perspLoc, 1, GL_FALSE, glm::value_ptr(persp));
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		program->bezierShader.use();
		perspLoc = glGetUniformLocation(program->bezierShader.ID, "persp");
		viewLoc = glGetUniformLocation(program->bezierShader.ID, "view");
		glUniformMatrix4fv(perspLoc, 1, GL_FALSE, glm::value_ptr(persp));
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

		program->mp->Reset();
		for (int i = 0; i < program->figures.size(); ++i)
		{
			program->figures[i]->RecalcFigure();
			program->figures[i]->Draw();
			program->mp->Add(program->figures[i]);
		}

		program->mp->Draw();
		program->cur->Draw();

		RenderGui();
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	for (int i = 0; i < program->figures.size(); ++i) delete program->figures[i];
	delete program->mp;
	delete program;
	return 0;
}

