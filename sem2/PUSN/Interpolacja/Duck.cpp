#include "Duck.h"
#include "imgui\imgui.h"
#include <string>
#include "Camera.h"

Duck::Duck() : Figure()
{
	sprintf_s(name, STRMAX, ("Duck - " + std::to_string(idx++) + " " + gen_random(12, idx)).c_str());
	figureType = FigureType::Duck;

}

void Duck::Initialize(std::shared_ptr<Program> _program)
{
	Figure::Initialize(_program);
	shader = window->lightShader;
}

void Duck::RecalcFigure()
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

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);

		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(2);

		glBindVertexArray(0);
	}
}


bool Duck::GetGuiInternal(std::shared_ptr<Figure> par)
{
	bool b = false;
	return b;
}

void Duck::Draw()
{
	Figure::Draw();
	shader->use();

	auto texLocation = glGetUniformLocation(shader->ID, "colorTexture");
	glUniform1i(texLocation, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, program->currentWindow->colorTexture);

	int invLoc = glGetUniformLocation(shader->ID, "inversed");
	auto inversed = glm::transpose(glm::inverse(program->cam->GetViewportMatrix() * GetModel()));
	glUniformMatrix4fv(invLoc, 1, GL_FALSE, glm::value_ptr(inversed));
	//glBindVertexArray(VAO);
	//glEnable(GL_CULL_FACE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

bool Duck::Create()
{
	bool fCreate = Figure::Create();
	if (!fCreate) return false;

	float coeff = 4.0f / 100.0f;
	std::ifstream input;
	// In general we shouldn't throw exceptions on end-of-file,
	// however, in case of this file format if we reach the end
	// of a file before we read all values, the file is
	// ill-formated and we would need to throw an exception anyway
	input.exceptions(std::ios::badbit | std::ios::failbit | std::ios::eofbit);
	input.open(L"resources/duck/duck.txt");

	int vn, in, kn;
	input >> vn;

	std::vector<glm::vec3> verts_tmp(vn);
	std::vector<glm::vec3> norms_tmp(vn);
	std::vector<glm::vec2> tex_tmp(vn);
	for (auto i = 0; i < vn; ++i)
	{
		input >> verts_tmp[i].x >> verts_tmp[i].y >> verts_tmp[i].z;
		input >> norms_tmp[i].x >> norms_tmp[i].y >> norms_tmp[i].z;
		input >> tex_tmp[i].x >> tex_tmp[i].y;
	}

	vertices = std::vector<float>(8 * vn);
	int ind;
	for (auto i = 0; i < vn; ++i)
	{
		vertices[8 * i + 0] = verts_tmp[i].z * coeff;
		vertices[8 * i + 1] = verts_tmp[i].y * coeff;
		vertices[8 * i + 2] = -verts_tmp[i].x * coeff;
		vertices[8 * i + 3] = norms_tmp[i].z;
		vertices[8 * i + 4] = norms_tmp[i].y;
		vertices[8 * i + 5] = -norms_tmp[i].x;
		vertices[8 * i + 6] = tex_tmp[i].x;
		vertices[8 * i + 7] = tex_tmp[i].y;
	}

	input >> in;
	indices = std::vector<unsigned int>(3 * in);
	for (auto i = 0; i < in; ++i) {
		input >> indices[3 * i] >> indices[3 * i + 1] >> indices[3 * i + 2];
	}

	return true;
}