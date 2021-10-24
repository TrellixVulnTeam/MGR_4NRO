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
	Figure::Initialize(_program);
	shader = _program->lightShader;

	data.clear();
	int XDIV = xSplit;
	int YDIV = zSplit;
	float d = M_PI / YDIV;
	for (int i = 0; i < zSplit; ++i)
		for (int j = 0; j < xSplit; ++j)
		{
			data.push_back(height);
		}
}

void Cube::RecalcFigure()
{
	if (genTexture || true)
	{
		GenTexture();
		genTexture = false;
	}
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

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 12 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 12 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);

		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 12 * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(2);

		glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 12 * sizeof(float), (void*)(8 * sizeof(float)));
		glEnableVertexAttribArray(3);

		glVertexAttribPointer(4, 2, GL_FLOAT, GL_FALSE, 12 * sizeof(float), (void*)(10 * sizeof(float)));
		glEnableVertexAttribArray(4);

		glBindVertexArray(0);
	}
}

void Cube::Drill(glm::vec3 from, glm::vec3 to)
{
	auto pos1 = GetPos(glm::vec2(from.x, from.z));
	auto pos2 = GetPos(glm::vec2(to.x, to.z));
	DoBresenham(pos1, pos2, from.y, to.y);
}

void Cube::DoBresenham(glm::ivec2 v1, glm::ivec2 v2, float h1, float h2) {
	int x = v1.x, y = v1.y, x2 = v2.x, y2 = v2.y;
	float height = h1;
	int w = x2 - x;
	int h = y2 - y;
	int dx1 = 0, dy1 = 0, dx2 = 0, dy2 = 0;
	if (w < 0) dx1 = -1; else if (w > 0) dx1 = 1;
	if (h < 0) dy1 = -1; else if (h > 0) dy1 = 1;
	if (w < 0) dx2 = -1; else if (w > 0) dx2 = 1;
	int longest = abs(w);
	int shortest = abs(h);
	if (!(longest > shortest)) {
		longest = abs(h);
		shortest = abs(w);
		if (h < 0) dy2 = -1; else if (h > 0) dy2 = 1;
		dx2 = 0;
	}
	float diff = (h2 - h1) / longest;
	int numerator = longest >> 1;
	for (int i = 0; i <= longest; i++) {
		PutDrill(x, y, height);
		height += diff;
		numerator += shortest;
		if (!(numerator < longest)) {
			numerator -= longest;
			x += dx1;
			y += dy1;
		}
		else {
			x += dx2;
			y += dy2;
		}
	}
}

void Cube::PutDrill(int x, int y, float height)
{
	for (int i = -2; i <= 2; ++i)
		for (int j = -2; j <= 2; ++j)
			SetHeight(x + i, y + j, height);
}

void Cube::SetHeight(int x, int y, float height)
{
	if (x < 0 || y < 0 || x >= xSplit || y >= zSplit) return;

	if (data[y * xSplit + x] > height)
		data[y * xSplit + x] = height;
}

