#pragma once
#include "SomeCurve.h"
#include "Point.h"
#include "PointsLine.h"
#include "InterpolationCurveC2.h"
class IntersectionLine : public SomeCurve
{
public:
	IntersectionLine();
	virtual void Draw() override;
	virtual void AddPoint(Point* point) override;
	virtual void RemovePoint(int to_del) override;
	void virtual CleanUp() override;
	void virtual Initialize(Program* _program) override;
	void virtual ReplaceInParent(Point* oldPoint, Point* newPoint) override;
	void ConvertToInterpolation();
private:
	bool virtual Create() override;
};

