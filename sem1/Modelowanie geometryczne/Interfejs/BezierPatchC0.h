#pragma once
#include "SomePatch.h"
#include "Point.h"
#include "PointsLine.h"

class BezierPatchC0 : public SomePatch
{
public:
	BezierPatchC0();
	BezierPatchC0(int _n, int _m, float _width, float _length, float _r, bool _cylinder) :BezierPatchC0() { n = _n; m = _m; width = _width; length = _length; r = _r; cylinder = _cylinder; }
	virtual void Draw() override;
	void virtual CleanUp() override;
	void virtual Initialize(Program* _program) override;
	void virtual RecalcFigure() override;
private:
	void AddPatch(int i, int j, float t, float t2, float from, float to, int splits, int& ii);
	void GeneratePoints();
	void ClearPoints();
	bool virtual Create() override;
};

