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
class Parameters;
class Puma;
class Camera;
class Obstacle;
class Cursor;

class Window
{
public:
	Window();
	Window(int id_);
	~Window() = default;
	int id = -1;
	std::shared_ptr<Shader> shader = {};
	std::shared_ptr<Shader> shader2D = {};
	std::shared_ptr<Shader> lightShader = {};
	GLFWwindow* window = {};
	std::vector<std::shared_ptr<Figure>> figures = {};
	bool focused = false;
	unsigned int colorTexture = 0;
	std::shared_ptr<Puma> puma = {};
	std::shared_ptr<Camera> cam = {};
	std::shared_ptr<Cursor> cursor = {};
	int current_width = 0;
	int current_height = 0;
};

class Program
{
public:
	Program();
	~Program() = default;

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

	std::shared_ptr<Window> currentWindow = {};
	std::shared_ptr<Window> wind1 = {};
	std::shared_ptr<Window> wind2 = {};

	bool simulating = false;
	float t = 0;
	float simTime = 1.0f;

	glm::vec3 p1 = {};
	glm::vec3 p2 = {};

	glm::quat q1 = {};
	glm::quat q2 = {};

	glm::vec3 startPos = {};
	glm::vec3 endPos = {};

	glm::quat startQuat = {};
	glm::quat endQuat = {};

	std::shared_ptr<Parameters> startParams = {};
	std::shared_ptr<Parameters> endParams = {};
	std::shared_ptr<Parameters> diffParams = {};

	bool normalize_live = true;
};

