#include "Cube.h"
#include "imgui\imgui.h"
#include <string>

Cube::Cube() : Figure()
{
	sprintf_s(name, STRMAX, ("Cube - " + std::to_string(idx++) + " " + gen_random(12, idx)).c_str());
	figureType = FigureType::Cube;

}

void Cube::Initialize(std::shared_ptr<Program> _program)
{
	height = _program->height;
	width = _program->width;
	length = _program->length;
	xSplit = _program->xSplit;
	zSplit = _program->ySplit;
	xDiff = width / xSplit;
	zDiff = length / zSplit;

	Figure::Initialize(_program);
	shader = window->lightShader;
}

void Cube::RecalcFigure()
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


bool Cube::GetGuiInternal(std::shared_ptr<Figure> par)
{
	bool b = false;
	return b;
}

void Cube::Draw()
{
	Figure::Draw();
	shader->use();

	//glEnable(GL_CULL_FACE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

bool Cube::Create()
{
	bool fCreate = Figure::Create();
	if (!fCreate) return false;
	int nX = xSplit + 1;
	int nZ = zSplit + 1;

	float w = xDiff;
	float l = zDiff;
	float wTex = 1.0f / xSplit;
	float lTex = 1.0f / zSplit;

	vertices.clear();
	indices.clear();

	int cur1 = vertices.size();
	int cur2 = indices.size();
	int a = 0;
	int nn;

#pragma region D??
	cur1 = vertices.size();
	cur2 = indices.size();

	for (int i = 0; i < 4 * 9; ++i) vertices.push_back(0.0f);
	for (int i = 0; i < 6; ++i)indices.push_back(0);

	for (int i = 0; i < 2; ++i)
		for (int j = 0; j < 2; ++j)
		{
			int idx1 = (2 * i + j) * 9 + cur1;
			vertices[idx1] = i * width - width / 2;
			vertices[idx1 + 1] = -height / 2;
			vertices[idx1 + 2] = -j * length + length / 2;
			vertices[idx1 + 3] = 0.0f;
			vertices[idx1 + 4] = -1.0f;
			vertices[idx1 + 5] = 0.0f;
			vertices[idx1 + 6] = 1.0f;
			vertices[idx1 + 7] = 0.0f;
			vertices[idx1 + 8] = 0.0f;
		}

	int idx2 = cur2;

	indices[idx2] = a;
	indices[idx2 + 1] = a + 1;
	indices[idx2 + 2] = a + 3;

	indices[idx2 + 3] = a + 3;
	indices[idx2 + 4] = a + 2;
	indices[idx2 + 5] = a;
	a += 4;
#pragma endregion
#pragma region G?ra
	cur1 = vertices.size();
	cur2 = indices.size();

	for (int i = 0; i < nX * nZ * 9; ++i) vertices.push_back(0.0f);
	for (int i = 0; i < xSplit * zSplit * 2 * 3; ++i)indices.push_back(0);

	for (int i = 0; i < nX; ++i)
	{
		int idx1 = i * 9 + cur1;
		vertices[idx1] = i * w - width / 2;
		vertices[idx1 + 1] = height / 2;
		vertices[idx1 + 2] = length / 2;
		vertices[idx1 + 3] = 0.0f;
		vertices[idx1 + 4] = 1.0f;
		vertices[idx1 + 5] = 0.0f;
		vertices[idx1 + 6] = 1.0f;
		vertices[idx1 + 7] = 0.0f;
		vertices[idx1 + 8] = 0.0f;
	}


	for (int j = 1; j < nZ; ++j)
	{
		float z = -l * j;

		int nn = j * nX + a;
		int mm = (j - 1) * 2 * 3 * xSplit + cur2;

		for (int i = 0; i < nX; ++i)
		{
			int idx = nn + i;
			int idx1 = 9 * idx;
			vertices[idx1] = i * w - width / 2;
			vertices[idx1 + 1] = height / 2;
			vertices[idx1 + 2] = z + length / 2;
			vertices[idx1 + 3] = 0.0f;
			vertices[idx1 + 4] = 1.0f;
			vertices[idx1 + 5] = 0.0f;
			vertices[idx1 + 6] = 1.0f;
			vertices[idx1 + 7] = 0.0f;
			vertices[idx1 + 8] = 0.0f;

			int idx2 = (i - 1) * 2 * 3 + mm;
			if (i > 0)
			{
				indices[idx2] = idx - nX - 1;
				indices[idx2 + 1] = idx - nX;
				indices[idx2 + 2] = idx;

				indices[idx2 + 3] = idx;
				indices[idx2 + 4] = idx - 1;
				indices[idx2 + 5] = idx - nX - 1;
			}
		}
	}
	a += nZ * nX;
#pragma endregion

#pragma region Ty?
	cur1 = vertices.size();
	cur2 = indices.size();

	for (int i = 0; i < nX * 2 * 9; ++i) vertices.push_back(0.0f);
	for (int i = 0; i < xSplit * 2 * 3; ++i)indices.push_back(0);

	for (int i = 0; i < nX; ++i)
	{
		int idx1 = i * 9 + cur1;
		vertices[idx1] = i * w - width / 2;
		vertices[idx1 + 1] = -height / 2;
		vertices[idx1 + 2] = -length / 2;
		vertices[idx1 + 3] = 0.0f;
		vertices[idx1 + 4] = 0.0f;
		vertices[idx1 + 5] = -1.0f;
		vertices[idx1 + 6] = 1.0f;
		vertices[idx1 + 7] = 0.0f;
		vertices[idx1 + 8] = 0.0f;
	}

	nn = nX + a;

	for (int i = 0; i < nX; ++i)
	{
		int idx = nn + i;
		int idx1 = 9 * idx;
		vertices[idx1] = i * w - width / 2;
		vertices[idx1 + 1] = height / 2;
		vertices[idx1 + 2] = -length / 2;
		vertices[idx1 + 3] = 0.0f;
		vertices[idx1 + 4] = 0.0f;
		vertices[idx1 + 5] = -1.0f;
		vertices[idx1 + 6] = 1.0f;
		vertices[idx1 + 7] = 0.0f;
		vertices[idx1 + 8] = 0.0f;

		int idx2 = (i - 1) * 2 * 3 + cur2;
		if (i > 0)
		{
			indices[idx2] = idx - nX - 1;
			indices[idx2 + 2] = idx - nX;
			indices[idx2 + 1] = idx;

			indices[idx2 + 3] = idx;
			indices[idx2 + 5] = idx - 1;
			indices[idx2 + 4] = idx - nX - 1;
		}
	}
	a += 2 * nX;
#pragma endregion

#pragma region Prz?d
	cur1 = vertices.size();
	cur2 = indices.size();

	for (int i = 0; i < nX * 2 * 9; ++i) vertices.push_back(0.0f);
	for (int i = 0; i < xSplit * 2 * 3; ++i)indices.push_back(0);

	for (int i = 0; i < nX; ++i)
	{
		int idx1 = i * 9 + cur1;
		vertices[idx1] = i * w - width / 2;
		vertices[idx1 + 1] = -height / 2;
		vertices[idx1 + 2] = length / 2;
		vertices[idx1 + 3] = 0.0f;
		vertices[idx1 + 4] = 0.0f;
		vertices[idx1 + 5] = 1.0f;
		vertices[idx1 + 6] = 1.0f;
		vertices[idx1 + 7] = 0.0f;
		vertices[idx1 + 8] = 0.0f;
	}

	nn = nX + a;

	for (int i = 0; i < nX; ++i)
	{
		int idx = nn + i;
		int idx1 = 9 * idx;
		vertices[idx1] = i * w - width / 2;
		vertices[idx1 + 1] = height / 2;
		vertices[idx1 + 2] = length / 2;
		vertices[idx1 + 3] = 0.0f;
		vertices[idx1 + 4] = 0.0f;
		vertices[idx1 + 5] = 1.0f;
		vertices[idx1 + 6] = 1.0f;
		vertices[idx1 + 7] = 0.0f;
		vertices[idx1 + 8] = 0.0f;

		int idx2 = (i - 1) * 2 * 3 + cur2;
		if (i > 0)
		{
			indices[idx2] = idx - nX - 1;
			indices[idx2 + 1] = idx - nX;
			indices[idx2 + 2] = idx;

			indices[idx2 + 3] = idx;
			indices[idx2 + 4] = idx - 1;
			indices[idx2 + 5] = idx - nX - 1;
		}

	}
	a += 2 * nX;
#pragma endregion

#pragma region Lewo
	cur1 = vertices.size();
	cur2 = indices.size();

	for (int i = 0; i < nZ * 2 * 9; ++i) vertices.push_back(0.0f);
	for (int i = 0; i < zSplit * 2 * 3; ++i)indices.push_back(0);

	for (int i = 0; i < nZ; ++i)
	{
		int idx1 = i * 9 + cur1;
		vertices[idx1] = -width / 2;
		vertices[idx1 + 1] = -height / 2;
		vertices[idx1 + 2] = -i * l + length / 2;
		vertices[idx1 + 3] = -1.0f;
		vertices[idx1 + 4] = 0.0f;
		vertices[idx1 + 5] = 0.0f;
		vertices[idx1 + 6] = 1.0f;
		vertices[idx1 + 7] = 0.0f;
		vertices[idx1 + 8] = 0.0f;
	}

	nn = nZ + a;

	for (int i = 0; i < nZ; ++i)
	{
		int idx = nn + i;
		int idx1 = 9 * idx;
		vertices[idx1] = -width / 2;
		vertices[idx1 + 1] = height / 2;
		vertices[idx1 + 2] = -i * l + length / 2;
		vertices[idx1 + 3] = -1.0f;
		vertices[idx1 + 4] = 0.0f;
		vertices[idx1 + 5] = 0.0f;
		vertices[idx1 + 6] = 1.0f;
		vertices[idx1 + 7] = 0.0f;
		vertices[idx1 + 8] = 0.0f;

		int idx2 = (i - 1) * 2 * 3 + cur2;
		if (i > 0)
		{
			indices[idx2] = idx - nZ - 1;
			indices[idx2 + 2] = idx - nZ;
			indices[idx2 + 1] = idx;

			indices[idx2 + 3] = idx;
			indices[idx2 + 5] = idx - 1;
			indices[idx2 + 4] = idx - nZ - 1;
		}
	}
	a += 2 * nZ;
#pragma endregion

#pragma region Prawo
	cur1 = vertices.size();
	cur2 = indices.size();

	for (int i = 0; i < nZ * 2 * 9; ++i) vertices.push_back(0.0f);
	for (int i = 0; i < zSplit * 2 * 3; ++i)indices.push_back(0);

	for (int i = 0; i < nZ; ++i)
	{
		int idx1 = i * 9 + cur1;
		vertices[idx1] = width / 2;
		vertices[idx1 + 1] = -height / 2;
		vertices[idx1 + 2] = -i * l + length / 2;
		vertices[idx1 + 3] = 1.0f;
		vertices[idx1 + 4] = 0.0f;
		vertices[idx1 + 5] = 0.0f;
		vertices[idx1 + 6] = 1.0f;
		vertices[idx1 + 7] = 0.0f;
		vertices[idx1 + 8] = 0.0f;
	}

	nn = nZ + a;

	for (int i = 0; i < nZ; ++i)
	{
		int idx = nn + i;
		int idx1 = 9 * idx;
		vertices[idx1] = width / 2;
		vertices[idx1 + 1] = height / 2;
		vertices[idx1 + 2] = -i * l + length / 2;
		vertices[idx1 + 3] = 1.0f;
		vertices[idx1 + 4] = 0.0f;
		vertices[idx1 + 5] = 0.0f;
		vertices[idx1 + 6] = 1.0f;
		vertices[idx1 + 7] = 0.0f;
		vertices[idx1 + 8] = 0.0f;

		int idx2 = (i - 1) * 2 * 3 + cur2;
		if (i > 0)
		{
			indices[idx2] = idx - nZ - 1;
			indices[idx2 + 1] = idx - nZ;
			indices[idx2 + 2] = idx;

			indices[idx2 + 3] = idx;
			indices[idx2 + 4] = idx - 1;
			indices[idx2 + 5] = idx - nZ - 1;
		}
	}
	a += 2 * nZ;
#pragma endregion
	return true;
}