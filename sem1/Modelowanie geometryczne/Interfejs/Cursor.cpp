#include "Cursor.h"
#include "imgui\imgui.h"
#include <string>

Cursor::Cursor(Shader _shader) : Figure(_shader)
{
	strncpy_s(_name, "Cursor", STRMAX);
	figureType = FigureType::Cursor;
}

bool Cursor::GetGui(int i)
{
	bool b = false;
	if (ImGui::TreeNode("Cursor"))
	{
		if (ImGui::TreeNode("Position"))
		{
			ImGui::SliderFloat("x", &translation[3][0], -7.0f, 7.0f);
			ImGui::SliderFloat("y", &translation[3][1], -7.0f, 7.0f);
			ImGui::SliderFloat("z", &translation[3][2], -7.0f, 7.0f);
			ImGui::TreePop();
		}
		ImGui::TreePop();
	}

	RecalcModel();

	return b;
}

void Cursor::Draw(int transLoc)
{
	Figure::Draw(transLoc);
	glPointSize(2.0f);
	glDrawElements(GL_LINES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

bool Cursor::Create()
{
	bool fCreate = Figure::Create();
	if (firstTime || fCreate)
	{
		vertices.clear();
		for (int i = 0; i < 6 * 6; ++i) vertices.push_back(0.0f);

		vertices[0] = -0.1f;
		vertices[1] = 0.0f;
		vertices[2] = 0.0f;
		vertices[3] = 1.0f;
		vertices[4] = 0.0f;
		vertices[5] = 0.0f;

		vertices[6] = 0.1f;
		vertices[7] = 0.0f;
		vertices[8] = 0.0f;
		vertices[9] = 1.0f;
		vertices[10] = 0.0f;
		vertices[11] = 0.0f;

		vertices[12] = 0.0f;
		vertices[13] = -0.1f;
		vertices[14] = 0.0f;
		vertices[15] = 0.0f;
		vertices[16] = 1.0f;
		vertices[17] = 0.0f;

		vertices[18] = 0.0f;
		vertices[19] = 0.1f;
		vertices[20] = 0.0f;
		vertices[21] = 0.0f;
		vertices[22] = 1.0f;
		vertices[23] = 0.0f;

		vertices[24] = 0.0f;
		vertices[25] = 0.0f;
		vertices[26] = -0.1f;
		vertices[27] = 1.0f;
		vertices[28] = 1.0f;
		vertices[29] = 0.0f;

		vertices[30] = 0.0f;
		vertices[31] = 0.0f;
		vertices[32] = 0.1f;
		vertices[33] = 1.0f;
		vertices[34] = 1.0f;
		vertices[35] = 0.0f;

		indices.clear();
		indices.push_back(0);
		indices.push_back(1);
		indices.push_back(2);
		indices.push_back(3);
		indices.push_back(4);
		indices.push_back(5);

		firstTime = false;
		return true;
	}

	return false;

}

