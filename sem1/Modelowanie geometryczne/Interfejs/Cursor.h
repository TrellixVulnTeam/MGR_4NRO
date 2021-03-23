#pragma once
#include "Figure.h"
class Cursor : public Figure
{
public:
	Cursor(Shader _shader);
	bool virtual GetGuiInternal() override;
	virtual void Draw(int transLoc) override;
	glm::mat4 persp;
	glm::mat4 view;
	glm::mat4 inv_persp;
	glm::mat4 inv_view;
	int cur_width;
	int cur_height;
private:
	bool virtual Create() override;
	bool firstTime = true;
	glm::vec3 pos;
	glm::vec3 posOld;
	glm::ivec3 posScreen;
	glm::ivec3 posScreenOld;
};

