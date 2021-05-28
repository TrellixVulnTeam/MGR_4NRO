#pragma once
#include "Figure.h"
#include "Point.h"
#include "PointsLine.h"

class SomeCurve : public Figure
{
public:
	SomeCurve();
	bool virtual GetGuiInternal(Figure* par) final;
	virtual void Draw() override = 0;
	void virtual AddPoint(Point* point) = 0;
	void virtual RemovePoint(int to_del) = 0;
	void virtual Recalc() final { first = true; }
	void virtual Initialize(Program* _program) override { Figure::Initialize(_program); shader = Shader(program->bezierShader);};
	std::vector<Point*> points;
protected:
	bool virtual Create() override { return false; };
	bool first = true;
	bool drawLine = true;
	PointsLine* pointsLine;
};

