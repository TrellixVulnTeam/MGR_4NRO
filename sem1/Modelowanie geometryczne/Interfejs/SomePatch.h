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
		if (figureType == FigureType::BezierPatchC2 || figureType == FigureType::BezierPatchC2Cylinder)
			shader = Shader(program->patchShaderDeBoor);
		else
			shader = Shader(program->patchShader);
	};
protected:
	bool virtual Create() override { return false; };
	std::vector<Point*> points;
	bool first = true;
	bool drawLine = true;
	PointsLines* pointsLines;
};

