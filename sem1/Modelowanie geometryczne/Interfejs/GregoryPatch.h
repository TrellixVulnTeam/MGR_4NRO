#pragma once
#include "SomePatch.h"
#include "Point.h"
#include "PointsLine.h"

class GregoryPatch : public Figure
{
public:
	GregoryPatch();
	virtual void Draw() override;
	void virtual CleanUp() override;
	void virtual Recalc() final { first = true; }
	bool virtual GetGuiInternal(Figure* par) override;
	void virtual Initialize(Program* _program) override;
	void ReplaceInParent(Point* oldPoint, Point* newPoint);
private:
	bool first = true;
	int splitA = 4, splitB = 4;
	std::vector<Point*> points;
	PointsLines* pointsLines;
	void GeneratePoints();
	void ClearPoints();
	bool virtual Create() override;
};

