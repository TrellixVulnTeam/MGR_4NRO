#include "Torus.h"
#include "imgui\imgui.h"
#include <string>

Torus::Torus(Shader _shader) : Figure(_shader)
{
	name = std::string("Torus");
}

bool Torus::GetGui(int i)
{
	bool b = false;

	b = Figure::GetGui(i);
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

void Torus::Draw(int transLoc)
{
	Figure::Draw(transLoc);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glDrawElements(GL_LINES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
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
			float y = sina * (r * cosb + R);
			float z = r * sinb;

			vertices[idx1] = x;
			vertices[idx1 + 1] = y;
			vertices[idx1 + 2] = z;
			vertices[idx1 + 3] = selected ? 0.0f : 1.0f;
			vertices[idx1 + 4] = selected ? 1.0f : 0.0f;
			vertices[idx1 + 5] = 0.0f;

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

