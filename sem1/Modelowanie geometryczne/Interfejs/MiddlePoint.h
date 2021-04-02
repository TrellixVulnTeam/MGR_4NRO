#pragma once
#include "Figure.h"
class MiddlePoint : public Figure
{
public:
	MiddlePoint();
	bool virtual GetGuiInternal(bool fromMainGui) override;
	void Reset();
	void Add(Figure* figure);
	virtual void Draw() override;
	void virtual Initialize(Program* _program) override;
private:
	bool virtual Create() override;
	bool firstTime = true;
	int count = 0;
	glm::vec3 pos;
};

