#pragma once
#include "Figure.h"
class Cursor : public Figure
{
public:
	Cursor();
	void ForceRecalcScreenPos();
	bool virtual GetGuiInternal(bool fromMainGui) override;
	virtual void Draw() override;
	void virtual Initialize(Program* _program) override;
private:
	bool virtual Create() override;
	bool firstTime = true;
	glm::vec3 pos;
	glm::vec3 posOld;
	glm::ivec3 posScreen;
	glm::ivec3 posScreenOld;
};

