#pragma once
#include "SomeCurve.h"
#include "Point.h"
#include "PointsLine.h"

class BezierCurveC0 : public SomeCurve
{
public:
	BezierCurveC0();
	virtual void Draw() override;
	virtual void AddPoint(Point* point) override;
	virtual void RemovePoint(int to_del) override;
	void virtual CleanUp() override;
	void virtual Initialize(Program* _program) override;
	void virtual ReplaceInParent(Point* oldPoint, Point* newPoint) override;
private:
	bool virtual Create() override;
};

