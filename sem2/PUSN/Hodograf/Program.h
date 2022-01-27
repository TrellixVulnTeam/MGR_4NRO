#pragma once
#include <vector>
#include "Shader.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>
#include <implot.h>
#define DEFAULT_WIDTH 1280
#define DEFAULT_HEIGHT 720
class Figure;
class Cube;
class Tool;
class Camera;
class Obstacle;

class ScrollingBuffer {
public:
	int MaxSize;
	int Offset;
	ImVector<ImVec2> Data;
	ScrollingBuffer(int max_size = 2000);
	void AddPoint(float x, float y);
	void Erase();
};

class Window
{
public:
	Window();
	~Window() = default;
	std::shared_ptr<Shader> shader = {};
	std::shared_ptr<Shader> shader2D = {};
	std::shared_ptr<Shader> lightShader = {};
	GLFWwindow* window = {};
	std::vector<std::shared_ptr<Figure>> figures = {};
	bool focused = false;
	std::shared_ptr<Tool> tool = {};
};

class Program
{
public:
	Program();
	~Program() = default;
	void AddValues(float t, float y, float dy, float ddy);

	int current_width = 0;
	int current_height = 0;

	unsigned int tex = 0;
	std::string error = "";

	std::shared_ptr<Camera> cam = {};
	std::shared_ptr<Window> currentWindow = {};

	bool simulating = true;
	float t = 0;
	float simTime = 1.0f;

	ScrollingBuffer _y = {};
	ScrollingBuffer _dy = {};
	ScrollingBuffer _ddy = {};
	ScrollingBuffer _state = {};
	float omega = 0.1f;
};

