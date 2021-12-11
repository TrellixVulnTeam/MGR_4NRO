#include "Obstacle.h"
#include "imgui\imgui.h"
#include <string>
#include "Camera.h"

Obstacle::Obstacle() : Figure()
{
	sprintf_s(name, STRMAX, ("Obstacle - " + std::to_string(idx++) + " " + gen_random(12, idx)).c_str());
	figureType = FigureType::Obstacle;

}

void Obstacle::Initialize(std::shared_ptr<Program> _program)
{
	Figure::Initialize(_program);
	shader = window->shader2D;
}

void Obstacle::RecalcFigure()
{
	if (Create()) {
		shader->use();
		glBindVertexArray(VAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);

		glBufferData(
			GL_ARRAY_BUFFER,
			vertices.size() * sizeof(float),
			&vertices[0],
			GL_STATIC_DRAW
		);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

		glBufferData(
			GL_ELEMENT_ARRAY_BUFFER,
			indices.size() * sizeof(unsigned int),
			&indices[0],
			GL_STATIC_DRAW
		);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);

		glBindVertexArray(0);
	}
}

bool Obstacle::GetGuiInternal(std::shared_ptr<Figure> par)
{
	bool b = false;
	return b;
}

void Obstacle::Draw()
{
	Figure::Draw();
	shader->use();

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDrawElements(GL_LINES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

glm::vec2 Obstacle::GetPoint(int n)
{
	if (n == 1)
		return glm::vec2(xPos - width / 2, yPos - height / 2);
	if (n == 2)
		return glm::vec2(xPos + width / 2, yPos - height / 2);
	if (n == 3)
		return glm::vec2(xPos + width / 2, yPos + height / 2);
	if (n == 4)
		return glm::vec2(xPos - width / 2, yPos + height / 2);
	return glm::vec2(0.0f, 0.0f);
}


bool Obstacle::Create()
{
	bool fCreate = Figure::Create();


	if (x == xPos && y == yPos && h == height && w == width) return false;
	xPos = x; yPos = y; w = width; h = height;
	vertices = std::vector<float>(24);
	indices = std::vector<unsigned int>(8);
	
	glm::vec2 p0 = GetPoint(1);
	glm::vec2 p1 = GetPoint(2);
	glm::vec2 p2 = GetPoint(3);
	glm::vec2 p3 = GetPoint(4);

	vertices[0] = p0.x;
	vertices[1] = p0.y;
	vertices[2] = 0.0f;
	vertices[3] = 0.0f;
	vertices[4] = 0.0f;
	vertices[5] = 1.0f;

	vertices[6 + 0] = p1.x;
	vertices[6 + 1] = p1.y;
	vertices[6 + 2] = 0.0f;
	vertices[6 + 3] = 0.0f;
	vertices[6 + 4] = 0.0f;
	vertices[6 + 5] = 1.0f;

	vertices[12 + 0] = p2.x;
	vertices[12 + 1] = p2.y;
	vertices[12 + 2] = 0.0f;
	vertices[12 + 3] = 0.0f;
	vertices[12 + 4] = 0.0f;
	vertices[12 + 5] = 1.0f;

	vertices[18 + 0] = p3.x;
	vertices[18 + 1] = p3.y;
	vertices[18 + 2] = 0.0f;
	vertices[18 + 3] = 0.0f;
	vertices[18 + 4] = 0.0f;
	vertices[18 + 5] = 1.0f;

	indices[0] = 0;
	indices[1] = 1;
	indices[2] = 1;
	indices[3] = 2;
	indices[4] = 2;
	indices[5] = 3;
	indices[6] = 3;
	indices[7] = 0;

	return true;
}