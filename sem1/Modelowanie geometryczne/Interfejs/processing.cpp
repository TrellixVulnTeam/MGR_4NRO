#pragma once
#include "processing.h"

Processing::Processing()
{
	vertices = new float[1];
	indices = new unsigned int[1];
	m_new = 8;
	n_new = 8;

	CreateTorus();
}

Processing::~Processing()
{
	delete[] vertices;
	delete[] indices;
}


void Processing::framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void Processing::processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

bool Processing::CreateTorus()
{
	if (m_new == m_old && n_new == n_old) return false;

	m_old = m_new;
	n_old = n_new;
	int n = n_new;
	int m = m_new;

	float r = 0.2f;
	float R = 1.0f;

	delete[] vertices;
	delete[] indices;
	vertices_s = n * m * 6;
	vertices = new float[vertices_s];
	indices_s = 4 * n * m;
	indices = new unsigned int[indices_s];

	for (int i = 0; i < m; ++i)
	{
		float alpha = i * (2*M_PI) / m;
		float cosa = cos(alpha);
		float sina = sin(alpha);
		for (int j = 0; j < n; ++j)
		{
			int idx = i * n + j;
			int idx1 = idx * 6;
			int idx2 = idx * 4;
			float beta = j * (2 * M_PI) / n;
			float cosb = cos(beta);
			float sinb = sin(beta);

			float x = cosa * (r * cosb + R);
			float y = sina * (r * cosb + R);
			float z = r * sinb;

			vertices[idx1] = x;
			vertices[idx1 + 1] = y;
			vertices[idx1 + 2] = z;
			vertices[idx1 + 3] = 1.0f;
			vertices[idx1 + 4] = 0.0f;
			vertices[idx1 + 5] = 0.0f;

			indices[idx2] = idx;
			if (j < n - 1)
				indices[idx2 + 1] = idx + 1;
			else
				indices[idx2 + 1] = idx - n + 1;

			indices[idx2 + 2] = idx;
			if (i < m - 1)
				indices[idx2 + 3] = idx + n;
			else
				indices[idx2 + 3] = idx % n;
		}
	}

	return true;


}

