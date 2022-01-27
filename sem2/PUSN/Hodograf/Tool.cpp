#include "Tool.h"
#include "imgui\imgui.h"
#include <string>
#include "Camera.h"

Tool::Tool() : Figure()
{
	sprintf_s(name, STRMAX, ("Tool - " + std::to_string(idx++) + " " + gen_random(12, idx)).c_str());
	figureType = FigureType::Tool;
	selected = true;
	canMove = true;
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

	if (l_old == l && r_old == r
		&& angle_old == angle
		&& block_size_old == block_size
		) return false;

	l_old = l; r_old = r; 
	angle_old = angle;
	block_size_old = block_size;

	int wheel_positions = 100;
	int vertices_count = wheel_positions + 2 + 4;
	vertices = std::vector<float>(6 * vertices_count);
	indices = std::vector<unsigned int>();

	for (int i = 0; i < wheel_positions; ++i)
	{
		float alpha = ((float)i / wheel_positions) * 2 * M_PI;

		vertices[6 * i + 0] = r * cos(alpha);
		vertices[6 * i + 1] = r * sin(alpha);
		vertices[6 * i + 2] = 0.0f;
		vertices[6 * i + 3] = 1.0f;
		vertices[6 * i + 4] = 0.0f;
		vertices[6 * i + 5] = 0.0f;
		if (i == wheel_positions - 1)
		{
			indices.push_back(i);
			indices.push_back(0);
		}
		else
		{
			indices.push_back(i);
			indices.push_back(i + 1);
		}
	}

	double angle_t = M_PI - angle;
	double r_t = r;
	double l_t = l+eps;
	glm::f64vec2 pos = { r_t * glm::cos(angle_t), r_t * glm::sin(angle_t) };
	double dist = sqrt(l_t * l_t - pos.y * pos.y);
	glm::f64vec2 pos2 = { pos.x + dist,0.0f };
	pos_now = pos2.x-l_t;

	int n = wheel_positions;
	vertices[6 * n + 0] = pos.x;
	vertices[6 * n + 1] = pos.y;
	vertices[6 * n + 2] = 0.0f;
	vertices[6 * n + 3] = 0.0f;
	vertices[6 * n + 4] = 1.0f;
	vertices[6 * n + 5] = 0.0f;
	indices.push_back(n);

	n++;
	vertices[6 * n + 0] = pos2.x;
	vertices[6 * n + 1] = pos2.y;
	vertices[6 * n + 2] = 0.0f;
	vertices[6 * n + 3] = 0.0f;
	vertices[6 * n + 4] = 1.0f;
	vertices[6 * n + 5] = 0.0f;
	indices.push_back(n);

	n++;
	vertices[6 * n + 0] = pos2.x;
	vertices[6 * n + 1] = pos2.y + block_size / 2;
	vertices[6 * n + 2] = 0.0f;
	vertices[6 * n + 3] = 1.0f;
	vertices[6 * n + 4] = 1.0f;
	vertices[6 * n + 5] = 0.0f;

	n++;
	vertices[6 * n + 0] = pos2.x;
	vertices[6 * n + 1] = pos2.y - block_size / 2;
	vertices[6 * n + 2] = 0.0f;
	vertices[6 * n + 3] = 1.0f;
	vertices[6 * n + 4] = 1.0f;
	vertices[6 * n + 5] = 0.0f;

	n++;
	vertices[6 * n + 0] = pos2.x + block_size;
	vertices[6 * n + 1] = pos2.y + block_size / 2;
	vertices[6 * n + 2] = 0.0f;
	vertices[6 * n + 3] = 1.0f;
	vertices[6 * n + 4] = 1.0f;
	vertices[6 * n + 5] = 0.0f;

	n++;
	vertices[6 * n + 0] = pos2.x + block_size;
	vertices[6 * n + 1] = pos2.y - block_size / 2;
	vertices[6 * n + 2] = 0.0f;
	vertices[6 * n + 3] = 1.0f;
	vertices[6 * n + 4] = 1.0f;
	vertices[6 * n + 5] = 0.0f;

	indices.push_back(n - 3);
	indices.push_back(n - 2);

	indices.push_back(n - 3);
	indices.push_back(n - 1);

	indices.push_back(n );
	indices.push_back(n - 2);

	indices.push_back(n - 1);
	indices.push_back(n);

	return true;
}