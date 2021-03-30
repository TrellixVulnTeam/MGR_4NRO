#pragma once
#include "Figure.h"
class Cursor : public Figure
{
public:
	Cursor(Shader _shader);
	void ForceRecalcScreenPos();
	bool virtual GetGuiInternal(bool fromMainGui) override;
	virtual void Draw(int transLoc) override;
private:
	bool virtual Create() override;
	bool firstTime = true;
	glm::vec3 pos;
	glm::vec3 posOld;
	glm::ivec3 posScreen;
	glm::ivec3 posScreenOld;
};

