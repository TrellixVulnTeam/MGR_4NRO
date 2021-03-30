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