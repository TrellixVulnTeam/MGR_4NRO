#pragma once
#include "Figure.h"
#include "Point.h"
#include "GregoryPatch.h"
class Hole :Figure
{
public:
	Hole();
	Hole(std::vector<std::vector<std::vector<Point*>>> _hole);
	void Recalc();
	bool virtual GetGuiInternal(Figure* par) override;
	void virtual Initialize(Program* _program) override;
	virtual void Draw() override;
	void virtual RecalcFigure() override {};
	void Delete() { toDelete = true; }
	void virtual CleanUp() override;
private:
	Program* program;
	std::vector<std::vector<std::vector<Point*>>> hole;
	GregoryPatch* gp1;
	GregoryPatch* gp2;
	GregoryPatch* gp3;
	bool shouldRecalc = false;
	bool toDelete = false;
	void ClearPoints();
};