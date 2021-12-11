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
	void InverseKinematics(glm::vec2 pos);
	glm::vec2 GetPoint(int n, float alpha, float beta);
	float l1 = 0.5f, l2 = 0.5f;
	double alpha = 90.0f, beta = 90.0f;
private:
	bool virtual Create() override;
	float len1 = -1.0f, len2 = -1.0f;
	double a = -1.0f, b = -1.0f;
};

