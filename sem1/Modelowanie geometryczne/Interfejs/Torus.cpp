#include "Torus.h"
#include "imgui\imgui.h"
#include <string>

Torus::Torus() : Figure()
{
	sprintf_s(name, STRMAX, ("Torus - " + std::to_string(idx++) + " " + gen_random(10, idx)).c_str());
	figureType = FigureType::Torus;
}

void Torus::Initialize(Program* _program)
{
	Figure::Initialize(_program);
}

bool Torus::GetGuiInternal(Figure* par)
{
	bool b = false;

	if (ImGui::TreeNode("Size"))
	{
		ImGui::SliderInt("n", &n_new, 5, 50);
		ImGui::SliderInt("m", &m_new, 5, 50);
		ImGui::SliderFloat("r", &r_new, 0.1f, 5.0f);
		ImGui::SliderFloat("R", &R_new, 0.3f, 10.0f);
		ImGui::TreePop();
	}
	return b;
}

void Torus::Draw()
{
	Figure::Draw();
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glDrawElements(GL_LINES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

glm::vec3 Torus::GetParametrizedPos(float u, float v)
{
	if (u == 1.0f) u = 0.0f;
	if (v == 1.0f) v = 0.0f;
	int n = n_new;
	int m = m_new;
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
	int n = n_new;
	int m = m_new;
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
		x = -sina * (r * cosb + R);
		y = 0;
		z = cosa * (r * cosb + R);
	}
	else
	{
		x = -r * cosa * sinb;
		y = r * cosb;
		z = -r * sina * sinb;
	}
	return glm::vec3(x, y, z);
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
	for (int i = 0; i < n * m * 6; ++i) vertices.push_back(0.0f);
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
			int idx1 = idx * 6;
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
			vertices[idx1 + 3] = selected ? 0.0f : 1.0f;
			vertices[idx1 + 4] = selected ? 1.0f : 1.0f;
			vertices[idx1 + 5] = selected ? 0.0f : 1.0f;

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

