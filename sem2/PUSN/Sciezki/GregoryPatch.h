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
	void UpdateMesh(std::vector<glm::vec3> positions);
	std::vector<Point*> points;
private:
	bool first = true;
	bool drawPoints = false, draw = true, drawLine = false;
	int splitA = 4, splitB = 4;
	PointsLines* pointsLines;
	void GeneratePoints();
	void ClearPoints();
	bool virtual Create() override;
};

