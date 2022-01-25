#pragma once
#include "Figure.h"


class Tool : public Figure
{
public:
	Tool();
	bool virtual GetGuiInternal(std::shared_ptr<Figure> par) override;
	virtual void Draw() override;
	void virtual Initialize(std::shared_ptr<Program> _program) override;
	void virtual RecalcFigure() override;
	float r = 1.0f, l = 2.0f;
	float angle = M_PI/4;
	float block_size = 0.2f;
private:
	bool virtual Create() override;
	float r_old = -1.0f, l_old = -1.0f;
	float angle_old = 0.0f;
};