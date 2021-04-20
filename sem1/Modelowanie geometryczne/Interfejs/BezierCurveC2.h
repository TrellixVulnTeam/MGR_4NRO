#pragma once
#include "SomeCurve.h"
#include "Point.h"
#include "PointsLine.h"

class BezierCurveC2 : public SomeCurve
{
public:
	BezierCurveC2();
	virtual void Draw() override;
	virtual void AddPoint(Point* point) override;
	virtual void RemovePoint(int to_del) override;
	void CleanUp();
	void virtual Initialize(Program* _program) override;
	std::vector<Point*> bernsteinPoints;
	void BernsteinMoved(int j);
	bool drawDeBoorLine = true;
private:
	bool virtual Create() override;
	PointsLine* deBoorLine;
};

