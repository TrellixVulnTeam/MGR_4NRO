#pragma once
#include <vector>
#include "Shader.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>
#define DEFAULT_WIDTH 1280
#define DEFAULT_HEIGHT 720
class Figure;
class Cube;
class Tool;
class Camera;
class Obstacle;

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
	unsigned int colorTexture = 0;
	std::shared_ptr<Tool> tool = {};
	std::vector<std::shared_ptr<Obstacle>> obstacles = {};
};

class Program
{
public:
	Program();
	~Program() = default;

	int current_width = 0;
	int current_height = 0;
	float width = 5.0f;
	float length = 3.0f;
	float height = 1.5f;
	int xSplit = 120;
	int ySplit = 120;

	bool edition = true;
	bool selection = false;
	bool showMap = false;
	bool showMap2 = false;
	int options = 0;

	unsigned int tex = 0;
	unsigned int distancesTex = 0;
	std::string error = "";

	glm::vec2 opt1 = {}, opt2 = {};

	glm::vec2 startPos = {}, endPos = {};

	std::shared_ptr<Camera> cam = {};
	std::shared_ptr<Window> currentWindow = {};

	bool simulating = false;
	float t = 0;
	float simTime = 1.0f;
};

