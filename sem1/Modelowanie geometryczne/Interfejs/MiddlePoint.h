#pragma once
#include "Figure.h"
class MiddlePoint : public Figure
{
public:
	MiddlePoint(Shader _shader);
	bool virtual GetGui(int i) override;
	void Reset();
	void Add(Figure* figure);
	virtual void Draw(int transLoc) override;
private:
	bool virtual Create() override;
	bool firstTime = true;
	int count = 0;
	glm::vec3 pos;
};
