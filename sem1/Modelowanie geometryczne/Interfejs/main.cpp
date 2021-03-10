#pragma once
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "processing.h"
#include "Shader.h"
int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
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

	glViewport(0, 0, 800, 600);
	glfwSetFramebufferSizeCallback(window, proc.framebuffer_size_callback);
	Shader ourShader("shaders/vertexShader.vs", "shaders/fragShader.fs");

	unsigned int VBO;
	glGenBuffers(1, &VBO);
	unsigned int VAO;
	glGenVertexArrays(1, &VAO);
	unsigned int EBO;
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, proc.vertices_s * sizeof(float), proc.vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, proc.indices_s * sizeof(unsigned int), proc.indices, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	float radians = 1.0f;

	glm::mat4 projection = glm::mat4(0.0f);

	glm::mat4 transf = glm::mat4(1.0f);
	glm::mat4 view = glm::mat4(1.0f);
	view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
	glm::mat4 persp = glm::mat4(1.0f);
	float _near = 1;
	float _far = 5;
	float fov = 1.0f;
	float e = 1 / tan(fov / 2);
	float aspect = (float)800 / (float)600;
	persp[0][0] = e / aspect;
	persp[1][1] = e;
	persp[2][2] = (_far + _near) / (_far - _near);
	persp[2][3] = -((_far * _near * 2) / (_far - _near));
	persp[3][3] = 0;
	persp[3][2] = 1;
	while (!glfwWindowShouldClose(window))
	{
		proc.processInput(window);

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glm::mat4 trans = glm::mat4(1.0f);

		radians += 0.010f;
		trans = glm::rotate(trans, glm::radians(radians), glm::vec3(0.0, 1.0, 0.0));
		transf = persp * view * trans;
		unsigned int transformLoc = glGetUniformLocation(ourShader.ID, "transform");
		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transf));

		ourShader.use();
		glBindVertexArray(VAO);
		glDrawElements(GL_LINES, proc.indices_s, GL_UNSIGNED_INT, 0);


		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}

