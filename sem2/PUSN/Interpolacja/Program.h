#pragma once
#include <vector>
#include "Shader.h"
#include <GLFW/glfw3.h>
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
	std::shared_ptr<Shader> bezierShader = {};
	GLFWwindow* window;
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
	
	float startX = 0.0f;
	float startY = 0.0f;
	float startZ = 0.0f;

	float startAngleZ1 = 0.0f;
	float startAngleX = 0.0f;
	float startAngleZ2 = 0.0f;

	float startQuatX = 0.0f;
	float startQuatY = 0.0f;
	float startQuatZ = 0.0f;
	float startQuatW = 0.0f;

	float endX = 0.0f;
	float endY = 0.0f;
	float endZ = 0.0f;

	float endAngleZ1 = 0.0f;
	float endAngleX = 0.0f;
	float endAngleZ2 = 0.0f;

	float endQuatX = 0.0f;
	float endQuatY = 0.0f;
	float endQuatZ = 0.0f;
	float endQuatW = 0.0f;

	float diffAngleZ1 = 0.0f;
	float diffAngleX = 0.0f;
	float diffAngleZ2 = 0.0f;

	float diffQuatX = 0.0f;
	float diffQuatY = 0.0f;
	float diffQuatZ = 0.0f;
	float diffQuatW = 0.0f;

	bool simulating = false;
	bool setQuats = false;
	bool spherical = false;
	std::shared_ptr<Camera> cam = {};
	std::shared_ptr<Window> currentWindow = {};
	std::shared_ptr<Window> wind1 = {};
	std::shared_ptr<Window> wind2 = {};
	std::shared_ptr<Cube> cube = {};

	float t = 0;
	float simSpeed = 0;
};

