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
#include "Duck.h"
#include "Tool.h"
#include "ImGuiFileDialog.h"
#include "Obstacle.h"
#include <queue>

bool firstCall = true;
bool rotate = false;
glm::vec2 mousePosOld;
glm::vec3 lookAt;
std::shared_ptr<Program> program = {};
std::vector<glm::ivec3> path = {};
double lastTime;

int obstaclesBitmap[BITMAP_SIZE][BITMAP_SIZE];
static GLubyte checkImage[BITMAP_SIZE][BITMAP_SIZE][4];
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void window_size_callback(GLFWwindow* window, int width, int height);

bool linesIntersect(double x1, double y1, double x2, double y2, double x3, double y3, double x4, double y4) {
	// Return false if either of the lines have zero length
	if (x1 == x2 && y1 == y2 ||
		x3 == x4 && y3 == y4) {
		return false;
	}
	// Fastest method, based on Franklin Antonio's "Faster Line Segment Intersection" topic "in Graphics Gems III" book (http://www.graphicsgems.org/)
	double ax = x2 - x1;
	double ay = y2 - y1;
	double bx = x3 - x4;
	double by = y3 - y4;
	double cx = x1 - x3;
	double cy = y1 - y3;

	double alphaNumerator = by * cx - bx * cy;
	double commonDenominator = ay * bx - ax * by;
	if (commonDenominator > 0) {
		if (alphaNumerator < 0 || alphaNumerator > commonDenominator) {
			return false;
		}
	}
	else if (commonDenominator < 0) {
		if (alphaNumerator > 0 || alphaNumerator < commonDenominator) {
			return false;
		}
	}
	double betaNumerator = ax * cy - ay * cx;
	if (commonDenominator > 0) {
		if (betaNumerator < 0 || betaNumerator > commonDenominator) {
			return false;
		}
	}
	else if (commonDenominator < 0) {
		if (betaNumerator > 0 || betaNumerator < commonDenominator) {
			return false;
		}
	}
	if (commonDenominator == 0) {
		// This code wasn't in Franklin Antonio's method. It was added by Keith Woodward.
		// The lines are parallel.
		// Check if they're collinear.
		double y3LessY1 = y3 - y1;
		double collinearityTestForP3 = x1 * (y2 - y3) + x2 * (y3LessY1)+x3 * (y1 - y2);	// see http://mathworld.wolfram.com/Collinear.html
		// If p3 is collinear with p1 and p2 then p4 will also be collinear, since p1-p2 is parallel with p3-p4
		if (collinearityTestForP3 == 0) {
			// The lines are collinear. Now check if they overlap.
			if (x1 >= x3 && x1 <= x4 || x1 <= x3 && x1 >= x4 ||
				x2 >= x3 && x2 <= x4 || x2 <= x3 && x2 >= x4 ||
				x3 >= x1 && x3 <= x2 || x3 <= x1 && x3 >= x2) {
				if (y1 >= y3 && y1 <= y4 || y1 <= y3 && y1 >= y4 ||
					y2 >= y3 && y2 <= y4 || y2 <= y3 && y2 >= y4 ||
					y3 >= y1 && y3 <= y2 || y3 <= y1 && y3 >= y2) {
					return true;
				}
			}
		}
		return false;
	}
	return true;
}
bool LinesIntersect(glm::vec2 p1, glm::vec2 p2, glm::vec2 p3, glm::vec2 p4) { return linesIntersect(p1.x, p1.y, p2.x, p2.y, p3.x, p3.y, p4.x, p4.y); }

