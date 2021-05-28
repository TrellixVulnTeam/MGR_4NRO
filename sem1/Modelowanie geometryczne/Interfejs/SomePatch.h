#pragma once
#include "Figure.h"
#include "Point.h"
#include "PointsLines.h"

class SomePatch : public Figure
{
public:
	SomePatch();
	bool virtual GetGuiInternal(Figure* par) final;
	virtual void Draw() override = 0;
	void virtual Recalc() final { first = true; }
	void virtual Initialize(Program* _program) override
	{
		Figure::Initialize(_program);
		if (figureType == FigureType::BezierPatchC2)
			shader = Shader(program->patchShaderDeBoor);
		else
			shader = Shader(program->patchShader);
	};
	int n = 1, m = 1, splitA = 1, splitB = 1;
	std::vector<Point*> points;
protected:
	bool virtual Create() override { return false; };
	bool first = true;
	bool drawLine = true;
	PointsLines* pointsLines;
	int splitAold = 1, splitBold = 1;
	float width = 1.0f, length = 1.0f, r = 1.0f;
	bool cylinder = false;
};

