#include "Tool.h"
#include "imgui\imgui.h"
#include <string>
#include "Camera.h"

Tool::Tool() : Figure()
{
	sprintf_s(name, STRMAX, ("Tool - " + std::to_string(idx++) + " " + gen_random(12, idx)).c_str());
	figureType = FigureType::Tool;

}

void Tool::Initialize(std::shared_ptr<Program> _program)
{
	Figure::Initialize(_program);
	shader = window->shader2D;
}

void Tool::RecalcFigure()
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

void Tool::InverseKinematics(glm::vec2 pos)
{
	double dist = sqrt(pos.x * pos.x + pos.y * pos.y);
	if (dist > len1 + len2)
	{
		program->error = std::string("Can't set tool in that position");
		return;
	}

	double g1 = acos((l1 * l1 + l2 * l2 - dist * dist) / (2 * l1 * l2));
	double g2 = 2 * M_PI - g1;

	double h1 = acos((l1 * l1 + dist * dist - l2 * l2) / (2 * l1 * dist));
	double h2 = 2 * M_PI - h1;

	double b1 = M_PI - g1;
	double b2 = M_PI - g2;


	double angle = atan(pos.y / pos.x);
	if (pos.x < 0) angle += M_PI;
	double a1 = angle - h1, a2 = angle - h2;

	program->selection = true;
	program->opt1 = glm::vec2(a1, b1);
	program->opt2 = glm::vec2(a2, b2);
}

glm::vec2 Tool::GetPoint(int n, float alpha, float beta)
{
	float al = alpha / 180.0f * M_PI;
	float bt = beta / 180.0f * M_PI;
	if (n == 1)
		return glm::vec2(0.0f, 0.0f);
	if (n == 2)
		return glm::vec2(len1 * cos(al), len1 * sin(al));
	if (n == 3)
		return glm::vec2(len1 * cos(al) + len2 * cos(al + bt)
			, len1 * sin(al) + len2 * sin(al + bt));
	return glm::vec2(0.0f, 0.0f);
}


bool Tool::GetGuiInternal(std::shared_ptr<Figure> par)
{
	bool b = false;
	return b;
}

void Tool::Draw()
{
	Figure::Draw();
	shader->use();

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDrawElements(GL_LINES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

bool Tool::Create()
{
	bool fCreate = Figure::Create();

	while (alpha < 0.0f) alpha += 360.0f;
	while (alpha > 360.0f) alpha -= 360.0f;
	while (beta < 0.0f) beta += 360.0f;
	while (beta > 360.0f) beta -= 360.0f;

	if (l1 == len1 && l2 == len2 && a == alpha && b == beta) return false;
	len1 = l1; len2 = l2; a = alpha; b = beta;
	vertices = std::vector<float>(18);
	indices = std::vector<unsigned int>(4);

	glm::vec2 p0 = GetPoint(1, a, b);
	glm::vec2 p1 = GetPoint(2, a, b);
	glm::vec2 p2 = GetPoint(3, a, b);

	vertices[0] = p0.x;
	vertices[1] = p0.y;
	vertices[2] = 0.0f;
	vertices[3] = 1.0f;
	vertices[4] = 0.0f;
	vertices[5] = 0.0f;

	vertices[6 + 0] = p1.x;
	vertices[6 + 1] = p1.y;
	vertices[6 + 2] = 0.0f;
	vertices[6 + 3] = 1.0f;
	vertices[6 + 4] = 0.0f;
	vertices[6 + 5] = 0.0f;

	vertices[12 + 0] = p2.x;
	vertices[12 + 1] = p2.y;
	vertices[12 + 2] = 0.0f;
	vertices[12 + 3] = 1.0f;
	vertices[12 + 4] = 0.0f;
	vertices[12 + 5] = 0.0f;

	indices[0] = 0;
	indices[1] = 1;
	indices[2] = 1;
	indices[3] = 2;

	return true;
}