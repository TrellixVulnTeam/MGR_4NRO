#pragma once
#include "Figure.h"
#include "Point.h"
#include "PointsLine.h"

class BezierCurve : public Figure
{
public:
	BezierCurve(Shader _shader);
	bool virtual GetGuiInternal(bool fromMainGui) override;
	virtual void Draw(int transLoc) override;
	void AddPoint(Point* point);
	void CleanUp();
	void Recalc() { first = true; }
private:
	bool virtual Create() override;
	std::vector<Point*> points;
	bool first = true;
	bool drawLine = true;
	PointsLine* pointsLine;
};

