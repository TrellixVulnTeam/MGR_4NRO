#pragma once
#include "Figure.h"
#include "Point.h"
#include "PointsLine.h"

class SomeCurve : public Figure
{
public:
	SomeCurve();
	bool virtual GetGuiInternal(std::shared_ptr<Figure> par) final;
	virtual void Draw() override = 0;
	void virtual AddPoint(std::shared_ptr<Point> point) = 0;
	void virtual RemovePoint(int to_del) = 0;
	void virtual Recalc() final { first = true; }
	void virtual Initialize(std::shared_ptr<Program> _program) override { Figure::Initialize(_program); shader = program->bezierShader; };
	std::vector<std::shared_ptr<Point>> points;
	bool drawLine = false;
	void virtual ReplaceInParent(std::shared_ptr<Point> oldPoint, std::shared_ptr<Point> newPoint) = 0;
protected:
	bool virtual Create() override { return false; };
	bool first = true;
	PointsLine* pointsLine;
};

