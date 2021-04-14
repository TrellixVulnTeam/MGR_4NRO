#pragma once
#include "SomeCurve.h"
#include "Point.h"
#include "PointsLine.h"

class BezierCurveC0 : public SomeCurve
{
public:
	BezierCurveC0();
	virtual void Draw() override;
	void AddPoint(Point* point);
	void CleanUp();
	void virtual Initialize(Program* _program) override;
private:
	bool virtual Create() override;
};

