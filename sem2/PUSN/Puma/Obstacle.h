#pragma once
#include "Figure.h"


class Obstacle : public Figure
{
public:
	Obstacle();
	bool virtual GetGuiInternal(std::shared_ptr<Figure> par) override;
	virtual void Draw() override;
	void virtual Initialize(std::shared_ptr<Program> _program) override;
	void virtual RecalcFigure() override;
	glm::vec2 GetPoint(int n);
	float x = 0.0f, y = 0.0f;
	float width = 0.2f, height = 0.2f;
private:
	bool virtual Create() override;
	float xPos = -1.0f, yPos = -1.0f;
	float w = -1.0f, h = -1.0f;
};

