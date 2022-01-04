#pragma once
#include "Figure.h"
#include "Cylinder.h"


class Puma : public Figure
{
public:
	Puma();
	bool virtual GetGuiInternal(std::shared_ptr<Figure> par) override;
	virtual void Draw() override;
	void virtual Initialize(std::shared_ptr<Program> _program) override;
	void virtual RecalcFigure() override;
private:
	bool virtual Create() override;
	std::vector<std::shared_ptr<Cylinder>> cylinders;
};

