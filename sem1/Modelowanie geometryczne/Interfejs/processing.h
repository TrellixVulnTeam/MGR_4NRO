#pragma once
#define _USE_MATH_DEFINES
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <math.h>
#include <cmath>

class Processing
{
public:
	Processing();
	~Processing();

	static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
	void processInput(GLFWwindow* window);

	float* vertices;
	int vertices_s;
    unsigned int* indices;
    int indices_s;

private:
	void CreateTorus();
};