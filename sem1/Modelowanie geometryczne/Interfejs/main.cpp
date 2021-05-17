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
#include "BezierCurveC0.h"
#include "BezierCurveC2.h"
#include "InterpolationCurveC2.h"
#include "Program.h"
#include "MiddlePoint.h"
#include "Cursor.h"
#include "BezierPatchC0.h"
#include "BezierPatchC0Cylinder.h"
#define DEFAULT_WIDTH 1280
#define DEFAULT_HEIGHT 720


bool firstCall = true;
bool rotate = false;
glm::vec2 mousePosOld;
glm::vec3 lookAt;
Program* program;



glm::vec3 ArbitraryRotate(glm::vec3 p, float angle, glm::vec3 axis)
{
	glm::quat quat = glm::angleAxis(angle, axis);
	return quat * p;
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
		if (lShiftState == GLFW_PRESS)
		{
			//TODO
			program->cur->ForceRecalcScreenPos();
		}
		else if (lAltState == GLFW_PRESS)
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
			int minInd_i = -1;
			int minInd_j = -1;
			for (int i = 0; i < program->figures.size(); ++i)
			{
				if (program->figures[i]->figureType == FigureType::BezierCurveC2)
				{

					for (int j = 0; j < ((BezierCurveC2*)program->figures[i])->bernsteinPoints.size(); ++j)
					{
						glm::vec3 point = ((BezierCurveC2*)program->figures[i])->bernsteinPoints[j]->GetPos();
						glm::vec3 toPoint(point - glm::vec3(lRayStart_world));
						glm::vec3 crossp = glm::cross(lRayDir_world, toPoint);
						float length = glm::length(crossp);
						if (length < minLength)
						{
							minLength = length;
							minInd_i = i;
							minInd_j = j;
						}
					}
				}
			}
			if (minInd_i != -1)
			{
				((BezierCurveC2*)program->figures[minInd_i])->bernsteinPoints[minInd_j]->MoveVec(8 * xDiff, program->cam->right);
				((BezierCurveC2*)program->figures[minInd_i])->bernsteinPoints[minInd_j]->MoveVec(-8 * yDiff, program->cam->up);
				((BezierCurveC2*)program->figures[minInd_i])->BernsteinMoved(minInd_j);
			}
		}
		else
		{
			for (int i = 0; i < program->figures.size(); ++i)
			{
				if (program->figures[i]->GetSelected() && program->figures[i]->CanMove())
				{
					program->figures[i]->MoveVec(8 * xDiff, program->cam->right);
					if (rAltState == GLFW_PRESS)
						program->figures[i]->MoveVec(-8 * yDiff, program->cam->front);
					else
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
	int lAltState = glfwGetKey(window, GLFW_KEY_LEFT_ALT);
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
	ImGui::Checkbox("Anaglyph", &program->anaglyph);
	ImGui::Checkbox("Transformate around cursor", &rotate);
	ImGui::SliderFloat("eyeDist", &program->eyeDist, 0.001f, 0.4f);
	ImGui::SliderFloat("d", &program->d, 0.005f, 5.0f);
	ImGui::SliderFloat("w", &program->w, 1.0f, 30.0f);
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
				if (program->figures[i]->GetSelected() && program->figures[i]->isCurve)
				{
					((SomeCurve*)program->figures[i])->AddPoint((Point*)f);
					((Point*)f)->AddParent(program->figures[i]);
				}
			}
		}
		if (ImGui::Button("New Bezier Curve C0"))
		{
			Figure* f = new BezierCurveC0();
			f->Initialize(program);
			program->figures.push_back(f);
			for (int i = 0; i < program->figures.size(); ++i)
			{
				if (program->figures[i]->GetSelected() && program->figures[i]->figureType == FigureType::Point)
				{
					((BezierCurveC0*)f)->AddPoint((Point*)program->figures[i]);
					((Point*)program->figures[i])->AddParent(f);
				}
			}
		}
		if (ImGui::Button("New Bezier Curve C2"))
		{
			Figure* f = new BezierCurveC2();
			f->Initialize(program);
			program->figures.push_back(f);
			for (int i = 0; i < program->figures.size(); ++i)
			{
				if (program->figures[i]->GetSelected() && program->figures[i]->figureType == FigureType::Point)
				{
					((BezierCurveC2*)f)->AddPoint((Point*)program->figures[i]);
					((Point*)program->figures[i])->AddParent(f);
				}
			}
		}
		if (ImGui::Button("New Interpolation Curve C2"))
		{
			Figure* f = new InterpolationCurveC2();
			f->Initialize(program);
			program->figures.push_back(f);
			for (int i = 0; i < program->figures.size(); ++i)
			{
				if (program->figures[i]->GetSelected() && program->figures[i]->figureType == FigureType::Point)
				{
					((InterpolationCurveC2*)f)->AddPoint((Point*)program->figures[i]);
					((Point*)program->figures[i])->AddParent(f);
				}
			}
		}
		ImGui::SliderInt("patches_n", &program->patches_n, 1, 100);
		ImGui::SliderInt("patches_m", &program->patches_m, 1, 100);
		ImGui::SliderFloat("r", &program->bezierC0r, 0.5f, 5.0f);
		ImGui::SliderFloat("length", &program->bezierC0length, 0.5f, 5.0f);
		ImGui::SliderFloat("width", &program->bezierC0width, 0.5f, 5.0f);
		if (ImGui::Button("New Bezier Patch C0 Rectangle"))
		{
			Figure* f = new BezierPatchC0(program->patches_n, program->patches_m, program->bezierC0width, program->bezierC0length);
			f->Initialize(program);
			program->figures.push_back(f);
		}

		if (ImGui::Button("New Bezier Patch C0 Cylinder"))
		{
			Figure* f = new BezierPatchC0Cylinder(program->patches_n, program->patches_m, program->bezierC0r, program->bezierC0length);
			f->Initialize(program);
			program->figures.push_back(f);
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
		Figure* f = program->figures[to_delete];
		program->figures.erase(program->figures.begin() + to_delete);
		f->CleanUp();
		delete f;
	}

	ImGui::End();
}

