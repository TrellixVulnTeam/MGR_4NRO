#pragma once
#include "Figure.h"
#include "Point.h"
class PointsLines : public Figure
{
public:
	PointsLines();
	bool virtual GetGuiInternal(Figure* par) override { return false; };
	virtual void Draw() override;
	void AddPoint(Point* point);
	void RemoveAt(int i);
	void Clear();
	void Recalc() { Create(); };
	void virtual Initialize(Program* _program) override;
	std::vector<Point*> points;
private:
	bool virtual Create() override;
};

