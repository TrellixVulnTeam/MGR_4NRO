#include"Math.h"
float DeCasteljau(std::vector<float> coeffs, float t)
{
	std::vector<float> coeffs_t(coeffs);

	for (int i = coeffs.size(); i > 0; --i)
	{
		for (int j = 0; j < i - 1; ++j)
		{
			coeffs_t[j] = (1 - t) * coeffs_t[j] + t * coeffs_t[j + 1];
		}
	}
	return coeffs_t[0];
}

glm::ivec3 GetScreenPos(Program* program, glm::vec4 pos)
{
	glm::ivec3 _posScreen;
	glm::vec4 posScreen = program->cam->GetProjectionMatrix() * program->cam->GetViewportMatrix() * pos;
	posScreen /= posScreen.w;
	posScreen = (posScreen + glm::vec4(1.0f)) / 2.0f;
	_posScreen.x = posScreen.x * program->current_width;
	_posScreen.y = posScreen.y * program->current_height;
	_posScreen.z = posScreen.z;
	return _posScreen;
}

void DeBoorToBernstein(float a, float b, float c, float d, glm::vec4& bernsteins)
{
	float p1 = a + 2.0f / 3.0f * (b - a);
	float p2 = b + 1.0f / 3.0f * (c - b);
	float p3 = b + 2.0f / 3.0f * (c - b);
	float p4 = c + 1.0f / 3.0f * (d - c);

	bernsteins.x = (p1 + p2) / 2.0f;
	bernsteins.y = p2;
	bernsteins.z = p3;
	bernsteins.w = (p3 + p4) / 2.0f;
}
