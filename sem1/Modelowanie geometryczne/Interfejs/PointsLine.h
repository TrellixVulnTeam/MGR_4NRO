
#pragma once
#include "Figure.h"
#include "Point.h"
class PointsLine : public Figure
{
public:
	PointsLine(Shader _shader);
	bool virtual GetGuiInternal(std::vector<Figure*> figures, bool fromMainGui) override { return false; };
	virtual void Draw(int transLoc) override;
	void AddPoint(Point* point);
	void RemoveAt(int i);
	void Recalc() { Create(); };
private:
	bool virtual Create() override;
	std::vector<Point*> points;
};