void Simulate()
{
	auto time = glfwGetTime();
	program->t = (time - lastTime) / (program->simTime);
	if (program->t >= 1.0f)
	{
		program->t = 1.0f;
		program->simulating = false;
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

void SetImage(unsigned int& texName, std::shared_ptr<Program> program, glm::ivec2 from = { -1,-1 }, glm::ivec2 to = { -1,-1 })
{
	glBindTexture(GL_TEXTURE_2D, texName);
	int inf = BITMAP_SIZE * BITMAP_SIZE;
	int max = -1;
	int i, j, c;

	for (i = 0; i < BITMAP_SIZE; i++)
		for (j = 0; j < BITMAP_SIZE; j++)
			if (obstaclesBitmap[i][j] > max && obstaclesBitmap[i][j] != inf) max = obstaclesBitmap[i][j];

	for (i = 0; i < BITMAP_SIZE; i++) {
		for (j = 0; j < BITMAP_SIZE; j++) {
			int w = 255;
			if (texName == program->distancesTex)
			{
				int dist = obstaclesBitmap[j][i];
				if (dist == -1)
				{
					checkImage[i][j][0] = (GLubyte)255;
					checkImage[i][j][1] = (GLubyte)0;
					checkImage[i][j][2] = (GLubyte)0;
					checkImage[i][j][3] = (GLubyte)255;
				}
				else if (dist == inf)
				{
					checkImage[i][j][0] = (GLubyte)0;
					checkImage[i][j][1] = (GLubyte)0;
					checkImage[i][j][2] = (GLubyte)255;
					checkImage[i][j][3] = (GLubyte)255;
				}
				else
				{
					int color;
					if (max == 0)color = 0;
					else color = (int)round(255.0f * ((float)dist / max));

					checkImage[i][j][0] = (GLubyte)color;
					checkImage[i][j][1] = (GLubyte)color;
					checkImage[i][j][2] = (GLubyte)color;
					checkImage[i][j][3] = (GLubyte)255;
				}
			}
			else
			{
				int dist = obstaclesBitmap[j][i];
				if (dist == -1)
				{
					checkImage[i][j][0] = (GLubyte)255;
					checkImage[i][j][1] = (GLubyte)0;
					checkImage[i][j][2] = (GLubyte)0;
					checkImage[i][j][3] = (GLubyte)255;
				}
				else
				{
					checkImage[i][j][0] = (GLubyte)255;
					checkImage[i][j][1] = (GLubyte)255;
					checkImage[i][j][2] = (GLubyte)255;
					checkImage[i][j][3] = (GLubyte)255;
				}

			}
		}
	}
	if (texName == program->tex)
	{
		for (int k = 0; k < path.size(); ++k)
		{
			int maxPath = path[0].z;
			int color;
			auto elem = path[k];
			if (maxPath > 0) color = (int)round(255.0f * ((float)elem.z / maxPath));
			else color = 0;

			checkImage[elem.y][elem.x][0] = (GLubyte)color;
			checkImage[elem.y][elem.x][1] = (GLubyte)color;
			checkImage[elem.y][elem.x][2] = (GLubyte)color;
			checkImage[elem.y][elem.x][3] = (GLubyte)255;
		}
	}
	if (from.x != -1)
	{
		checkImage[from.y][from.x][0] = (GLubyte)0;
		checkImage[from.y][from.x][1] = (GLubyte)255;
		checkImage[from.y][from.x][2] = (GLubyte)0;
		checkImage[from.y][from.x][3] = (GLubyte)255;

		checkImage[to.y][to.x][0] = (GLubyte)255;
		checkImage[to.y][to.x][1] = (GLubyte)0;
		checkImage[to.y][to.x][2] = (GLubyte)255;
		checkImage[to.y][to.x][3] = (GLubyte)255;
	}

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, BITMAP_SIZE,
		BITMAP_SIZE, 0, GL_RGBA, GL_UNSIGNED_BYTE,
		checkImage);
}

void FillImage(unsigned int& texName, std::shared_ptr<Program> program)
{
	int inf = BITMAP_SIZE * BITMAP_SIZE;
	int max = -1;
	int i, j, c;

	for (i = 0; i < BITMAP_SIZE; i++)
		for (j = 0; j < BITMAP_SIZE; j++)
			if (obstaclesBitmap[i][j] > max && obstaclesBitmap[i][j] != inf) max = obstaclesBitmap[i][j];

	for (i = 0; i < BITMAP_SIZE; i++) {
		for (j = 0; j < BITMAP_SIZE; j++) {
			int w = 255;
			checkImage[i][j][0] = (GLubyte)255;
			checkImage[i][j][1] = (GLubyte)255;
			checkImage[i][j][2] = (GLubyte)255;
			checkImage[i][j][3] = (GLubyte)255;
		}
	}

	glGenTextures(1, &texName);
	glBindTexture(GL_TEXTURE_2D, texName);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
		GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
		GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, BITMAP_SIZE,
		BITMAP_SIZE, 0, GL_RGBA, GL_UNSIGNED_BYTE,
		checkImage);
}

