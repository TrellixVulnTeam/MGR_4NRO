#pragma once
#include "Figure.h"


class Cursor : public Figure
{
public:
	Cursor(int a_ = 0);
	bool virtual GetGuiInternal(std::shared_ptr<Figure> par) override;
	virtual void Draw() override;
	void virtual Initialize(std::shared_ptr<Program> _program) override;
	void virtual RecalcFigure() override;
private:
	bool virtual Create() override;
	int a;
};