void CreateColorbuffer(unsigned int& framebuffer, unsigned int& textureColorbuffer)
{
	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	// create a color attachment texture
	glGenTextures(1, &textureColorbuffer);
	glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, DEFAULT_WIDTH, DEFAULT_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);
	// create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
	/*
	unsigned int rbo;
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, DEFAULT_WIDTH, DEFAULT_WIDTH); // use a single renderbuffer object for both a depth AND stencil buffer.
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo); // now actually attach it
	// now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
	*/
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;

	// draw as wireframe
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

void DrawScene()
{
	// make sure we clear the framebuffer's content
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClearColor(0.4f, 0.4f, 0.4f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	program->shader.use();
	unsigned int perspLoc = glGetUniformLocation(program->shader.ID, "persp");
	unsigned int viewLoc = glGetUniformLocation(program->shader.ID, "view");
	glm::mat4 persp = program->cam->GetProjectionMatrix();
	glm::mat4 view = program->cam->GetViewportMatrix();
	glUniformMatrix4fv(perspLoc, 1, GL_FALSE, glm::value_ptr(persp));
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	program->bezierShader.use();
	perspLoc = glGetUniformLocation(program->bezierShader.ID, "persp");
	viewLoc = glGetUniformLocation(program->bezierShader.ID, "view");
	glUniformMatrix4fv(perspLoc, 1, GL_FALSE, glm::value_ptr(persp));
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	program->patchShader.use();
	perspLoc = glGetUniformLocation(program->patchShader.ID, "persp");
	viewLoc = glGetUniformLocation(program->patchShader.ID, "view");
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
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
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

	program->shader = Shader("shaders/vertexShader.vs"
		, "shaders/fragShader.fs"
		, nullptr);
	program->bezierShader = Shader("shaders/bezierVertexShader.vs"
		, "shaders/fragShader.fs"
		, "shaders/bezierGeometryShader.gs");

	program->patchShader = Shader("shaders/patchVertexShader.vs"
		, "shaders/fragShader.fs"
		, "shaders/patchGeometryShader.gs"
		);

	program->mp = new MiddlePoint();
	program->mp->Initialize(program);
	program->cur = new Cursor();
	program->cur->Initialize(program);

	program->cam = new Camera();
	program->cam->LookAt({ 0,0,3 }, { 0,0,-1 }, { 0,1,0 });
	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	// Setup Platform/Renderer bindings
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 130");
	// Setup Dear ImGui style
	ImGui::StyleColorsDark();

#pragma region quad  
	float quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
	// positions   // texCoords
	-1.0f, 1.0f, 0.0f, 1.0f,
		-1.0f, -1.0f, 0.0f, 0.0f,
		1.0f, -1.0f, 1.0f, 0.0f,

		-1.0f, 1.0f, 0.0f, 1.0f,
		1.0f, -1.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 1.0f, 1.0f
	};
	// screen quad VAO
	unsigned int quadVAO, quadVBO;
	glGenVertexArrays(1, &quadVAO);
	glGenBuffers(1, &quadVBO);
	glBindVertexArray(quadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

	Shader screenShader("textureVertexShader.vs", "textureFragmentShader.fs");
	screenShader.use();
	screenShader.setInt("screenTexture", 0);
#pragma endregion

	unsigned int framebufferRed, textureColorbufferRed;
	CreateColorbuffer(framebufferRed, textureColorbufferRed);

	unsigned int framebufferBlue, textureColorbufferBlue;
	CreateColorbuffer(framebufferBlue, textureColorbufferBlue);

	while (!glfwWindowShouldClose(window))
	{
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		//ImGui::ShowDemoWindow();
		proc.processInput(window);
		RenderGui();

		if (program->anaglyph)
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
#pragma region red
			program->cam->SetPerspective(program->w, 2.0f * program->eyeDist, program->d, program->w / program->current_width * program->current_height, false);
			program->cam->LookAt({ program->eyeDist,0,3 }, { 0,0,-1 }, { 0,1,0 });

			glBindFramebuffer(GL_FRAMEBUFFER, framebufferRed);
			//glEnable(GL_DEPTH_TEST); // enable depth testing (is disabled for rendering screen-space quad)
			DrawScene();
#pragma endregion
#pragma region blue
			program->cam->SetPerspective(program->w, 2.0f * program->eyeDist, program->d, program->w / program->current_width * program->current_height, true);
			program->cam->LookAt({ -program->eyeDist,0,3 }, { 0,0,-1 }, { 0,1,0 });

			glBindFramebuffer(GL_FRAMEBUFFER, framebufferBlue);
			//glEnable(GL_DEPTH_TEST); // enable depth testing (is disabled for rendering screen-space quad)

			DrawScene();
#pragma endregion

			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			// now bind back to default framebuffer and draw a quad plane with the attached framebuffer color texture
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glDisable(GL_DEPTH_TEST); // disable depth test so screen-space quad isn't discarded due to depth test.
			// clear all relevant buffers
			glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // set clear color to white (not really necessary actually, since we won't be able to see behind the quad anyways)
			glClear(GL_COLOR_BUFFER_BIT);

			screenShader.use();
			glBindVertexArray(quadVAO);

			// Get the uniform variables location. You've probably already done that before...
			auto redTexLocation = glGetUniformLocation(screenShader.ID, "screenTextureRed");
			auto blueTexLocation = glGetUniformLocation(screenShader.ID, "screenTextureBlue");

			// Then bind the uniform samplers to texture units:
			glUniform1i(redTexLocation, 0);
			glUniform1i(blueTexLocation, 1);
			glActiveTexture(GL_TEXTURE0 + 0); // Texture unit 0
			glBindTexture(GL_TEXTURE_2D, textureColorbufferRed);

			glActiveTexture(GL_TEXTURE0 + 1); // Texture unit 1
			glBindTexture(GL_TEXTURE_2D, textureColorbufferBlue);
			glDrawArrays(GL_TRIANGLES, 0, 6);
		}
		else
		{
			float aspect = (float)program->current_width / (float)program->current_height;
			program->cam->SetPerspective(aspect);
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			DrawScene();
		}

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glDeleteVertexArrays(1, &quadVAO);
	glDeleteBuffers(1, &quadVBO);

	glfwTerminate();
	for (int i = 0; i < program->figures.size(); ++i) delete program->figures[i];
	delete program->mp;
	delete program;
	return 0;
}

