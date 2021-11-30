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
class Camera;

class Window
{
public:
	Window();
	~Window() = default;
	std::shared_ptr<Shader> shader = {};
	std::shared_ptr<Shader> lightShader = {};
	GLFWwindow* window = {};
	std::vector<std::shared_ptr<Figure>> figures = {};
	bool focused = false;
};

class Program
{
public:
	Program();
	~Program() = default;

	int current_width = 0;
	int current_height = 0;
	unsigned int colorTexture = 0;
	float width = 5.0f;
	float length = 3.0f;
	float height = 1.5f;
	int xSplit = 120;
	int ySplit = 120;

	glm::vec3 startPos = {};
	glm::vec3 endPos = {};

	glm::vec3 startAngle = {};
	glm::vec3 endAngle = {};
	glm::vec3 diffAngle = {};

	glm::quat startQuat = {};
	glm::quat endQuat = {};



	int frames = 5;

	bool simulating = false;
	bool setQuats = false;
	bool spherical = false;
	std::shared_ptr<Camera> cam = {};
	std::shared_ptr<Window> currentWindow = {};
	std::shared_ptr<Window> wind1 = {};
	std::shared_ptr<Window> wind2 = {};

	float t = 0;
	float simSpeed = 0;
};

