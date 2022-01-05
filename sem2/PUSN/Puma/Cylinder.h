#pragma once
#include "Figure.h"


class Cylinder : public Figure
{
public:
	Cylinder(bool mid_,float r_,float h_,glm::vec3 col_);
	bool virtual GetGuiInternal(std::shared_ptr<Figure> par) override;
	virtual void Draw() override;
	void virtual Initialize(std::shared_ptr<Program> _program) override;
	void virtual RecalcFigure() override;
	bool onX = false, onZ = false;
	float r, h,mid;
private:
	bool virtual Create() override;
	glm::vec3 col;
	int n = 50;
};

