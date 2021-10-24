#pragma once
#include "Figure.h"
class MiddlePoint : public Figure
{
public:
	MiddlePoint();
	bool virtual GetGuiInternal(std::shared_ptr<Figure> par) override;
	void Reset();
	void Add(std::shared_ptr<Figure> figure);
	virtual void Draw() override;
	void virtual Initialize(std::shared_ptr<Program> _program) override;
private:
	bool virtual Create() override;
	bool firstTime = true;
	int count = 0;
	glm::vec3 pos;
};