void RecalcObstaclesBitmap()
{
	int inf = BITMAP_SIZE * BITMAP_SIZE;
	for (int i = 0; i < BITMAP_SIZE; ++i)
		for (int j = 0; j < BITMAP_SIZE; ++j)
		{
			float a = (float)i / BITMAP_SIZE * 360.0f;
			float b = (float)j / BITMAP_SIZE * 360.0f;
			glm::vec2 tool_p0 = program->currentWindow->tool->GetPoint(1, a, b);
			glm::vec2 tool_p1 = program->currentWindow->tool->GetPoint(2, a, b);
			glm::vec2 tool_p2 = program->currentWindow->tool->GetPoint(3, a, b);

			bool wrong = false;
			for (int k = 0; k < program->currentWindow->obstacles.size() && !wrong; ++k)
			{
				auto& obstacle = program->currentWindow->obstacles[k];
				glm::vec2 p0 = obstacle->GetPoint(1);
				glm::vec2 p1 = obstacle->GetPoint(2);
				glm::vec2 p2 = obstacle->GetPoint(3);
				glm::vec2 p3 = obstacle->GetPoint(4);

				if (LinesIntersect(tool_p0, tool_p1, p0, p1)) wrong = true;
				if (LinesIntersect(tool_p0, tool_p1, p1, p2)) wrong = true;
				if (LinesIntersect(tool_p0, tool_p1, p2, p3)) wrong = true;
				if (LinesIntersect(tool_p0, tool_p1, p3, p0)) wrong = true;

				if (LinesIntersect(tool_p1, tool_p2, p0, p1)) wrong = true;
				if (LinesIntersect(tool_p1, tool_p2, p1, p2)) wrong = true;
				if (LinesIntersect(tool_p1, tool_p2, p2, p3)) wrong = true;
				if (LinesIntersect(tool_p1, tool_p2, p3, p0)) wrong = true;

			}
			obstaclesBitmap[i][j] = wrong ? -1 : inf;
		}
	path.clear();
	SetImage(program->tex, program);
	SetImage(program->distancesTex, program);
}

