#pragma once
#include "SomeCurve.h"
#include "Point.h"
#include "PointsLine.h"

class InterpolationCurveC2 : public SomeCurve
{
public:
	InterpolationCurveC2();
	virtual void Draw() override;
	virtual void AddPoint(Point* point) override;
	virtual void RemovePoint(int to_del) override;
	void CleanUp();
	void virtual Initialize(Program* _program) override;
	std::vector<Point*> bernsteinPoints;
	bool drawBernsteinLine;
	bool drawBernsteinPoints;
	void virtual ReplaceInParent(Point* oldPoint, Point* newPoint) override;
private:
	bool virtual Create() override;
	PointsLine* bernsteinLine;
};

