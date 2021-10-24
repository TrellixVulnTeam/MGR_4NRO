#pragma once
#include "Figure.h"
#include "Point.h"
class PointsLines : public Figure
{
public:
	PointsLines();
	bool virtual GetGuiInternal(std::shared_ptr<Figure> par) override { return false; };
	virtual void Draw() override;
	void AddPoint(std::shared_ptr<Point> point);
	void RemoveAt(int i);
	void Clear();
	void Recalc() { Create(); };
	void virtual Initialize(std::shared_ptr<Program> _program) override;
	std::vector<std::shared_ptr<Point>> points;
private:
	bool virtual Create() override;
};

