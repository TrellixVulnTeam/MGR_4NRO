#pragma once
#include "Figure.h"
class Cursor : public Figure
{
public:
	Cursor();
	void ForceRecalcScreenPos();
	bool virtual GetGuiInternal(std::shared_ptr<Figure> par) override;
	virtual void Draw() override;
	void virtual Initialize(std::shared_ptr<Program> _program) override;
private:
	bool virtual Create() override;
	bool firstTime = true;
	glm::vec3 pos;
	glm::vec3 posOld;
	glm::ivec3 posScreen;
	glm::ivec3 posScreenOld;
};

