#include "Cylinder.h"
#include "imgui\imgui.h"
#include <string>
#include "Camera.h"

Cylinder::Cylinder(bool mid_, float r_, float h_, glm::vec3 col_) :mid(mid_), r(r_), h(h_), col(col_), Figure()
{
	sprintf_s(name, STRMAX, ("Cylinder - " + std::to_string(idx++) + " " + gen_random(12, idx)).c_str());
	figureType = FigureType::Cylinder;

}

void Cylinder::Initialize(std::shared_ptr<Program> _program)
{
	Figure::Initialize(_program);
	shader = window->lightShader;
}

void Cylinder::RecalcFigure()
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


bool Cylinder::GetGuiInternal(std::shared_ptr<Figure> par)
{
	bool b = false;
	return b;
}

void Cylinder::Draw()
{
	Figure::Draw();
	shader->use();

	int invLoc = glGetUniformLocation(shader->ID, "inversed");
	auto inversed = glm::transpose(glm::inverse(window->cam->GetViewportMatrix() * GetModel()));
	glUniformMatrix4fv(invLoc, 1, GL_FALSE, glm::value_ptr(inversed));
	//glBindVertexArray(VAO);
	//glEnable(GL_CULL_FACE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

bool Cylinder::Create()
{
	bool fCreate = Figure::Create();
	if (!fCreate) return false;

	vertices.clear();
	indices.clear();
	float diff = 2 * M_PI / n;
	vertices = std::vector<float>(9 * (4 * n + 2));
	int ind;
	float angle = 0;
	for (auto i = 0; i < n; ++i)
	{
		auto ca = cos(angle);
		auto sa = sin(angle);
		vertices[36 * i + 0] = r * ca;
		vertices[36 * i + 1] = mid ? -h / 2 : 0.0f;
		vertices[36 * i + 2] = r * sa;
		vertices[36 * i + 3] = ca;
		vertices[36 * i + 4] = 0.0f;
		vertices[36 * i + 5] = sa;
		vertices[36 * i + 6] = col.r;
		vertices[36 * i + 7] = col.g;
		vertices[36 * i + 8] = col.b;

		vertices[36 * i + 9 + 0] = r * ca;
		vertices[36 * i + 9 + 1] = mid ? -h / 2 : 0.0f;
		vertices[36 * i + 9 + 2] = r * sa;
		vertices[36 * i + 9 + 3] = 0.0f;
		vertices[36 * i + 9 + 4] = -1.0f;
		vertices[36 * i + 9 + 5] = 0.0f;
		vertices[36 * i + 9 + 6] = col.r;
		vertices[36 * i + 9 + 7] = col.g;
		vertices[36 * i + 9 + 8] = col.b;

		vertices[36 * i + 18 + 0] = r * ca;
		vertices[36 * i + 18 + 1] = mid ? h / 2 : h;
		vertices[36 * i + 18 + 2] = r * sa;
		vertices[36 * i + 18 + 3] = ca;
		vertices[36 * i + 18 + 4] = 0.0f;
		vertices[36 * i + 18 + 5] = sa;
		vertices[36 * i + 18 + 6] = col.r;
		vertices[36 * i + 18 + 7] = col.g;
		vertices[36 * i + 18 + 8] = col.b;

		vertices[36 * i + 27 + 0] = r * ca;
		vertices[36 * i + 27 + 1] = mid ? h / 2 : h;
		vertices[36 * i + 27 + 2] = r * sa;
		vertices[36 * i + 27 + 3] = 0.0f;
		vertices[36 * i + 27 + 4] = 1.0f;
		vertices[36 * i + 27 + 5] = 0.0f;
		vertices[36 * i + 27 + 6] = col.r;
		vertices[36 * i + 27 + 7] = col.g;
		vertices[36 * i + 27 + 8] = col.b;
		angle += diff;
	}

	vertices[36 * n + 0] = 0.0f;
	vertices[36 * n + 1] = mid ? -h / 2 : 0.0f;
	vertices[36 * n + 2] = 0.0f;
	vertices[36 * n + 3] = 0.0f;
	vertices[36 * n + 4] = -1.0f;
	vertices[36 * n + 5] = 0.0f;
	vertices[36 * n + 6] = col.r;
	vertices[36 * n + 7] = col.g;
	vertices[36 * n + 8] = col.b;

	vertices[36 * n + 9 + 0] = 0.0f;
	vertices[36 * n + 9 + 1] = mid ? h / 2 : h;
	vertices[36 * n + 9 + 2] = 0.0f;
	vertices[36 * n + 9 + 3] = 0.0f;
	vertices[36 * n + 9 + 4] = 1.0f;
	vertices[36 * n + 9 + 5] = 0.0f;
	vertices[36 * n + 9 + 6] = col.r;
	vertices[36 * n + 9 + 7] = col.g;
	vertices[36 * n + 9 + 8] = col.b;

	int b = 4 * n;
	int t = 4 * n + 1;

	indices = std::vector<unsigned int>(3 * 4 * n);

	for (auto i = 0; i < n; ++i)
	{
		indices[12 * i + 0] = 4 * i;
		indices[12 * i + 1] = (4 * i + 4) % (4 * n);
		indices[12 * i + 2] = (4 * i + 6) % (4 * n);

		indices[12 * i + 3] = 4 * i;
		indices[12 * i + 4] = (4 * i + 6) % (4 * n);
		indices[12 * i + 5] = (4 * i + 2);

		indices[12 * i + 6] = 4 * i;
		indices[12 * i + 7] = (4 * i + 4) % (4 * n);
		indices[12 * i + 8] = b;

		indices[12 * i + 9] = (4 * i + 6) % (4 * n);
		indices[12 * i + 10] = (4 * i + 2);
		indices[12 * i + 11] = t;

	}

	if (onX || onZ)
		for (int i = 0; i < 4 * n + 2; ++i)
		{
			int ind = 9 * i;
			if (onX)
			{
				float t = vertices[ind + 1];
				vertices[ind + 1] = vertices[ind];
				vertices[ind] = t;

				t = vertices[ind + 4];
				vertices[ind + 4] = vertices[ind + 3];
				vertices[ind + 3] = t;
			}
			if (onZ)
			{
				float t = vertices[ind + 1];
				vertices[ind + 1] = vertices[ind + 2];
				vertices[ind + 2] = t;

				t = vertices[ind + 4];
				vertices[ind + 4] = vertices[ind + 5];
				vertices[ind + 5] = t;
			}
		}

	return true;
}