glm::ivec2 Cube::GetPos(glm::vec2 coords)
{
	float xDiff = width / xSplit;
	float zDiff = length / zSplit;

	float xx = coords.x - (-width / 2);
	float zz = -(coords.y - (length / 2));

	return glm::ivec2(round(xx / xDiff), round(zz / zDiff));
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

	if (genTexture) GenTexture();
	auto texLocation = glGetUniformLocation(shader->ID, "colorTexture");
	glUniform1i(texLocation, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, program->colorTexture);

	texLocation = glGetUniformLocation(shader->ID, "heightTexture");
	glUniform1i(texLocation, 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, heightTextureID);

	int xDiffPos = glGetUniformLocation(shader->ID, "xDiff");
	glUniform1f(xDiffPos, 1.0f / xSplit);

	int yDiffPos = glGetUniformLocation(shader->ID, "yDiff");
	glUniform1f(yDiffPos, 1.0f / zSplit);

	//glEnable(GL_CULL_FACE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

bool Cube::Create()
{
	bool fCreate = Figure::Create();
	if (!fCreate) return false;
	int split_count = xSplit;
	int n = split_count + 1;

	float w = width / split_count;
	float l = length / split_count;
	float h = height / split_count;
	float wTex = 1.0f / split_count;
	float lTex = 1.0f / split_count;

	vertices.clear();
	indices.clear();

	int cur1 = vertices.size();
	int cur2 = indices.size();
	int a = 0;
	int nn;

#pragma region Dó³
	cur1 = vertices.size();
	cur2 = indices.size();

	for (int i = 0; i < 4 * 12; ++i) vertices.push_back(0.0f);
	for (int i = 0; i < 6; ++i)indices.push_back(0);

	for (int i = 0; i < 2; ++i)
		for (int j = 0; j < 2; ++j)
		{
			int idx1 = (2 * i + j) * 12 + cur1;
			vertices[idx1] = i * width - width / 2;
			vertices[idx1 + 1] = 0.0f;
			vertices[idx1 + 2] = -j * length + length / 2;
			vertices[idx1 + 3] = 0.0f;
			vertices[idx1 + 4] = -1.0f;
			vertices[idx1 + 5] = 0.0f;
			vertices[idx1 + 6] = i * 1.0f;
			vertices[idx1 + 7] = j * 1.0f;
			vertices[idx1 + 8] = -2.0f;
			vertices[idx1 + 9] = -2.0f;
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
#pragma region Góra
	cur1 = vertices.size();
	cur2 = indices.size();

	for (int i = 0; i < n * n * 12; ++i) vertices.push_back(0.0f);
	for (int i = 0; i < split_count * split_count * 2 * 3; ++i)indices.push_back(0);

	for (int i = 0; i < n; ++i)
	{
		int idx1 = i * 12 + cur1;
		vertices[idx1] = i * w - width / 2;
		vertices[idx1 + 1] = height;
		vertices[idx1 + 2] = length / 2;
		vertices[idx1 + 3] = -2.0f;
		vertices[idx1 + 4] = -2.0f;
		vertices[idx1 + 5] = -2.0f;
		vertices[idx1 + 6] = i * wTex;
		vertices[idx1 + 7] = 0.0f;
		vertices[idx1 + 8] = i * wTex;
		vertices[idx1 + 9] = 0.0f;
	}


	for (int j = 1; j < n; ++j)
	{
		float z = -l * j;

		int nn = j * n + a;
		int mm = (j - 1) * 2 * 3 * split_count + cur2;

		for (int i = 0; i < n; ++i)
		{
			int idx = nn + i;
			int idx1 = 12 * idx;
			vertices[idx1] = i * w - width / 2;
			vertices[idx1 + 1] = height;
			vertices[idx1 + 2] = z + length / 2;
			vertices[idx1 + 3] = -2.0f;
			vertices[idx1 + 4] = -2.0f;
			vertices[idx1 + 5] = -2.0f;
			vertices[idx1 + 6] = i * wTex;
			vertices[idx1 + 7] = j * lTex;
			vertices[idx1 + 8] = i * wTex;
			vertices[idx1 + 9] = j * lTex;

			int idx2 = (i - 1) * 2 * 3 + mm;
			if (i > 0)
			{
				indices[idx2] = idx - n - 1;
				indices[idx2 + 1] = idx - n;
				indices[idx2 + 2] = idx;

				indices[idx2 + 3] = idx;
				indices[idx2 + 4] = idx - 1;
				indices[idx2 + 5] = idx - n - 1;
			}
		}
	}
	a += n * n;
#pragma endregion

#pragma region Ty³
	cur1 = vertices.size();
	cur2 = indices.size();

	for (int i = 0; i < n * 2 * 12; ++i) vertices.push_back(0.0f);
	for (int i = 0; i < split_count * 2 * 3; ++i)indices.push_back(0);

	for (int i = 0; i < n; ++i)
	{
		int idx1 = i * 12 + cur1;
		vertices[idx1] = i * w - width / 2;
		vertices[idx1 + 1] = 0.0f;
		vertices[idx1 + 2] = -length / 2;
		vertices[idx1 + 3] = 0.0f;
		vertices[idx1 + 4] = 0.0f;
		vertices[idx1 + 5] = -1.0f;
		vertices[idx1 + 6] = i * wTex;
		vertices[idx1 + 7] = 0.0f;
		vertices[idx1 + 8] = -2.0f;
		vertices[idx1 + 9] = -2.0f;
	}

	nn = n + a;

	for (int i = 0; i < n; ++i)
	{
		int idx = nn + i;
		int idx1 = 12 * idx;
		vertices[idx1] = i * w - width / 2;
		vertices[idx1 + 1] = height;
		vertices[idx1 + 2] = -length / 2;
		vertices[idx1 + 3] = 0.0f;
		vertices[idx1 + 4] = 0.0f;
		vertices[idx1 + 5] = -1.0f;
		vertices[idx1 + 6] = i * wTex;
		vertices[idx1 + 7] = 1.0f;
		vertices[idx1 + 8] = i * wTex;
		vertices[idx1 + 9] = 1.0f;

		int idx2 = (i - 1) * 2 * 3 + cur2;
		if (i > 0)
		{
			indices[idx2] = idx - n - 1;
			indices[idx2 + 2] = idx - n;
			indices[idx2 + 1] = idx;

			indices[idx2 + 3] = idx;
			indices[idx2 + 5] = idx - 1;
			indices[idx2 + 4] = idx - n - 1;
		}
	}
	a += 2 * n;
#pragma endregion

#pragma region Przód
	cur1 = vertices.size();
	cur2 = indices.size();

	for (int i = 0; i < n * 2 * 12; ++i) vertices.push_back(0.0f);
	for (int i = 0; i < split_count * 2 * 3; ++i)indices.push_back(0);

	for (int i = 0; i < n; ++i)
	{
		int idx1 = i * 12 + cur1;
		vertices[idx1] = i * w - width / 2;
		vertices[idx1 + 1] = 0.0f;
		vertices[idx1 + 2] = length / 2;
		vertices[idx1 + 3] = 0.0f;
		vertices[idx1 + 4] = 0.0f;
		vertices[idx1 + 5] = 1.0f;
		vertices[idx1 + 6] = i * wTex;
		vertices[idx1 + 7] = 0.0f;
		vertices[idx1 + 8] = -2.0f;
		vertices[idx1 + 9] = -2.0f;
	}

	nn = n + a;

	for (int i = 0; i < n; ++i)
	{
		int idx = nn + i;
		int idx1 = 12 * idx;
		vertices[idx1] = i * w - width / 2;
		vertices[idx1 + 1] = height;
		vertices[idx1 + 2] = length / 2;
		vertices[idx1 + 3] = 0.0f;
		vertices[idx1 + 4] = 0.0f;
		vertices[idx1 + 5] = 1.0f;
		vertices[idx1 + 6] = i * wTex;
		vertices[idx1 + 7] = 1.0f;
		vertices[idx1 + 8] = i * wTex;
		vertices[idx1 + 9] = 0.0f;

		int idx2 = (i - 1) * 2 * 3 + cur2;
		if (i > 0)
		{
			indices[idx2] = idx - n - 1;
			indices[idx2 + 1] = idx - n;
			indices[idx2 + 2] = idx;

			indices[idx2 + 3] = idx;
			indices[idx2 + 4] = idx - 1;
			indices[idx2 + 5] = idx - n - 1;
		}

	}
	a += 2 * n;
#pragma endregion

#pragma region Lewo
	cur1 = vertices.size();
	cur2 = indices.size();

	for (int i = 0; i < n * 2 * 12; ++i) vertices.push_back(0.0f);
	for (int i = 0; i < split_count * 2 * 3; ++i)indices.push_back(0);

	for (int i = 0; i < n; ++i)
	{
		int idx1 = i * 12 + cur1;
		vertices[idx1] = -width / 2;
		vertices[idx1 + 1] = 0.0f;
		vertices[idx1 + 2] = -i * l + length / 2;
		vertices[idx1 + 3] = -1.0f;
		vertices[idx1 + 4] = 0.0f;
		vertices[idx1 + 5] = 0.0f;
		vertices[idx1 + 6] = i * lTex;
		vertices[idx1 + 7] = 0.0f;
		vertices[idx1 + 8] = -2.0f;
		vertices[idx1 + 9] = -2.0f;
	}

	nn = n + a;

	for (int i = 0; i < n; ++i)
	{
		int idx = nn + i;
		int idx1 = 12 * idx;
		vertices[idx1] = -width / 2;
		vertices[idx1 + 1] = height;
		vertices[idx1 + 2] = -i * l + length / 2;
		vertices[idx1 + 3] = -1.0f;
		vertices[idx1 + 4] = 0.0f;
		vertices[idx1 + 5] = 0.0f;
		vertices[idx1 + 6] = i * lTex;
		vertices[idx1 + 7] = 1.0f;
		vertices[idx1 + 8] = 0.0f;
		vertices[idx1 + 9] = i * lTex;

		int idx2 = (i - 1) * 2 * 3 + cur2;
		if (i > 0)
		{
			indices[idx2] = idx - n - 1;
			indices[idx2 + 2] = idx - n;
			indices[idx2 + 1] = idx;

			indices[idx2 + 3] = idx;
			indices[idx2 + 5] = idx - 1;
			indices[idx2 + 4] = idx - n - 1;
		}
	}
	a += 2 * n;
#pragma endregion

#pragma region Prawo
	cur1 = vertices.size();
	cur2 = indices.size();

	for (int i = 0; i < n * 2 * 12; ++i) vertices.push_back(0.0f);
	for (int i = 0; i < split_count * 2 * 3; ++i)indices.push_back(0);

	for (int i = 0; i < n; ++i)
	{
		int idx1 = i * 12 + cur1;
		vertices[idx1] = width / 2;
		vertices[idx1 + 1] = 0.0f;
		vertices[idx1 + 2] = -i * l + length / 2;
		vertices[idx1 + 3] = 1.0f;
		vertices[idx1 + 4] = 0.0f;
		vertices[idx1 + 5] = 0.0f;
		vertices[idx1 + 6] = i * lTex;
		vertices[idx1 + 7] = 0.0f;
		vertices[idx1 + 8] = -2.0f;
		vertices[idx1 + 9] = -2.0f;
	}

	nn = n + a;

	for (int i = 0; i < n; ++i)
	{
		int idx = nn + i;
		int idx1 = 12 * idx;
		vertices[idx1] = width / 2;
		vertices[idx1 + 1] = height;
		vertices[idx1 + 2] = -i * l + length / 2;
		vertices[idx1 + 3] = 1.0f;
		vertices[idx1 + 4] = 0.0f;
		vertices[idx1 + 5] = 0.0f;
		vertices[idx1 + 6] = i * lTex;
		vertices[idx1 + 7] = 1.0f;
		vertices[idx1 + 8] = 1.0f;
		vertices[idx1 + 9] = i * lTex;

		int idx2 = (i - 1) * 2 * 3 + cur2;
		if (i > 0)
		{
			indices[idx2] = idx - n - 1;
			indices[idx2 + 1] = idx - n;
			indices[idx2 + 2] = idx;

			indices[idx2 + 3] = idx;
			indices[idx2 + 4] = idx - 1;
			indices[idx2 + 5] = idx - n - 1;
		}
	}
	a += 2 * n;
#pragma endregion
	return true;
}


void Cube::GenTexture()
{
	shader->use();
	glDeleteTextures(1, &heightTextureID);
	glGenTextures(1, &heightTextureID);
	glBindTexture(GL_TEXTURE_2D, heightTextureID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, xSplit, zSplit, 0, GL_RED, GL_FLOAT, data.data());
	glGenerateMipmap(GL_TEXTURE_2D);

}