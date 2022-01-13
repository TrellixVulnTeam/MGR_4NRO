#include"Math.h"


glm::ivec3 GetScreenPos(std::shared_ptr<Program> program, glm::vec4 pos)
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