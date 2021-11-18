#include "Torus.h"
#include "imgui\imgui.h"
#include <string>
#include "Intersections.h"

Torus::Torus() : Figure()
{
	sprintf_s(name, STRMAX, ("Torus - " + std::to_string(idx++) + " " + gen_random(10, idx)).c_str());
	figureType = FigureType::Torus;
}

void Torus::Initialize(Program* _program)
{
	Figure::Initialize(_program);

	shader = Shader(program->trimShader);
	FillImage(trimTex, program);
	FillImage(trimLine, program);
}

void Torus::RecalcFigure()
{
	if (Create()) {
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


bool Torus::GetGuiInternal(Figure* par)
{
	bool b = false;

	if (ImGui::TreeNode("Size"))
	{
		ImGui::SliderInt("n", &n_new, 5, 100);
		ImGui::SliderInt("m", &m_new, 5, 100);
		ImGui::SliderFloat("r", &r_new, 0.1f, 5.0f);
		ImGui::SliderFloat("R", &R_new, 0.3f, 10.0f);
		ImGui::TreePop();
	}
	if (ImGui::Button("Inverse"))
	{
		SetTexture(trimTex, trimTex, false, true, wrapParametrizationFloodFill, program);
	}
	return b;
}

void Torus::Draw()
{
	Figure::Draw();

	auto texLocation = glGetUniformLocation(shader.ID, "trimTexture");

	glUniform1i(texLocation, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, trimTex);

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glDrawElements(GL_LINES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

glm::vec3 Torus::GetParametrizedPos(float u, float v)
{
	if (u == 1.0f) u = 0.0f;
	if (v == 1.0f) v = 0.0f;
	float r = r_new;
	float R = R_new;

	float alpha = u * 2.0f * M_PI;
	float beta = v * 2.0f * M_PI;

	float cosa = cos(alpha);
	float sina = sin(alpha);

	float cosb = cos(beta);
	float sinb = sin(beta);

	float x = cosa * (r * cosb + R);
	float y = r * sinb;
	float z = sina * (r * cosb + R);

	return glm::vec3(model * glm::vec4(x, y, z, 1.0f));
}

glm::vec3 Torus::GetParametrizedDer(float u, float v, bool du)
{
	if (u == 1.0f) u = 0.0f;
	if (v == 1.0f) v = 0.0f;
	float r = r_new;
	float R = R_new;

	float alpha = u * 2.0f * M_PI;
	float beta = v * 2.0f * M_PI;

	float cosa = cos(alpha);
	float sina = sin(alpha);

	float cosb = cos(beta);
	float sinb = sin(beta);

	float x;
	float y;
	float z;

	if (du)
	{
		x = -sina * (r * cosb + R) * 2.0f * M_PI;
		y = 0;
		z = cosa * (r * cosb + R) * 2.0f * M_PI;
	}
	else
	{
		x = -r * cosa * sinb * 2.0f * M_PI;
		y = r * cosb * 2.0f * M_PI;
		z = -r * sina * sinb * 2.0f * M_PI;
	}
	return  glm::vec3(model * glm::vec4(x, y, z, 0.0f));
}

bool Torus::Create()
{
	bool fCreate = Figure::Create();
	if (m_new == m_old && n_new == n_old &&
		r_new == r_old && R_new == R_old && !fCreate) return false;

	if (m_new < 3 || n_new < 3 || r_new <= 0 || R_new <= 0) return false;

	m_old = m_new;
	n_old = n_new;
	r_old = r_new;
	R_old = R_new;

	int n = n_new;
	int m = m_new;
	float r = r_new;
	float R = R_new;

	vertices.clear();
	for (int i = 0; i < n * m * 8; ++i) vertices.push_back(0.0f);
	indices.clear();
	for (int i = 0; i < 4 * n * m; ++i)indices.push_back(0);

	for (int i = 0; i < m; ++i)
	{
		float alpha = i * (2 * M_PI) / m;
		float cosa = cos(alpha);
		float sina = sin(alpha);
		for (int j = 0; j < n; ++j)
		{
			int idx = i * n + j;
			int idx1 = idx * 8;
			int idx2 = idx * 4;
			float beta = j * (2 * M_PI) / n;
			float cosb = cos(beta);
			float sinb = sin(beta);

			float x = cosa * (r * cosb + R);
			float y = r * sinb;
			float z = sina * (r * cosb + R);

			vertices[idx1] = x;
			vertices[idx1 + 1] = y;
			vertices[idx1 + 2] = z;
			vertices[idx1 + 3] = selected ? 0.0f : 0.7f;
			vertices[idx1 + 4] = selected ? 1.0f : 0.7f;
			vertices[idx1 + 5] = selected ? 0.0f : 0.7f;
			vertices[idx1 + 6] = ((float)i) / m;
			vertices[idx1 + 7] = ((float)j) / n;

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