void MakeFloodFill(glm::ivec2 from, glm::ivec2 to)
{
	RecalcObstaclesBitmap();
	int inf = BITMAP_SIZE * BITMAP_SIZE;
	path.clear();
	std::queue<glm::ivec2> stack;
	if (obstaclesBitmap[to.x][to.y] == inf) {
		stack.push(to);
		obstaclesBitmap[to.x][to.y] = 0;
	}
	while (stack.size() > 0)
	{
		auto top = stack.front(); stack.pop();

		auto dist = obstaclesBitmap[top.x][top.y];

		auto l = glm::ivec2(top.x - 1, top.y);
		if (l.x < 0) l.x = BITMAP_SIZE - 1;

		auto r = glm::ivec2(top.x + 1, top.y);
		if (r.x == BITMAP_SIZE) r.x = 0;

		auto u = glm::ivec2(top.x, top.y + 1);
		if (u.y == BITMAP_SIZE) u.y = 0;

		auto d = glm::ivec2(top.x, top.y - 1);
		if (d.y < 0) d.y = BITMAP_SIZE - 1;

		{
			auto cur = l;
			if (obstaclesBitmap[cur.x][cur.y] > dist + 1)
			{
				obstaclesBitmap[cur.x][cur.y] = dist + 1;
				stack.push(cur);
			}
		}

		{
			auto cur = r;
			if (obstaclesBitmap[cur.x][cur.y] > dist + 1)
			{
				obstaclesBitmap[cur.x][cur.y] = dist + 1;
				stack.push(cur);
			}
		}

		{
			auto cur = u;
			if (obstaclesBitmap[cur.x][cur.y] > dist + 1)
			{
				obstaclesBitmap[cur.x][cur.y] = dist + 1;
				stack.push(cur);
			}
		}

		{
			auto cur = d;
			if (obstaclesBitmap[cur.x][cur.y] > dist + 1)
			{
				obstaclesBitmap[cur.x][cur.y] = dist + 1;
				stack.push(cur);
			}
		}

	}

	if (obstaclesBitmap[from.x][from.y] == -1) program->error = "Start position is in invalid configuration";
	if (obstaclesBitmap[to.x][to.y] == -1) program->error = "End position is in invalid configuration";
	if (obstaclesBitmap[from.x][from.y] == inf) program->error = "End is unreachable from Start";
	if (obstaclesBitmap[from.x][from.y] >= 0 && obstaclesBitmap[from.x][from.y] != inf)
	{
		auto cur = from;
		while (obstaclesBitmap[cur.x][cur.y] > 0)
		{
			auto dist = obstaclesBitmap[cur.x][cur.y];
			path.push_back(glm::ivec3(cur, dist));


			auto l = glm::ivec2(cur.x - 1, cur.y);
			if (l.x < 0) l.x = BITMAP_SIZE - 1;

			auto r = glm::ivec2(cur.x + 1, cur.y);
			if (r.x == BITMAP_SIZE) r.x = 0;

			auto u = glm::ivec2(cur.x, cur.y + 1);
			if (u.y == BITMAP_SIZE) u.y = 0;

			auto d = glm::ivec2(cur.x, cur.y - 1);
			if (d.y < 0) d.y = BITMAP_SIZE - 1;


			if (obstaclesBitmap[l.x][l.y] == dist - 1) cur = l;
			else if (obstaclesBitmap[r.x][r.y] == dist - 1) cur = r;
			else if (obstaclesBitmap[d.x][d.y] == dist - 1) cur = d;
			else if (obstaclesBitmap[u.x][u.y] == dist - 1) cur = u;
		}
		path.push_back(glm::ivec3(cur, 0));
	}
	SetImage(program->tex, program, from, to);
	SetImage(program->distancesTex, program, from, to);
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

		if (program->selection)
		{
			if (ImGui::Button("Show 1"))
			{
				program->currentWindow->tool->alpha = program->opt1.x * 180.0f / M_PI;
				program->currentWindow->tool->beta = program->opt1.y * 180.0f / M_PI;
			}
			if (ImGui::Button("Show 2"))
			{
				program->currentWindow->tool->alpha = program->opt2.x * 180.0f / M_PI;
				program->currentWindow->tool->beta = program->opt2.y * 180.0f / M_PI;
			}
			if (ImGui::Button("Select 1"))
			{
				program->currentWindow->tool->alpha = program->opt1.x * 180.0f / M_PI;
				program->currentWindow->tool->beta = program->opt1.y * 180.0f / M_PI;
				program->selection = false;
			}
			if (ImGui::Button("Select 2"))
			{
				program->currentWindow->tool->alpha = program->opt2.x * 180.0f / M_PI;
				program->currentWindow->tool->beta = program->opt2.y * 180.0f / M_PI;
				program->selection = false;
			}
		}
		else
		{
			ImGui::Checkbox("Edition", &program->edition);

			if (program->edition) {
				if (ImGui::TreeNode("Tool"))
				{
					ImGui::SliderFloat("L1", &program->currentWindow->tool->l1, 0.01f, 1.0f);
					ImGui::SliderFloat("L2", &program->currentWindow->tool->l2, 0.01f, 1.0f);


					if (ImGui::Button("Set angles as start pos"))
						program->startPos = glm::vec2(program->currentWindow->tool->alpha, program->currentWindow->tool->beta);
					if (ImGui::Button("Set angles as end pos"))
						program->endPos = glm::vec2(program->currentWindow->tool->alpha, program->currentWindow->tool->beta);

					if (ImGui::Button("Show start pos"))
					{
						program->currentWindow->tool->alpha = program->startPos.x;
						program->currentWindow->tool->beta = program->startPos.y;
					}
					if (ImGui::Button("Show end pos"))
					{
						program->currentWindow->tool->alpha = program->endPos.x;
						program->currentWindow->tool->beta = program->endPos.y;
					}
					ImGui::TreePop();
				}

				if (ImGui::Button("Find path"))
				{
					int xFrom = (int)round(BITMAP_SIZE * (program->startPos.x / 360.0f));
					int yFrom = (int)round(BITMAP_SIZE * (program->startPos.y / 360.0f));
					int xTo = (int)round(BITMAP_SIZE * (program->endPos.x / 360.0f));
					int yTo = (int)round(BITMAP_SIZE * (program->endPos.y / 360.0f));

					MakeFloodFill(glm::ivec2(xFrom, yFrom), glm::ivec2(xTo, yTo));
				}

				if (ImGui::TreeNode("Obstacles"))
				{
					if (ImGui::Button("Recalc obstacles map"))
					{
						RecalcObstaclesBitmap();
					}

					ImGui::Checkbox("Show obstacles map with path", &program->showMap);
					if (program->showMap) {
						ImGui::Begin("Obstacles map with path##uu", &program->showMap);
						ImGuiIO& io = ImGui::GetIO();
						ImTextureID my_tex_id = (void*)program->tex;
						float my_tex_w = BITMAP_SIZE;
						float my_tex_h = BITMAP_SIZE;
						{
							ImVec2 pos = ImGui::GetCursorScreenPos();
							ImVec2 uv_min = ImVec2(0.0f, 0.0f);
							ImVec2 uv_max = ImVec2(1.0f, 1.0f);
							ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
							ImVec4 border_col = ImVec4(1.0f, 1.0f, 1.0f, 0.5f);
							ImGui::Image(my_tex_id, ImVec2(my_tex_w, my_tex_h), uv_min, uv_max, tint_col, border_col);
							if (ImGui::IsItemHovered())
							{
								ImGui::BeginTooltip();
								float region_sz = 32.0f;
								float region_x = io.MousePos.x - pos.x - region_sz * 0.5f;
								float region_y = io.MousePos.y - pos.y - region_sz * 0.5f;
								float zoom = 4.0f;
								if (region_x < 0.0f) { region_x = 0.0f; }
								else if (region_x > my_tex_w - region_sz) { region_x = my_tex_w - region_sz; }
								if (region_y < 0.0f) { region_y = 0.0f; }
								else if (region_y > my_tex_h - region_sz) { region_y = my_tex_h - region_sz; }
								ImGui::Text("Min: (%.2f, %.2f)", region_x, region_y);
								ImGui::Text("Max: (%.2f, %.2f)", region_x + region_sz, region_y + region_sz);
								ImVec2 uv0 = ImVec2((region_x) / my_tex_w, (region_y) / my_tex_h);
								ImVec2 uv1 = ImVec2((region_x + region_sz) / my_tex_w, (region_y + region_sz) / my_tex_h);
								ImGui::Image(my_tex_id, ImVec2(region_sz * zoom, region_sz * zoom), uv0, uv1, tint_col, border_col);
								ImGui::EndTooltip();
							}
						}
						ImGui::End();
					}

					ImGui::Checkbox("Show obstacles map with distances", &program->showMap2);
					if (program->showMap2) {
						ImGui::Begin("Obstacles map with distances##uu", &program->showMap2);
						ImGuiIO& io = ImGui::GetIO();
						ImTextureID my_tex_id = (void*)program->distancesTex;
						float my_tex_w = BITMAP_SIZE;
						float my_tex_h = BITMAP_SIZE;
						{
							ImVec2 pos = ImGui::GetCursorScreenPos();
							ImVec2 uv_min = ImVec2(0.0f, 0.0f);
							ImVec2 uv_max = ImVec2(1.0f, 1.0f);
							ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
							ImVec4 border_col = ImVec4(1.0f, 1.0f, 1.0f, 0.5f);
							ImGui::Image(my_tex_id, ImVec2(my_tex_w, my_tex_h), uv_min, uv_max, tint_col, border_col);
							if (ImGui::IsItemHovered())
							{
								ImGui::BeginTooltip();
								float region_sz = 32.0f;
								float region_x = io.MousePos.x - pos.x - region_sz * 0.5f;
								float region_y = io.MousePos.y - pos.y - region_sz * 0.5f;
								float zoom = 4.0f;
								if (region_x < 0.0f) { region_x = 0.0f; }
								else if (region_x > my_tex_w - region_sz) { region_x = my_tex_w - region_sz; }
								if (region_y < 0.0f) { region_y = 0.0f; }
								else if (region_y > my_tex_h - region_sz) { region_y = my_tex_h - region_sz; }
								ImGui::Text("Min: (%.2f, %.2f)", region_x, region_y);
								ImGui::Text("Max: (%.2f, %.2f)", region_x + region_sz, region_y + region_sz);
								ImVec2 uv0 = ImVec2((region_x) / my_tex_w, (region_y) / my_tex_h);
								ImVec2 uv1 = ImVec2((region_x + region_sz) / my_tex_w, (region_y + region_sz) / my_tex_h);
								ImGui::Image(my_tex_id, ImVec2(region_sz * zoom, region_sz * zoom), uv0, uv1, tint_col, border_col);
								ImGui::EndTooltip();
							}
						}
						ImGui::End();
					}
					if (ImGui::Button("Add obstacle"))
					{
						auto obstacle = std::make_shared<Obstacle>();
						obstacle->Initialize(program);
						program->currentWindow->figures.push_back(obstacle);
						program->currentWindow->obstacles.push_back(obstacle);
					}

					if (ImGui::TreeNode("Obstacles list"))
					{
						for (int i = 0; i < program->currentWindow->obstacles.size(); ++i)
						{
							auto& obstacle = program->currentWindow->obstacles[i];
							if (ImGui::TreeNode((std::string("Obstacle - ") + std::to_string(i)).c_str()))
							{

								ImGui::SliderFloat("xPos", &obstacle->x, -1.0f, 1.0f);
								ImGui::SliderFloat("yPos", &obstacle->y, -1.0f, 1.0f);
								ImGui::SliderFloat("width", &obstacle->width, 0.01f, 1.0f);
								ImGui::SliderFloat("height", &obstacle->height, 0.01f, 1.0f);
								if (ImGui::Button("Remove"))
								{
									i--;
									auto figure = std::find(program->currentWindow->figures.begin(), program->currentWindow->figures.end(), obstacle);
									program->currentWindow->figures.erase(figure);
									auto obs = std::find(program->currentWindow->obstacles.begin(), program->currentWindow->obstacles.end(), obstacle);
									program->currentWindow->obstacles.erase(obs);
								}
								ImGui::TreePop();
							}
						}
						ImGui::TreePop();
					}
					ImGui::TreePop();
				}
			}
			if (program->currentWindow->tool->l1 < 0.01f)program->currentWindow->tool->l1 = 0.01f;
			if (program->currentWindow->tool->l1 > 1.0f)program->currentWindow->tool->l1 = 1.0f;
			if (program->currentWindow->tool->l2 < 0.01f)program->currentWindow->tool->l2 = 0.01f;
			if (program->currentWindow->tool->l2 > 1.0f)program->currentWindow->tool->l2 = 1.0f;

			if (ImGui::Button("Simulate"))
			{
				program->simulating = true;
				program->t = 0;
				lastTime = glfwGetTime();
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
	program->cam->LookAt({ 0,0,10 }, { 0,0,-1 }, { 0,1,0 });

	program->currentWindow->tool = std::make_shared<Tool>();
	program->currentWindow->tool->Initialize(program);
	program->currentWindow->figures.push_back(program->currentWindow->tool);
	program->startPos = glm::vec2(program->currentWindow->tool->alpha, program->currentWindow->tool->beta);
	program->endPos = glm::vec2(program->currentWindow->tool->alpha, program->currentWindow->tool->beta);
	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	// Setup Platform/Renderer bindings
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 130");
	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	FillImage(program->tex, program);
	FillImage(program->distancesTex, program);
	RecalcObstaclesBitmap();
	while (!glfwWindowShouldClose(window))
	{
		if (program->simulating)
		{
			Simulate();
		}
		float aspect = (float)program->current_width / (float)program->current_height;
		program->cam->SetPerspective(aspect);

		glfwMakeContextCurrent(window);
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		//ImGui::ShowDemoWindow();
		proc.processInput(window);
		RenderGui();
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		DrawScene();
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);


		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}



void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);
	int lCtrlState = glfwGetKey(window, GLFW_KEY_LEFT_CONTROL);
	int lAltState = glfwGetKey(window, GLFW_KEY_LEFT_ALT);
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && lCtrlState == GLFW_PRESS)
	{
		if (program->edition && !program->selection)
		{
			glm::vec2 pos(
				((float)xpos / (float)program->current_width - 0.5f) * 2.0f,
				-((float)ypos / (float)program->current_height - 0.5f) * 2.0f
			);

			program->currentWindow->tool->InverseKinematics(pos);
		}
	}

}

void window_size_callback(GLFWwindow* window, int width, int height) {
	program->current_width = width;
	program->current_height = height;
}