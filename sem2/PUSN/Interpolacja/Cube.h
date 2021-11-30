#pragma once
#include "Figure.h"


class Cube : public Figure
{
public:
	Cube();
	bool virtual GetGuiInternal(std::shared_ptr<Figure> par) override;
	virtual void Draw() override;
	void virtual Initialize(std::shared_ptr<Program> _program) override;
	void virtual RecalcFigure() override;
private:
	bool virtual Create() override;

	float drillRadius;
	int zSplit, xSplit;
	float xDiff, zDiff;
	float width, length, height;
};

