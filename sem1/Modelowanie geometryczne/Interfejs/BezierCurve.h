#pragma once
#include "Figure.h"
#include "Point.h"
#include "PointsLine.h"

class BezierCurve : public Figure
{
public:
	BezierCurve();
	bool virtual GetGuiInternal(Figure* par) override;
	virtual void Draw() override;
	void AddPoint(Point* point);
	void CleanUp();
	void Recalc() { first = true; }
	void virtual Initialize(Program* _program) override;
private:
	bool virtual Create() override;
	std::vector<Point*> points;
	bool first = true;
	bool drawLine = true;
	PointsLine* pointsLine;
};

