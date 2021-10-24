#include "Cursor.h"
#include "imgui\imgui.h"
#include "Math.h"
#include <string>

Cursor::Cursor() : Figure()
{
	sprintf_s(name, STRMAX, "Cursor");
	figureType = FigureType::Cursor;
	pos = glm::vec3(0.0f, 0.0f, 0.0f);
	posOld = glm::vec3(-1.0f);
}

void Cursor::Initialize(std::shared_ptr<Program> _program)
{
	Figure::Initialize(_program);
}

void Cursor::ForceRecalcScreenPos()
{
	posOld = { -1,-1,-1 };
}

bool Cursor::GetGuiInternal(std::shared_ptr<Figure> par)
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
		ImGui::SliderInt("x", &posScreen.x, 0, program->current_width);
		ImGui::SliderInt("y", &posScreen.y, 0, program->current_height);
		ImGui::TreePop();
	}


	if (pos != posOld)
	{
		posOld = pos;
		translation[3][0] = pos.x;
		translation[3][1] = pos.y;
		translation[3][2] = pos.z;
		posScreen = GetScreenPos(program, translation*glm::vec4(0.0f,0.0f,0.0f,1.0f));
		posScreenOld = posScreen;
		RecalcModel();
	}

	if (posScreen != posScreenOld)
	{
		posScreenOld = posScreen;

		float A = program->cam->front.x;
		float B = program->cam->front.y;
		float C = program->cam->front.z;
		float D = -program->cam->front.x * pos.x
			- program->cam->front.y * pos.y
			- program->cam->front.z * pos.z;

		glm::vec4 lRayStart_NDC(
			((float)posScreen.x / (float)program->current_width - 0.5f) * 2.0f,
			((float)posScreen.y / (float)program->current_height - 0.5f) * 2.0f,
			-1.0,
			1.0f
		);
		glm::vec4 lRayEnd_NDC(
			((float)posScreen.x / (float)program->current_width - 0.5f) * 2.0f,
			((float)posScreen.y / (float)program->current_height - 0.5f) * 2.0f,
			0.0,
			1.0f
		);

		glm::vec4 lRayStart_camera = program->cam->GetInvProjectionMatrix() * lRayStart_NDC;    lRayStart_camera /= lRayStart_camera.w;
		glm::vec4 lRayStart_world = program->cam->GetInvViewportMatrix() * lRayStart_camera; lRayStart_world /= lRayStart_world.w;
		glm::vec4 lRayEnd_camera = program->cam->GetInvProjectionMatrix() * lRayEnd_NDC;      lRayEnd_camera /= lRayEnd_camera.w;
		glm::vec4 lRayEnd_world = program->cam->GetInvViewportMatrix() * lRayEnd_camera;   lRayEnd_world /= lRayEnd_world.w;
		glm::vec3 lRayDir_world(lRayEnd_world - lRayStart_world);
		lRayDir_world = glm::normalize(lRayDir_world);

		float t = (-D - A * lRayStart_world.x - B * lRayStart_world.y - C * lRayStart_world.z) / (A * lRayDir_world.x + B * lRayDir_world.y + C * lRayDir_world.z);

		pos.x = lRayStart_world.x + t * lRayDir_world.x;
		pos.y = lRayStart_world.y + t * lRayDir_world.y;
		pos.z = lRayStart_world.z + t * lRayDir_world.z;

		translation[3][0] = pos.x;
		translation[3][1] = pos.y;
		translation[3][2] = pos.z;
		posOld = pos;
		RecalcModel();


	}
	return b;
}

void Cursor::Draw()
{
	Figure::Draw();
	glPointSize(2.0f);
	glDrawElements(GL_LINES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

bool Cursor::Create()
{
	bool fCreate = Figure::Create();
	if (firstTime || fCreate)
	{
		float size = 20.0f;
		vertices.clear();
		for (int i = 0; i < 6 * 6; ++i) vertices.push_back(0.0f);

		vertices[0] = -size;
		vertices[1] = 0.0f;
		vertices[2] = 0.0f;
		vertices[3] = 1.0f;
		vertices[4] = 0.0f;
		vertices[5] = 0.0f;

		vertices[6] = size;
		vertices[7] = 0.0f;
		vertices[8] = 0.0f;
		vertices[9] = 1.0f;
		vertices[10] = 0.0f;
		vertices[11] = 0.0f;

		vertices[12] = 0.0f;
		vertices[13] = -size;
		vertices[14] = 0.0f;
		vertices[15] = 0.0f;
		vertices[16] = 1.0f;
		vertices[17] = 0.0f;

		vertices[18] = 0.0f;
		vertices[19] = size;
		vertices[20] = 0.0f;
		vertices[21] = 0.0f;
		vertices[22] = 1.0f;
		vertices[23] = 0.0f;

		vertices[24] = 0.0f;
		vertices[25] = 0.0f;
		vertices[26] = -size;
		vertices[27] = 1.0f;
		vertices[28] = 1.0f;
		vertices[29] = 0.0f;

		vertices[30] = 0.0f;
		vertices[31] = 0.0f;
		vertices[32] = size;
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

