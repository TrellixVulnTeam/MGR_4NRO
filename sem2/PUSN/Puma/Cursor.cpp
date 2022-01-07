#include "Cursor.h"
#include "imgui\imgui.h"
#include <string>
#include "Camera.h"

Cursor::Cursor() :Figure()
{
	sprintf_s(name, STRMAX, ("Cursor - " + std::to_string(idx++) + " " + gen_random(12, idx)).c_str());
	figureType = FigureType::Cursor;

}

void Cursor::Initialize(std::shared_ptr<Program> _program)
{
	Figure::Initialize(_program);
	shader = window->shader;
}

void Cursor::RecalcFigure()
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

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);

		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(2);

		glBindVertexArray(0);
	}
}


bool Cursor::GetGuiInternal(std::shared_ptr<Figure> par)
{
	bool b = false;
	return b;
}

void Cursor::Draw()
{
	Figure::Draw();
	shader->use();

	int invLoc = glGetUniformLocation(shader->ID, "inversed");
	auto inversed = glm::transpose(glm::inverse(window->cam->GetViewportMatrix() * GetModel()));
	glUniformMatrix4fv(invLoc, 1, GL_FALSE, glm::value_ptr(inversed));
	//glBindVertexArray(VAO);
	//glEnable(GL_CULL_FACE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDrawElements(GL_LINES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

bool Cursor::Create()
{
	bool fCreate = Figure::Create();
	if (!fCreate) return false;

	vertices.clear();
	indices.clear();

	int n = 0;
	vertices = std::vector<float>(9 * 6);
	vertices[n + 0] = 0.0f;
	vertices[n + 1] = 0.0f;
	vertices[n + 2] = 0.0f;
	vertices[n + 3] = 0.0f;
	vertices[n + 4] = 0.0f;
	vertices[n + 5] = 0.0f;
	vertices[n + 6] = 1.0f;
	vertices[n + 7] = 0.0f;
	vertices[n + 8] = 0.0f;
	n += 9;

	vertices[n + 0] = 1.0f;
	vertices[n + 1] = 0.0f;
	vertices[n + 2] = 0.0f;
	vertices[n + 3] = 0.0f;
	vertices[n + 4] = 0.0f;
	vertices[n + 5] = 0.0f;
	vertices[n + 6] = 1.0f;
	vertices[n + 7] = 0.0f;
	vertices[n + 8] = 0.0f;
	n += 9;

	vertices[n + 0] = 0.0f;
	vertices[n + 1] = 0.0f;
	vertices[n + 2] = 0.0f;
	vertices[n + 3] = 0.0f;
	vertices[n + 4] = 0.0f;
	vertices[n + 5] = 0.0f;
	vertices[n + 6] = 0.0f;
	vertices[n + 7] = 1.0f;
	vertices[n + 8] = 0.0f;
	n += 9;

	vertices[n + 0] = 0.0f;
	vertices[n + 1] = 1.0f;
	vertices[n + 2] = 0.0f;
	vertices[n + 3] = 0.0f;
	vertices[n + 4] = 0.0f;
	vertices[n + 5] = 0.0f;
	vertices[n + 6] = 0.0f;
	vertices[n + 7] = 1.0f;
	vertices[n + 8] = 0.0f;
	n += 9;

	vertices[n + 0] = 0.0f;
	vertices[n + 1] = 0.0f;
	vertices[n + 2] = 0.0f;
	vertices[n + 3] = 0.0f;
	vertices[n + 4] = 0.0f;
	vertices[n + 5] = 0.0f;
	vertices[n + 6] = 0.0f;
	vertices[n + 7] = 0.0f;
	vertices[n + 8] = 1.0f;
	n += 9;

	vertices[n + 0] = 0.0f;
	vertices[n + 1] = 0.0f;
	vertices[n + 2] = 1.0f;
	vertices[n + 3] = 0.0f;
	vertices[n + 4] = 0.0f;
	vertices[n + 5] = 0.0f;
	vertices[n + 6] = 0.0f;
	vertices[n + 7] = 0.0f;
	vertices[n + 8] = 1.0f;
	n += 9;

	indices.push_back(0);
	indices.push_back(1);
	indices.push_back(2);
	indices.push_back(3);
	indices.push_back(4);
	indices.push_back(5);
	return true;
}