#pragma once
#include "SomePatch.h"
#include "Point.h"
#include "PointsLine.h"

class BezierPatchC0 : public SomePatch
{
public:
	BezierPatchC0();
	virtual void Draw() override;
	void virtual CleanUp() override;
	void virtual Initialize(Program* _program) override;
	void virtual RecalcFigure() override;
	int n = 1, m = 1, splitA = 1, splitB = 1;
	float width = 1.0f, length = 1.0f;
private:
	void AddPatch(int i, int j, float t, float t2, float from, float to, int splits, int& ii);
	void GeneratePoints();
	void ClearPoints();
	bool virtual Create() override;
	int splitAold = 1, splitBold = 1, nOld, mOld;
	float widthOld, lengthOld;
};

