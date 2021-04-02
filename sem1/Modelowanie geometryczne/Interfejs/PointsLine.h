
#pragma once
#include "Figure.h"
#include "Point.h"
class PointsLine : public Figure
{
public:
	PointsLine();
	bool virtual GetGuiInternal(bool fromMainGui) override { return false; };
	virtual void Draw() override;
	void AddPoint(Point* point);
	void RemoveAt(int i);
	void Recalc() { Create(); };
	void virtual Initialize(Program* _program) override;
private:
	bool virtual Create() override;
	std::vector<Point*> points;
};

