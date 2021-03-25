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
#include "Camera.h"
#include "BezierCurve.h"
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
Camera* cam;
glm::vec3 lookAt;

glm::mat4 ArbitraryRotationMatrix(glm::vec3 v, float a)
{
	glm::mat4 ret = glm::mat4(1.0f);
	float t = 1.0f - cos(a);
	float s = sin(a);
	float c = cos(a);
	ret[0][0] = t * v.x * v.x + c;
	ret[0][1] = t * v.x * v.y - s * v.z;
	ret[0][2] = t * v.x * v.z + s * v.y;

	ret[1][0] = t * v.x * v.y + s * v.z;
	ret[1][1] = t * v.y * v.y + c;
	ret[1][2] = t * v.y * v.z - s * v.x;

	ret[2][0] = t * v.x * v.z - s * v.y;
	ret[2][1] = t * v.y * v.z + s * v.x;
	ret[2][2] = t * v.z * v.z + c;
	return ret;
}

//via http://paulbourke.net/geometry/rotate/source.c
glm::vec3 ArbitraryRotate(glm::vec3 p, double theta, glm::vec3 r)
{
	glm::vec3 q = { 0.0,0.0,0.0 };
	double costheta, sintheta;

	glm::normalize(r);
	costheta = cos(theta);
	sintheta = sin(theta);

	q.x += (costheta + (1 - costheta) * r.x * r.x) * p.x;
	q.x += ((1 - costheta) * r.x * r.y - r.z * sintheta) * p.y;
	q.x += ((1 - costheta) * r.x * r.z + r.y * sintheta) * p.z;

	q.y += ((1 - costheta) * r.x * r.y + r.z * sintheta) * p.x;
	q.y += (costheta + (1 - costheta) * r.y * r.y) * p.y;
	q.y += ((1 - costheta) * r.y * r.z - r.x * sintheta) * p.z;

	q.z += ((1 - costheta) * r.x * r.z - r.y * sintheta) * p.x;
	q.z += ((1 - costheta) * r.y * r.z + r.x * sintheta) * p.y;
	q.z += (costheta + (1 - costheta) * r.z * r.z) * p.z;

	return q;
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (ImGui::GetIO().WantCaptureMouse) return;
	glm::vec2 mousePos = { xpos,ypos };
	glm::vec2 diff = mousePos - mousePosOld;
	double xDiff = (double)diff.x / current_width;
	double yDiff = (double)diff.y / current_height;
	int lShiftState = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT);
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
	{
		if (lShiftState == GLFW_PRESS)
		{
			//TODO
			cur->ForceRecalcScreenPos();
		}
		else
		{
			for (int i = 0; i < figures.size(); ++i)
			{
				if (figures[i]->GetSelected() && figures[i]->CanMove())
				{
					figures[i]->MoveVec(8 * xDiff, cam->right);
					figures[i]->MoveVec(-8 * yDiff, cam->up);
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

			glm::vec3 camVec = cam->pos - lookAt;
			camVec = ArbitraryRotate(camVec, xAngle, cam->right);
			cam->LookAt(lookAt + camVec, ArbitraryRotate(cam->front, xAngle, cam->right), ArbitraryRotate(cam->up, xAngle, cam->right));


			camVec = cam->pos - lookAt;
			camVec = ArbitraryRotate(camVec, yAngle, cam->up);
			cam->LookAt(lookAt + camVec, ArbitraryRotate(cam->front, yAngle, cam->up), cam->up);
			cur->ForceRecalcScreenPos();
		}
		else
		{
			glm::mat4 rotX = glm::mat4(1.0f);
			glm::mat4 rotY = glm::mat4(1.0f);

			float xAngle = 2 * yDiff;
			float yAngle = 2 * xDiff;


			glm::mat4 rot1 = ArbitraryRotationMatrix(cam->up, -yAngle);
			glm::mat4 rot2 = ArbitraryRotationMatrix(cam->right, -xAngle);
			glm::mat4 m_rotate = rot2 * rot1;

			for (int i = 0; i < figures.size(); ++i)
			{
				if (figures[i]->GetSelected() && figures[i]->CanMove())
				{
					if (rotate)
					{
						figures[i]->RotateAroundWithMtx(cur->GetPos(), m_rotate);
					}
					else
					{
						figures[i]->RotateAroundWithMtx(mp->GetPos(), m_rotate);
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
			for (int i = 0; i < figures.size(); ++i)
			{
				figures[i]->Select();
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
			((float)xpos / (float)current_width - 0.5f) * 2.0f,
			-((float)ypos / (float)current_height - 0.5f) * 2.0f,
			-1.0,
			1.0f
		);
		glm::vec4 lRayEnd_NDC(
			((float)xpos / (float)current_width - 0.5f) * 2.0f,
			-((float)ypos / (float)current_height - 0.5f) * 2.0f,
			0.0,
			1.0f
		);

		glm::vec4 lRayStart_camera = cam->GetInvProjectionMatrix() * lRayStart_NDC;    lRayStart_camera /= -lRayStart_camera.w;
		glm::vec4 lRayStart_world = cam->GetInvViewportMatrix() * lRayStart_camera; lRayStart_world /= lRayStart_world.w;
		glm::vec4 lRayEnd_camera = cam->GetInvProjectionMatrix() * lRayEnd_NDC;      lRayEnd_camera /= -lRayEnd_camera.w;
		glm::vec4 lRayEnd_world = cam->GetInvViewportMatrix() * lRayEnd_camera;   lRayEnd_world /= lRayEnd_world.w;
		glm::vec3 lRayDir_world(lRayEnd_world - lRayStart_world);
		lRayDir_world = glm::normalize(lRayDir_world);

		float minLength = 9999.0f;
		int minInd = -1;

		for (int i = 0; i < figures.size(); ++i)
		{
			if (figures[i]->figureType == FigureType::Point)
			{
				glm::vec3 point = figures[i]->GetPos();
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
			figures[minInd]->Select();
		}
	}
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS && lCtrlState == GLFW_PRESS)
	{
		for (int i = 0; i < figures.size(); ++i)
		{
			figures[i]->Unselect();
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
			glm::vec3 camVec = cam->pos - lookAt;
			camVec = scale * camVec;
			cam->LookAt(lookAt + camVec, cam->front, cam->up);
			cur->ForceRecalcScreenPos();
		}
		if (yoffset <= -1)
		{
			glm::mat3 scale = glm::mat3(1.1f);
			glm::vec3 camVec = cam->pos - lookAt;
			camVec = scale * camVec;
			cam->LookAt(lookAt + camVec, cam->front, cam->up);
			cur->ForceRecalcScreenPos();
		}

	}
	else
	{
		for (int i = 0; i < figures.size(); ++i)
		{
			if (figures[i]->GetSelected() && figures[i]->CanMove())
			{
				if (yoffset >= 1)
				{
					if (rotate)
					{
						figures[i]->ScaleAround(cur->GetPos(), 1.1f);
					}
					else
					{
						figures[i]->ScaleAround(mp->GetPos(), 1.1f);
					}
				}
				if (yoffset <= -1)
				{
					if (rotate)
					{
						figures[i]->ScaleAround(cur->GetPos(), 0.9f);
					}
					else
					{
						figures[i]->ScaleAround(mp->GetPos(), 0.9f);
					}
				}
			}
		}
	}
}

void window_size_callback(GLFWwindow* window, int width, int height) {
	current_width = width;
	current_height = height;
	cur->cur_width = width;
	cur->cur_height = height;
}


void RenderGui(Shader& shader)
{
	if (firstCall)
	{
		ImGui::SetNextWindowSize(ImVec2(250, 400), 0);
		firstCall = false;
	}

	int to_delete = -1;

	ImGui::Begin("Menu");
	ImGui::Checkbox("Transformate around cursor", &rotate);
	cur->GetGui(-1, figures, true);
	if (ImGui::TreeNode("Adding"))
	{
		if (ImGui::Button("New Torus"))
		{
			Figure* f = new Torus(shader);
			f->Initialize();
			auto pos = cur->GetPos();
			f->MoveTo(pos.x, pos.y, pos.z);
			figures.push_back(f);
		}
		if (ImGui::Button("New Point"))
		{
			Figure* f = new Point(shader);
			f->Initialize();
			auto pos = cur->GetPos();
			f->MoveTo(pos.x, pos.y, pos.z);
			figures.push_back(f);
			for (int i = 0; i < figures.size(); ++i)
			{
				if (figures[i]->GetSelected() && figures[i]->figureType == FigureType::BezierCurve)
				{
					((BezierCurve*)figures[i])->AddPoint((Point*)f);
					((Point*)f)->AddParent(figures[i]);
				}
			}
		}
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("Figures"))
	{
		for (int i = 0; i < figures.size(); ++i)
		{
			if (figures[i]->GetGui(i, figures, true))
			{
				to_delete = i;
			}
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
	//	glfwSetInputMode(window, GLFW_STICKY_MOUSE_BUTTONS, GLFW_TRUE);
	glfwSetFramebufferSizeCallback(window, proc.framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetWindowSizeCallback(window, window_size_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetKeyCallback(window, key_callback);


	Shader shader("shaders/vertexShader.vs", "shaders/fragShader.fs");

	figures.push_back(new BezierCurve(shader));
	figures[0]->Initialize();
	mp = new MiddlePoint(shader);
	mp->Initialize();
	cur = new Cursor(shader);
	cur->Initialize();

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	cam = new Camera();
	lookAt = { 0,0,0 };
	cam->LookAt({ 0,0,3 }, { 0,0,-1 }, { 0,1,0 });
	float aspect = (float)current_width / (float)current_height;
	cam->SetPerspective(aspect);
	glm::mat4 view = cam->GetViewportMatrix();
	glm::mat4 persp = cam->GetProjectionMatrix();

	cur->cam = cam;
	cur->cur_width = current_width;
	cur->cur_height = current_height;

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

	//	ImGui::ShowDemoWindow();
		proc.processInput(window);

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		unsigned int perspLoc = glGetUniformLocation(shader.ID, "persp");
		unsigned int viewLoc = glGetUniformLocation(shader.ID, "view");
		unsigned int transLoc = glGetUniformLocation(shader.ID, "transform");
		persp = cam->GetProjectionMatrix();
		view = cam->GetViewportMatrix();
		glUniformMatrix4fv(perspLoc, 1, GL_FALSE, glm::value_ptr(persp));
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

		mp->Reset();
		for (int i = 0; i < figures.size(); ++i)
		{
			figures[i]->RecalcFigure();
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

