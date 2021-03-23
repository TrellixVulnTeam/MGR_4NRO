#include "Cursor.h"
#include "imgui\imgui.h"
#include <string>

Cursor::Cursor(Shader _shader) : Figure(_shader)
{
	sprintf_s(name, STRMAX, "Cursor");
	_name = "Cursor";
	figureType = FigureType::Cursor;
	pos = glm::vec3(0.0f, 0.0f, 0.0f);
	posOld = glm::vec3(-1.0f);
}

bool Cursor::GetGuiInternal()
{
	bool b = false;
	if (ImGui::TreeNode("Position"))
	{
		ImGui::SliderFloat("x", &pos.x, -7.0f, 7.0f);
		ImGui::SliderFloat("y", &pos.y, -7.0f, 7.0f);
		ImGui::SliderFloat("z", &pos.z, -7.0f, 7.0f);
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("PositionScreen"))
	{
		ImGui::SliderInt("x", &posScreen.x, 0, cur_width);
		ImGui::SliderInt("y", &posScreen.y, 0, cur_height);
		ImGui::TreePop();
	}


	if (pos != posOld)
	{
		posOld = pos;
		translation[3][0] = pos.x;
		translation[3][1] = pos.y;
		translation[3][2] = pos.z;
		glm::vec4 _posScreen = persp * view * translation * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		_posScreen /= _posScreen.w;
		_posScreen = (_posScreen + glm::vec4(1.0f)) / 2.0f;
		posScreen.x = _posScreen.x * cur_width;
		posScreen.y = _posScreen.y * cur_height;
		posScreen.z = _posScreen.z;
		posScreenOld = posScreen;
		RecalcModel();
	}

	if (posScreen != posScreenOld)
	{
		posScreenOld = posScreen;

		glm::vec4 lRayStart_NDC(
			((float)posScreen.x / (float)cur_width - 0.5f) * 2.0f,
			((float)posScreen.y / (float)cur_height - 0.5f) * 2.0f,
			-1.0,
			1.0f
		);
		glm::vec4 lRayEnd_NDC(
			((float)posScreen.x / (float)cur_width - 0.5f) * 2.0f,
			((float)posScreen.y / (float)cur_height - 0.5f) * 2.0f,
			0.0,
			1.0f
		);

		glm::vec4 lRayStart_camera = inv_persp * lRayStart_NDC;    lRayStart_camera /= lRayStart_camera.w;
		glm::vec4 lRayStart_world = inv_view * lRayStart_camera; lRayStart_world /= lRayStart_world.w;
		glm::vec4 lRayEnd_camera = inv_persp * lRayEnd_NDC;      lRayEnd_camera /= lRayEnd_camera.w;
		glm::vec4 lRayEnd_world = inv_view * lRayEnd_camera;   lRayEnd_world /= lRayEnd_world.w;
		glm::vec3 lRayDir_world(lRayEnd_world - lRayStart_world);

		float coef = (pos.z - lRayStart_world.z) / lRayDir_world.z;

		pos.x = lRayStart_world.x + coef * lRayDir_world.x;
		pos.y = lRayStart_world.y + coef * lRayDir_world.y;
		translation[3][0] = pos.x;
		translation[3][1] = pos.y;
		posOld = pos;
		RecalcModel();


	}
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

