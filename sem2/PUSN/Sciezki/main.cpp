#pragma once
#include <iostream>
#include <algorithm>
#include <execution>
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
#include "BezierPatchC2.h"
#include "Tools.h"
#include "GregoryPatch.h"
#include "Serialization.h"
#include "Intersections.h"
#include <iomanip>

#include <chrono>
using namespace std::chrono;

bool firstCall = true;
bool rotate = false;
glm::vec2 mousePosOld;
glm::vec3 lookAt;
GLFWwindow* window;
Program* program;

void ResetParameters()
{
	program->startPointsSplits = 10;
	program->cursorPointsSplits = 10;
	program->closestPointsStartWindowSize = 0.1;
	program->newtonD = 0.1f;
	program->stopSearchingPointsLoopD = 1e-3;
	program->stopSearchingPointsFarD = 0.1f;
	program->connectLineToWallEps = 1e-3f;
	program->pointsDistNewton = 1e-3;
}

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

	if (ImGui::TreeNode("Small moves"))
	{
		ImGui::SliderFloat("smallMoveF", &program->smallMoveF, -100, 100);

		if (ImGui::Button("X Move"))
		{
			for (int i = 0; i < program->figures.size(); ++i)
			{
				if (program->figures[i]->GetSelected() && program->figures[i]->CanMove())
				{
					program->figures[i]->MoveVec(program->smallMoveF, glm::vec3{ 1.0f,0.0f,0.0f });
				}
			}
		}

		if (ImGui::Button("Y Move"))
		{
			for (int i = 0; i < program->figures.size(); ++i)
			{
				if (program->figures[i]->GetSelected() && program->figures[i]->CanMove())
				{
					program->figures[i]->MoveVec(program->smallMoveF, glm::vec3{ 0.0f,1.0f,0.0f });
				}
			}
		}

		if (ImGui::Button("Z Move"))
		{
			for (int i = 0; i < program->figures.size(); ++i)
			{
				if (program->figures[i]->GetSelected() && program->figures[i]->CanMove())
				{
					program->figures[i]->MoveVec(program->smallMoveF, glm::vec3{ 0.0f,0.0f,1.0f });
				}
			}
		}

		if (ImGui::Button("X Rot"))
		{
			for (int i = 0; i < program->figures.size(); ++i)
			{
				if (program->figures[i]->GetSelected() && program->figures[i]->CanMove())
				{
					program->figures[i]->RotateAround(program->mp->GetPos(), glm::vec3{ 1.0f,0.0f,0.0f }, program->smallMoveF);
				}
			}
		}

		if (ImGui::Button("Y Rot"))
		{
			for (int i = 0; i < program->figures.size(); ++i)
			{
				if (program->figures[i]->GetSelected() && program->figures[i]->CanMove())
				{
					program->figures[i]->RotateAround(program->mp->GetPos(), glm::vec3{ 0.0f,1.0f,0.0f }, program->smallMoveF);
				}
			}
		}

		if (ImGui::Button("Z Rot"))
		{
			for (int i = 0; i < program->figures.size(); ++i)
			{
				if (program->figures[i]->GetSelected() && program->figures[i]->CanMove())
				{
					program->figures[i]->RotateAround(program->mp->GetPos(), glm::vec3{ 0.0f,0.0f,1.0f }, program->smallMoveF);
				}
			}
		}
		ImGui::TreePop();

	}
	if (ImGui::TreeNode("Tools"))
	{
		if (ImGui::Button("Move to 0,0"))
		{
			for (int i = 0; i < program->figures.size(); ++i)
			{
				program->figures[i]->Select();
			}
			program->mp->Reset();
			for (int i = 0; i < program->figures.size(); ++i)
			{
				program->figures[i]->RecalcFigure();
				program->figures[i]->Draw();
				program->mp->Add(program->figures[i]);
			}
			program->mp->Draw();
			glm::vec3 pos = program->mp->GetPos();
			for (int i = 0; i < program->figures.size(); ++i)
			{
				if (program->figures[i]->CanMove())
					program->figures[i]->MoveVec(1.0f, -pos);
			}
		}
		ImGui::SliderInt("split", &program->split, 100, 10000);

		if (ImGui::Button("Aaa"))
		{
			float maxX = 0.0f, minX = 0.0f;
			float maxY = 0.0f, minY = 0.0f;
			float maxZ = 0.0f, minZ = 0.0f;
			std::vector<int> v1;
			for (int i = 0; i < program->split; ++i)
				v1.push_back(i);
			auto start = high_resolution_clock::now();
			for (int i = 0; i < program->figures.size(); ++i)
			{
				if (program->figures[i]->figureType == FigureType::BezierPatchC0 ||
					program->figures[i]->figureType == FigureType::BezierPatchC2)
				{
					std::mutex mutex;

					std::for_each(
						std::execution::par_unseq,
						v1.begin(),
						v1.end(),
						[&](auto&& i1)
						{

							float maxx = 0.0f, minx = 0.0f;
							float maxy = 0.0f, miny = 0.0f;
							float maxz = 0.0f, minz = 0.0f;

							for (int j = 0; j < program->split; ++j) {
								auto pos = GetPos(program->figures[i], j * 1.0f / (float)program->split, i1 * 1.0f / (float)program->split);
								if (pos.x < minx) { minx = pos.x; } if (pos.x > maxx) { maxx = pos.x; }
								if (pos.y < miny) { miny = pos.y; } if (pos.y > maxy) { maxy = pos.y; }
								if (pos.z < minz) { minz = pos.z; } if (pos.z > maxz) { maxz = pos.z; }
							}

							{
								auto ul = std::unique_lock{ mutex };
								if (minx < minX) { minX = minx; } if (maxx > maxX) { maxX = maxx; }
								if (miny < minY) { minY = miny; } if (maxy > maxY) { maxY = maxy; }
								if (minz < minZ) { minZ = minz; } if (maxz > maxZ) { maxZ = maxz; }
							}
						});
				}
			}
			auto stop = high_resolution_clock::now();
			auto duration = duration_cast<seconds>(stop - start);
#pragma region createPoints
			glfwSetWindowTitle(window, (std::to_string(duration.count())
				+ " xMin: " + std::to_string(minX)
				+ " yMin: " + std::to_string(minY)
				+ " zMin: " + std::to_string(minZ)
				+ " xMax: " + std::to_string(maxX)
				+ " yMax: " + std::to_string(maxY)
				+ " zMax: " + std::to_string(maxZ)
				).c_str());

			Figure* f = new Point();
			((Point*)f)->special = true;
			f->Initialize(program);
			f->MoveTo(maxX, 0.0f, 0.0f);
			sprintf_s(f->name, STRMAX, "MaxX");
			program->figures.push_back(f);
			f = new Point();
			((Point*)f)->special = true;
			f->Initialize(program);
			f->MoveTo(minX, 0.0f, 0.0f);
			sprintf_s(f->name, STRMAX, "MinX");
			program->figures.push_back(f);
			f = new Point();
			((Point*)f)->special = true;
			f->Initialize(program);
			f->MoveTo(0.0f, maxY, 0.0f);
			program->figures.push_back(f);
			sprintf_s(f->name, STRMAX, "MaxY");
			f = new Point();
			((Point*)f)->special = true;
			f->Initialize(program);
			f->MoveTo(0.0f, minY, 0.0f);
			sprintf_s(f->name, STRMAX, "MinY");
			program->figures.push_back(f);
			f = new Point();
			((Point*)f)->special = true;
			f->Initialize(program);
			f->MoveTo(0.0f, 0.0f, minZ);
			sprintf_s(f->name, STRMAX, "MinZ");
			program->figures.push_back(f);
			f = new Point();
			((Point*)f)->special = true;
			f->Initialize(program);
			f->MoveTo(0.0f, 0.0f, maxZ);
			sprintf_s(f->name, STRMAX, "MaxZ");
			program->figures.push_back(f);

#pragma endregion

			minX = -minX;
			minY = -minY;
			minZ = -minZ;
			maxX = maxX > minX ? maxX : minX;
			maxY = maxY > minY ? maxY : minY;
			maxZ = maxZ > minZ ? maxZ : minZ;

			for (int i = 0; i < program->figures.size(); ++i)
			{
				if (program->figures[i]->CanMove())
				{
					program->figures[i]->ScaleAroundX(program->cur->GetPos(), 65.0f / maxX);
					program->figures[i]->ScaleAroundZ(program->cur->GetPos(), 65.0f / maxZ);
					program->figures[i]->ScaleAroundY(program->cur->GetPos(), 30.0f / maxY);
				}
			}

		}
		if (ImGui::Button("Bbb"))
		{
			auto start = high_resolution_clock::now();

			std::vector<float> v;
			if (program->split % 2 == 0)program->split++;
			v.resize(program->split * program->split);

			for (int i = 0; i < program->split * program->split; ++i)
				v[i] = 0;

			float cellSize = 150.0f / (program->split - 1);
			std::vector<int> v1;
			for (int i = 0; i < program->split; ++i)
				v1.push_back(i);

			for (int i = 0; i < program->figures.size(); ++i)
				if (program->figures[i]->figureType == FigureType::BezierPatchC0 ||
					program->figures[i]->figureType == FigureType::BezierPatchC2)
				{
					std::for_each(
						//std::execution::par_unseq,
						std::execution::seq,
						v1.begin(),
						v1.end(),
						[&](auto&& i1)
						{
							for (int j = 0; j < program->split; ++j)
							{
								auto pos = GetPos(program->figures[i], j * 1.0f / (float)program->split, i1 * 1.0f / (float)program->split);
								int ii = (int)round(pos.x / cellSize) + (program->split - 1) / 2;
								int jj = (int)round(pos.z / cellSize) + (program->split - 1) / 2;
								if (pos.y > v[ii * program->split + jj])
									v[ii * program->split + jj] = pos.y;

							}
						});
				}
			auto stop = high_resolution_clock::now();
			auto duration = duration_cast<seconds>(stop - start);
			start = stop;

			int split2 = 41;
			int r = ceil(8.0f / cellSize);
			float cellSize2 = 150.0f / (split2 - 1);
			std::vector<glm::vec3> points1;
			std::vector<glm::vec3> points2;
			for (int i = 0; i < split2; ++i)
			{
				for (int j = 0; j < split2; ++j)
				{
					float h1 = 19.0f;
					float h2 = 2.0f;
					int a = 0;
					float x = i * cellSize2 - 75.0f;
					float z = j * cellSize2 - 75.0f;
					int ii = (int)round(x / cellSize) + (program->split - 1) / 2;
					int jj = (int)round(z / cellSize) + (program->split - 1) / 2;
					for (int k = ii - r; k < ii + r; ++k)
						for (int l = jj - r; l < jj + r; ++l)
						{
							if (k < 0 || l < 0 || k >= program->split || l >= program->split) continue;

							float curh = v[k * program->split + l];
							if (curh + 2 > h1) h1 = curh + 2;
							if (curh + 2 > h2) h2 = curh + 2;
						}

					Figure* f = new Point();
					((Point*)f)->special = true;
					f->Initialize(program);
					f->MoveTo(x, h1, z);
					points1.push_back(glm::vec3(x, h1, z));
					//program->figures.push_back(f);

					f = new Point();
					((Point*)f)->special = true;
					f->Initialize(program);
					f->MoveTo(x, h2, z);
					points2.push_back(glm::vec3(x, h2, z));
					//	program->figures.push_back(f);
				}
			}

			stop = high_resolution_clock::now();
			auto duration2 = duration_cast<seconds>(stop - start);
			glfwSetWindowTitle(window, ("Mapa: " + std::to_string(duration.count()) + " Punkty: " + std::to_string(duration2.count())).c_str());

			std::ofstream out_file;
			out_file.open("paths.k16");

			int n = 0;

			out_file << std::fixed << std::setprecision(3) << "N" << n << "G01X" << 0.0f << "Y" << 0.0f << "Z" << 70.0f << std::endl;
			n++;
			out_file << std::fixed << std::setprecision(3) << "N" << n << "G01X" << -75.0f << "Y" << -75.0f << "Z" << 70.0f << std::endl;
			n++;
			for (int i = 0; i < split2; ++i)
			{
				for (int jj = 0; jj < split2; ++jj)
				{
					int j = jj;
					if (i % 2 == 1)
					{
						j = split2 - jj - 1;
					}
					auto pos = points1[i * split2 + j];
					out_file << std::fixed << std::setprecision(3) << "N" << n << "G01X" << pos.x << "Y" << pos.z << "Z" << pos.y + 16.0f << std::endl;
					n++;

				}
			}
			out_file << std::fixed << std::setprecision(3) << "N" << n << "G01X" << 75.0f << "Y" << 75.0f << "Z" << 60.0f << std::endl;
			n++;
			out_file << std::fixed << std::setprecision(3) << "N" << n << "G01X" << -75.0f << "Y" << -75.0f << "Z" << 60.0f << std::endl;
			n++;
			for (int i = 0; i < split2; ++i)
			{
				for (int jj = 0; jj < split2; ++jj)
				{
					int j = jj;
					if (i % 2 == 1)
					{
						j = split2 - jj - 1;
					}
					auto pos = points2[i * split2 + j];
					out_file << std::fixed << std::setprecision(3) << "N" << n << "G01X" << pos.x << "Y" << pos.z << "Z" << pos.y + 15.0f << std::endl;
					n++;

				}
			}
			out_file.close();

			r = ceil(8.0f / cellSize);
			n = 0;
			bool goingBack = false;
			int mid = (split2 - 1) / 2;
			int j, i;
			points1.clear();
			points1.push_back(glm::vec3(0.0f, 50.0f, 0.0f));
			points1.push_back(glm::vec3(-85.0f, 50.0f, -85.0f));
			points1.push_back(glm::vec3(-85.0f, 0.0f, -85.0f));
#pragma region part1
			j = 0;

			for (i = 0; i < split2; ++i)
			{
				if (!goingBack)
				{
					bool isOk = true;
					while (j <= mid && isOk)
					{
						isOk = true;
						float x = i * cellSize2 - 75.0f;
						float z = j * cellSize2 - 75.0f;
						int ii = (int)round(x / cellSize) + (program->split - 1) / 2;
						int jj = (int)round(z / cellSize) + (program->split - 1) / 2;
						for (int k = ii - r; k < ii + r; ++k)
							for (int l = jj - r; l < jj + r; ++l)
							{
								if (k < 0 || l < 0 || k >= program->split || l >= program->split) continue;
								float curh = v[k * program->split + l];
								if (curh > 0) isOk = false;
							}
						if (isOk)
						{
							points1.push_back(glm::vec3(x, 0.0f, z));
							j++;
						}
						else j--;
					}
					goingBack = true;
					if (j > mid) j = mid;
				}
				else
				{
					int state = 0;//0 - repositioning, 1 - go forward, 2 return
					bool wasOk = true;
					bool isOk = true;
					while (j >= 0)
					{
						isOk = true;
						float x = i * cellSize2 - 75.0f;
						float z = j * cellSize2 - 75.0f;
						int ii = (int)round(x / cellSize) + (program->split - 1) / 2;
						int jj = (int)round(z / cellSize) + (program->split - 1) / 2;
						for (int k = ii - r; k < ii + r; ++k)
							for (int l = jj - r; l < jj + r; ++l)
							{
								if (k < 0 || l < 0 || k >= program->split || l >= program->split) continue;
								float curh = v[k * program->split + l];
								if (curh > 0) isOk = false;
							}
						if (state == 0)
						{
							if (isOk)
							{
								state = 1;
								points1.push_back(glm::vec3(x, 0.0f, z));
								j++;
								if (j > mid)
								{
									j -= 2;
									state = 2;
								}
							}
							else
							{
								x = (i - 1) * cellSize2 - 75.0f;
								j--;
								z = j * cellSize2 - 75.0f;
								points1.push_back(glm::vec3(x, 0.0f, z));
							}
						}
						else if (state == 1)
						{
							if (isOk)
							{
								points1.push_back(glm::vec3(x, 0.0f, z));
								j++;
								if (j > mid)
								{
									state = 2;
									j -= 2;
								}
							}
							else
							{
								state = 2;
								j -= 2;
								isOk = true;
							}
						}
						else
						{
							float x2 = (i - 1) * cellSize2 - 75.0f;
							if (isOk)
								if (wasOk)
									points1.push_back(glm::vec3(x, 0.0f, z));
								else
								{
									points1.push_back(glm::vec3(x2, 0.0f, z));
									points1.push_back(glm::vec3(x, 0.0f, z));
								}
							else
								if (wasOk)
								{
									points1.push_back(glm::vec3(x, 0.0f, z));
									points1.push_back(glm::vec3(x2, 0.0f, z));
								}
								else
									points1.push_back(glm::vec3(x2, 0.0f, z));
							j--;
						}

						wasOk = isOk;
					}
					goingBack = false;
					j = 0;
				}
			}
#pragma endregion
			points1.push_back(glm::vec3(75.0f, 50.0f, -75.0f));
			points1.push_back(glm::vec3(-85.0f, 50.0f, 85.0f));
			points1.push_back(glm::vec3(-85.0f, 0.0f, 85.0f));
#pragma region part2
			j = split2 - 1;

			for (i = 0; i < split2; ++i)
			{
				if (!goingBack)
				{
					bool isOk = true;
					while (j >= mid && isOk)
					{
						isOk = true;
						float x = i * cellSize2 - 75.0f;
						float z = j * cellSize2 - 75.0f;
						int ii = (int)round(x / cellSize) + (program->split - 1) / 2;
						int jj = (int)round(z / cellSize) + (program->split - 1) / 2;
						for (int k = ii - r; k < ii + r; ++k)
							for (int l = jj - r; l < jj + r; ++l)
							{
								if (k < 0 || l < 0 || k >= program->split || l >= program->split) continue;
								float curh = v[k * program->split + l];
								if (curh > 0) isOk = false;
							}
						if (isOk)
						{
							points1.push_back(glm::vec3(x, 0.0f, z));
							j--;
						}
						else j++;
					}
					goingBack = true;
					if (j < mid) j = mid;
				}
				else
				{
					int state = 0;//0 - repositioning, 1 - go forward, 2 return
					bool isOk = true;
					bool wasOk = true;
					while (j < split2)
					{
						isOk = true;
						float x = i * cellSize2 - 75.0f;
						float z = j * cellSize2 - 75.0f;
						int ii = (int)round(x / cellSize) + (program->split - 1) / 2;
						int jj = (int)round(z / cellSize) + (program->split - 1) / 2;
						for (int k = ii - r; k < ii + r; ++k)
							for (int l = jj - r; l < jj + r; ++l)
							{
								if (k < 0 || l < 0 || k >= program->split || l >= program->split) continue;
								float curh = v[k * program->split + l];
								if (curh > 0) isOk = false;
							}
						if (state == 0)
						{
							if (isOk)
							{
								state = 1;
								points1.push_back(glm::vec3(x, 0.0f, z));
								j--;
								if (j < mid)
								{
									j += 2;
									state = 2;
								}
							}
							else
							{
								x = (i - 1) * cellSize2 - 75.0f;
								j++;
								z = j * cellSize2 - 75.0f;
								points1.push_back(glm::vec3(x, 0.0f, z));
							}
						}
						else if (state == 1)
						{
							if (isOk)
							{
								points1.push_back(glm::vec3(x, 0.0f, z));
								j--;
								if (j < mid)
								{
									state = 2;
									j += 2;
								}
							}
							else
							{
								state = 2;
								j += 2;
								isOk = true;
							}
						}
						else
						{
							float x2 = (i - 1) * cellSize2 - 75.0f;
							if (isOk)
								if (wasOk)
									points1.push_back(glm::vec3(x, 0.0f, z));
								else
								{
									points1.push_back(glm::vec3(x2, 0.0f, z));
									points1.push_back(glm::vec3(x, 0.0f, z));
								}
							else
								if (wasOk)
								{
									points1.push_back(glm::vec3(x, 0.0f, z));
									points1.push_back(glm::vec3(x2, 0.0f, z));
								}
								else
									points1.push_back(glm::vec3(x2, 0.0f, z));

							j++;
						}

						wasOk = isOk;
					}
					goingBack = false;
					j = split2 - 1;
				}
			}
#pragma endregion

			points1.push_back(glm::vec3(75.0f, 50.0f, 75.0f));
			points1.push_back(glm::vec3(-85.0f, 50.0f, -85.0f));
			points1.push_back(glm::vec3(-85.0f, 0.0f, -85.0f));
#pragma region part3
			i = 0;

			for (int j = 0; j < split2; ++j)
			{
				if (!goingBack)
				{
					bool isOk = true;
					while (i <= mid && isOk)
					{
						isOk = true;
						float x = i * cellSize2 - 75.0f;
						float z = j * cellSize2 - 75.0f;
						int ii = (int)round(x / cellSize) + (program->split - 1) / 2;
						int jj = (int)round(z / cellSize) + (program->split - 1) / 2;
						for (int k = ii - r; k < ii + r; ++k)
							for (int l = jj - r; l < jj + r; ++l)
							{
								if (k < 0 || l < 0 || k >= program->split || l >= program->split) continue;
								float curh = v[k * program->split + l];
								if (curh > 0) isOk = false;
							}
						if (isOk)
						{
							points1.push_back(glm::vec3(x, 0.0f, z));
							i++;
						}
						else i--;
					}
					goingBack = true;
					if (i > mid) i = mid;
				}
				else
				{
					int state = 0;//0 - repositioning, 1 - go forward, 2 return
					bool isOk = true;
					bool wasOk = true;
					while (i >= 0)
					{
						isOk = true;
						float x = i * cellSize2 - 75.0f;
						float z = j * cellSize2 - 75.0f;
						int ii = (int)round(x / cellSize) + (program->split - 1) / 2;
						int jj = (int)round(z / cellSize) + (program->split - 1) / 2;
						for (int k = ii - r; k < ii + r; ++k)
							for (int l = jj - r; l < jj + r; ++l)
							{
								if (k < 0 || l < 0 || k >= program->split || l >= program->split) continue;
								float curh = v[k * program->split + l];
								if (curh > 0) isOk = false;
							}
						if (state == 0)
						{
							if (isOk)
							{
								state = 1;
								points1.push_back(glm::vec3(x, 0.0f, z));
								i++;
								if (i > mid)
								{
									i -= 2;
									state = 2;
								}
							}
							else
							{
								z = (j - 1) * cellSize2 - 75.0f;
								i--;
								x = i * cellSize2 - 75.0f;
								points1.push_back(glm::vec3(x, 0.0f, z));
							}
						}
						else if (state == 1)
						{
							if (isOk)
							{
								points1.push_back(glm::vec3(x, 0.0f, z));
								i++;
								if (i > mid)
								{
									state = 2;
									i -= 2;
								}
							}
							else
							{
								state = 2;
								i -= 2;
								isOk = true;
							}
						}
						else
						{
							float z2 = (j - 1) * cellSize2 - 75.0f;
							if (isOk)
								if (wasOk)
									points1.push_back(glm::vec3(x, 0.0f, z));
								else
								{
									points1.push_back(glm::vec3(x, 0.0f, z2));
									points1.push_back(glm::vec3(x, 0.0f, z));
								}
							else
								if (wasOk)
								{
									points1.push_back(glm::vec3(x, 0.0f, z));
									points1.push_back(glm::vec3(x, 0.0f, z2));
								}
								else
									points1.push_back(glm::vec3(x, 0.0f, z2));

							i--;
						}

						wasOk = isOk;
					}
					goingBack = false;
					i = 0;
				}
			}
#pragma endregion
			points1.push_back(glm::vec3(-75.0f, 50.0f, 75.0f));
			points1.push_back(glm::vec3(85.0f, 50.0f, -85.0f));
			points1.push_back(glm::vec3(85.0f, 0.0f, -85.0f));
#pragma region part4
			i = split2 - 1;

			for (j = 0; j < split2; ++j)
			{
				if (!goingBack)
				{
					bool isOk = true;
					while (i >= mid && isOk)
					{
						isOk = true;
						float x = i * cellSize2 - 75.0f;
						float z = j * cellSize2 - 75.0f;
						int ii = (int)round(x / cellSize) + (program->split - 1) / 2;
						int jj = (int)round(z / cellSize) + (program->split - 1) / 2;
						for (int k = ii - r; k < ii + r; ++k)
							for (int l = jj - r; l < jj + r; ++l)
							{
								if (k < 0 || l < 0 || k >= program->split || l >= program->split) continue;
								float curh = v[k * program->split + l];
								if (curh > 0) isOk = false;
							}
						if (isOk)
						{
							points1.push_back(glm::vec3(x, 0.0f, z));
							i--;
						}
						else i++;
					}
					goingBack = true;
					if (i < mid) i = mid;
				}
				else
				{
					int state = 0;//0 - repositioning, 1 - go forward, 2 return
					bool isOk = true;
					bool wasOk = true;
					while (i < split2)
					{
						isOk = true;
						float x = i * cellSize2 - 75.0f;
						float z = j * cellSize2 - 75.0f;
						int ii = (int)round(x / cellSize) + (program->split - 1) / 2;
						int jj = (int)round(z / cellSize) + (program->split - 1) / 2;
						for (int k = ii - r; k < ii + r; ++k)
							for (int l = jj - r; l < jj + r; ++l)
							{
								if (k < 0 || l < 0 || k >= program->split || l >= program->split) continue;
								float curh = v[k * program->split + l];
								if (curh > 0) isOk = false;
							}
						if (state == 0)
						{
							if (isOk)
							{
								state = 1;
								points1.push_back(glm::vec3(x, 0.0f, z));
								i--;
								if (i < mid)
								{
									i += 2;
									state = 2;
								}
							}
							else
							{
								z = (j - 1) * cellSize2 - 75.0f;
								i++;
								x = i * cellSize2 - 75.0f;
								points1.push_back(glm::vec3(x, 0.0f, z));
							}
						}
						else if (state == 1)
						{
							if (isOk)
							{
								points1.push_back(glm::vec3(x, 0.0f, z));
								i--;
								if (i < mid)
								{
									state = 2;
									i += 2;
								}
							}
							else
							{
								state = 2;
								i += 2;
								isOk = true;
							}
						}
						else
						{
							float z2 = (j - 1) * cellSize2 - 75.0f;
							if (isOk)
								if (wasOk)
									points1.push_back(glm::vec3(x, 0.0f, z));
								else
								{
									points1.push_back(glm::vec3(x, 0.0f, z2));
									points1.push_back(glm::vec3(x, 0.0f, z));
								}
							else
								if (wasOk)
								{
									points1.push_back(glm::vec3(x, 0.0f, z));
									points1.push_back(glm::vec3(x, 0.0f, z2));
								}
								else
									points1.push_back(glm::vec3(x, 0.0f, z2));

							i++;
						}

						wasOk = isOk;
					}
					goingBack = false;
					i = split2 - 1;
				}
			}
#pragma endregion
			points1.push_back(glm::vec3(85.0f, 50.0f, 85.0f));

			int ii = 1;
			while (ii < points1.size() - 1)
			{
				if (points1[ii - 1].y == 0.0f && points1[ii].y == 0.0f && points1[ii + 1].y == 0.0f)
					if (
						(points1[ii - 1].x == points1[ii].x && points1[ii + 1].x == points1[ii].x &&
							points1[ii - 1].z >= points1[ii].z && points1[ii].z >= points1[ii + 1].z) ||
						(points1[ii - 1].x == points1[ii].x && points1[ii + 1].x == points1[ii].x &&
							points1[ii - 1].z <= points1[ii].z && points1[ii].z <= points1[ii + 1].z) ||
						(points1[ii - 1].z == points1[ii].z && points1[ii + 1].z == points1[ii].z &&
							points1[ii - 1].x >= points1[ii].x && points1[ii].x >= points1[ii + 1].x) ||
						(points1[ii - 1].z == points1[ii].z && points1[ii + 1].z == points1[ii].z &&
							points1[ii - 1].x <= points1[ii].x && points1[ii].x <= points1[ii + 1].x)
						)
					{
						points1.erase(points1.begin() + ii);
					}
					else
						ii++;
				else
					ii++;
			}
			out_file.open("paths.f12");
			for (int i = 0; i < points1.size(); ++i)
			{
				auto pos = points1[i];
				out_file << std::fixed << std::setprecision(3) << "N" << n << "G01X" << pos.x << "Y" << pos.z << "Z" << pos.y + 15.0f << std::endl;
				//	Figure* f = new Point();
				//	((Point*)f)->special = true;
				//	f->Initialize(program);
				//	f->MoveTo(pos.x, pos.y, pos.z);
				//	program->figures.push_back(f);
			}
			out_file.close();
		}
		if (ImGui::Button("Ccc"))
		{
			Figure* f = new BezierPatchC2(program->patches_n, program->patches_m, program->bezierC0width, program->bezierC0length, program->bezierC0r, false);
			f->Initialize(program);
			program->figures.push_back(f);
			sprintf_s(f->newName, STRMAX, (std::string("Plaski")).c_str());
			sprintf_s(f->name, STRMAX, (std::string("Plaski")).c_str());

			program->cur->MoveTo(7.0f, 0.0f, 0.0f);
			program->newtonD = 0.1f;
			for (int i = 0; i < program->figures.size(); ++i)
			{
				if (strcmp(program->figures[i]->name, "Lewa Tuba") == 0)
					program->figures[i]->Select();
				else if (strcmp(program->figures[i]->name, "Plaski") == 0)
					program->figures[i]->Select();
				else
					program->figures[i]->Unselect();
			}
			auto points1 = Intersect2(program, 1.0f);

			program->cur->MoveTo(-7.0f, 0.0f, 0.0f);
			program->newtonD = 0.1f;
			for (int i = 0; i < program->figures.size(); ++i)
			{
				if (strcmp(program->figures[i]->name, "Prawa Tuba") == 0)
					program->figures[i]->Select();
				else if (strcmp(program->figures[i]->name, "Plaski") == 0)
					program->figures[i]->Select();
				else
					program->figures[i]->Unselect();
			}
			auto points2 = Intersect2(program, 1.0f);

			program->specialCase1 = true;

			program->cur->MoveTo(30.0f, 0.0f, 15.0f);
			program->newtonD = 0.1f;
			for (int i = 0; i < program->figures.size(); ++i)
			{
				if (strcmp(program->figures[i]->name, "Przod") == 0)
					program->figures[i]->Select();
				else if (strcmp(program->figures[i]->name, "Plaski") == 0)
					program->figures[i]->Select();
				else
					program->figures[i]->Unselect();
			}
			auto points3 = Intersect2(program, -1.0f);

			program->cur->MoveTo(30.0f, 0.0f, -30.0f);
			program->newtonD = 0.1f;
			for (int i = 0; i < program->figures.size(); ++i)
			{
				if (strcmp(program->figures[i]->name, "Tyl") == 0)
					program->figures[i]->Select();
				else if (strcmp(program->figures[i]->name, "Plaski") == 0)
					program->figures[i]->Select();
				else
					program->figures[i]->Unselect();
			}
			auto points4 = Intersect2(program, 1.0f);

			program->specialCase1 = false;

			std::vector<glm::vec3> ppoints1;
			std::vector<glm::vec3> ppoints2;
			std::vector<glm::vec3> ppoints3;
			std::vector<glm::vec3> ppoints4;
			float r = 5.0f;
			for (int i = 1; i < points1.size() - 1; ++i)
			{
				glm::vec3 v = points1[i + 1] - points1[i - 1];
				glm::vec3 v2 = glm::normalize(glm::vec3(-v.z, 0.0f, v.x));
				ppoints1.push_back(points1[i] + r * v2);
			}
			for (int i = 1; i < points2.size() - 1; ++i)
			{
				glm::vec3 v = points2[i + 1] - points2[i - 1];
				glm::vec3 v2 = glm::normalize(glm::vec3(-v.z, 0.0f, v.x));
				ppoints2.push_back(points2[i] + r * v2);
			}
			for (int i = 1; i < points3.size() - 1; ++i)
			{
				glm::vec3 v = points3[i + 1] - points3[i - 1];
				glm::vec3 v2 = glm::normalize(glm::vec3(v.z, 0.0f, -v.x));
				ppoints3.push_back(points3[i] + r * v2);
			}
			for (int i = 1; i < points4.size() - 1; ++i)
			{
				glm::vec3 v = points4[i + 1] - points4[i - 1];
				glm::vec3 v2 = glm::normalize(glm::vec3(-v.z, 0.0f, v.x));
				ppoints4.push_back(points4[i] + r * v2);
			}

			std::vector<glm::vec3> points;
			bool putNextPoint = true;
			int i = ppoints1.size() - 1;
			float eps = 0.5f;

			putNextPoint = true;
			while (putNextPoint)
			{
				points.push_back(ppoints1[i]);
				for (int j = 0; j < ppoints3.size(); ++j)
				{
					if (glm::distance(ppoints1[i], ppoints3[j]) < eps)
					{
						i = j;
						putNextPoint = false;
						break;
					}
				}
				if (putNextPoint)
				{
					i--;
				}
			}

			putNextPoint = true;
			while (putNextPoint)
			{
				points.push_back(ppoints3[i]);
				for (int j = 0; j < ppoints2.size(); ++j)
				{
					if (glm::distance(ppoints3[i], ppoints2[j]) < eps)
					{
						i = j;
						putNextPoint = false;
						break;
					}
				}
				if (putNextPoint)
				{
					i++;
				}
			}

			putNextPoint = true;
			while (putNextPoint)
			{
				points.push_back(ppoints2[i]);
				for (int j = 0; j < ppoints4.size(); ++j)
				{
					if (glm::distance(ppoints2[i], ppoints4[j]) < eps)
					{
						i = j;
						putNextPoint = false;
						break;
					}
				}
				if (putNextPoint)
				{
					i--;
					if (i < 0) i = ppoints2.size() - 1;
				}
			}

			putNextPoint = true;
			while (putNextPoint)
			{
				points.push_back(ppoints4[i]);
				for (int j = 0; j < ppoints1.size(); ++j)
				{
					if (glm::distance(ppoints4[i], ppoints1[j]) < eps)
					{
						i = j;
						putNextPoint = false;
						break;
					}
				}
				if (putNextPoint)
				{
					i--;
				}
			}

			while (i >= 0)
			{
				points.push_back(ppoints1[i]);
				i--;
			}


			for (int i = 0; i < points.size(); ++i)
			{
				Figure* f = new Point();
				((Point*)f)->special = true;
				f->Initialize(program);
				f->MoveTo(points[i].x, points[i].y, points[i].z);
				program->figures.push_back(f);
			}

			std::ofstream out_file;
			out_file.open("paths.f10");

			int n = 0;

			out_file << std::fixed << std::setprecision(3) << "N" << n << "G01X" << 0.0f << "Y" << 0.0f << "Z" << 70.0f << std::endl;
			n++;
			out_file << std::fixed << std::setprecision(3) << "N" << n << "G01X" << 85.0f << "Y" << 85.0f << "Z" << 70.0f << std::endl;
			n++;
			out_file << std::fixed << std::setprecision(3) << "N" << n << "G01X" << 85.0f << "Y" << 85.0f << "Z" << 15.0f << std::endl;
			n++;
			for (int i = 0; i < points.size(); ++i)
			{
				auto pos = points[i];
				out_file << std::fixed << std::setprecision(3) << "N" << n << "G01X" << pos.x << "Y" << pos.z << "Z" << 15.0f << std::endl;
			}
			out_file << std::fixed << std::setprecision(3) << "N" << n << "G01X" << 85.0f << "Y" << 85.0f << "Z" << 15.0f << std::endl;
			n++;
			out_file << std::fixed << std::setprecision(3) << "N" << n << "G01X" << 85.0f << "Y" << 85.0f << "Z" << 70.0f << std::endl;
			n++;
			out_file << std::fixed << std::setprecision(3) << "N" << n << "G01X" << 0.0f << "Y" << 0.0f << "Z" << 70.0f << std::endl;
			n++;
			out_file.close();
		}
		ImGui::TreePop();
	}

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
			Figure* f = new BezierPatchC0(program->patches_n, program->patches_m, program->bezierC0width, program->bezierC0length, program->bezierC0r, false);
			f->Initialize(program);
			program->figures.push_back(f);
		}

		if (ImGui::Button("New Bezier Patch C0 Cylinder"))
		{
			Figure* f = new BezierPatchC0(program->patches_n, program->patches_m, program->bezierC0width, program->bezierC0length, program->bezierC0r, true);
			f->Initialize(program);
			program->figures.push_back(f);
		}
		if (ImGui::Button("New Bezier Patch C2 Rectangle"))
		{
			Figure* f = new BezierPatchC2(program->patches_n, program->patches_m, program->bezierC0width, program->bezierC0length, program->bezierC0r, false);
			f->Initialize(program);
			program->figures.push_back(f);
		}
		if (ImGui::Button("New Bezier Patch C2 Cylinder"))
		{
			Figure* f = new BezierPatchC2(program->patches_n, program->patches_m, program->bezierC0width, program->bezierC0length, program->bezierC0r, true);
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

	if (ImGui::Button("Serialize"))
	{
		Serialize(program);
	}
	if (ImGui::Button("Deserialize"))
	{
		Deserialize(program);
	}
	if (ImGui::Button("Collapse"))
	{
		Collapse(program);
	}
	ImGui::Checkbox("Create All Patches", &program->allGregorys);
	if (ImGui::Button("CreateGregory"))
	{
		CreateGregory(program);
	}
	if (ImGui::TreeNode("Intersection Parameters"))
	{
		ImGui::Checkbox("Use Cursor", &program->useCursor);

		ImGui::SliderInt("Start Points Splits", &program->startPointsSplits, 1, 100);
		if (program->startPointsSplits < 1)program->startPointsSplits = 1;

		ImGui::SliderInt("Cursor Points Splits", &program->cursorPointsSplits, 1, 100);
		if (program->cursorPointsSplits < 1)program->cursorPointsSplits = 1;

		ImGui::SliderFloat("Closest Points Start Window Size", &program->closestPointsStartWindowSize, 0.0f, 0.5f);
		if (program->closestPointsStartWindowSize >= 0.5f)program->closestPointsStartWindowSize = 0.5f;
		if (program->closestPointsStartWindowSize <= 0.0f)program->closestPointsStartWindowSize = 0.01f;

		ImGui::SliderFloat("Newton D", &program->newtonD, 0.0f, 10.0f);
		if (program->newtonD <= 0.0f)program->newtonD = 0.01f;

		ImGui::SliderFloat("Newton Points Dist", &program->pointsDistNewton, 0.0f, 10.0f);
		if (program->pointsDistNewton <= 0.0f)program->pointsDistNewton = 0.01f;

		ImGui::SliderFloat("Stop Searching Points Loop D", &program->stopSearchingPointsLoopD, 0.0f, 10.0f);
		if (program->stopSearchingPointsLoopD <= 0.0f)program->stopSearchingPointsLoopD = 0.01f;

		ImGui::SliderFloat("Stop Searching Points Far D", &program->stopSearchingPointsFarD, 0.0f, 10.0f);
		if (program->stopSearchingPointsFarD <= 0.0f)program->stopSearchingPointsFarD = 0.01f;

		ImGui::SliderFloat("Connect Line To Wall Eps", &program->connectLineToWallEps, 0.0f, 10.0f);
		if (program->connectLineToWallEps <= 0.0f)program->connectLineToWallEps = 0.01f;

		if (ImGui::Button("Reset parameters"))
		{
			ResetParameters();
		}

		ImGui::TreePop();
	}
	if (ImGui::Button("Intersect"))
	{
		Intersect(program);
	}
	if (ImGui::Button("Clear"))
	{
		Clear(program);
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
	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glm::mat4 persp = program->cam->GetProjectionMatrix();
	glm::mat4 view = program->cam->GetViewportMatrix();

	program->shader.use();
	unsigned int perspLoc = glGetUniformLocation(program->shader.ID, "persp");
	unsigned int viewLoc = glGetUniformLocation(program->shader.ID, "view");
	glUniformMatrix4fv(perspLoc, 1, GL_FALSE, glm::value_ptr(persp));
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	program->trimShader.use();
	perspLoc = glGetUniformLocation(program->trimShader.ID, "persp");
	viewLoc = glGetUniformLocation(program->trimShader.ID, "view");
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
	program->patchShaderDeBoor.use();
	perspLoc = glGetUniformLocation(program->patchShaderDeBoor.ID, "persp");
	viewLoc = glGetUniformLocation(program->patchShaderDeBoor.ID, "view");
	glUniformMatrix4fv(perspLoc, 1, GL_FALSE, glm::value_ptr(persp));
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	program->patchGregoryShader.use();
	perspLoc = glGetUniformLocation(program->patchGregoryShader.ID, "persp");
	viewLoc = glGetUniformLocation(program->patchGregoryShader.ID, "view");
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

	window = glfwCreateWindow(DEFAULT_WIDTH, DEFAULT_HEIGHT, "LearnOpenGL", NULL, NULL);
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

	program->trimShader = Shader("shaders/trimVertexShader.vs"
		, "shaders/fragShader.fs"
		, nullptr);

	program->bezierShader = Shader("shaders/bezierVertexShader.vs"
		, "shaders/fragShader.fs"
		, "shaders/bezierGeometryShader.gs");

	program->patchShader = Shader("shaders/patchVertexShader.vs"
		, "shaders/fragShader.fs"
		, "shaders/patchGeometryShader.gs"
	);

	program->patchShaderDeBoor = Shader("shaders/patchVertexShader.vs"
		, "shaders/fragShader.fs"
		, "shaders/patchDeBoorGeometryShader.gs"
	);

	program->patchGregoryShader = Shader("shaders/gregoryVertexShader.vs"
		, "shaders/fragShader.fs"
		, nullptr
		, "shaders/gregoryHullShader.hs"
		, "shaders/gregoryDomainShader.ds"
	);

	program->mp = new MiddlePoint();
	program->mp->Initialize(program);
	program->cur = new Cursor();
	program->cur->Initialize(program);

	//Figure* f = new GregoryPatch();
	//f->Initialize(program);
	//program->figures.push_back(f);

	program->cam = new Camera();
	program->cam->LookAt({ 0,0,100 }, { 0,0,-1 }, { 0,1,0 });
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
	glLineWidth(0.2f);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

	Shader screenShader("shaders/textureVertexShader.vs", "shaders/textureFragmentShader.fs");
	screenShader.use();
	screenShader.setInt("screenTexture", 0);
#pragma endregion

	unsigned int framebufferRed, textureColorbufferRed;
	CreateColorbuffer(framebufferRed, textureColorbufferRed);

	unsigned int framebufferBlue, textureColorbufferBlue;
	CreateColorbuffer(framebufferBlue, textureColorbufferBlue);

	FillImage(program->testTex, program);

	while (!glfwWindowShouldClose(window))
	{
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		//ImGui::ShowDemoWindow();
		proc.processInput(window);
		RenderGui();

		float aspect = (float)program->current_width / (float)program->current_height;
		program->cam->SetPerspective(aspect);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		DrawScene();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glDeleteVertexArrays(1, &quadVAO);
	glDeleteBuffers(1, &quadVBO);

	glfwTerminate();
	Clear(program);
	for (int i = 0; i < program->figures.size(); ++i) delete program->figures[i];
	delete program->mp;
	delete program;
	return 0;
}



