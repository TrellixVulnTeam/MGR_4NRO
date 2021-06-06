#pragma once
#include "SomePatch.h"
#include "Point.h"
#include "PointsLine.h"

class BezierPatchC2 : public SomePatch
{
public:
	BezierPatchC2();
	BezierPatchC2(int _n, int _m) :BezierPatchC2() { n = _n; m = _m; generate = false; }
	BezierPatchC2(int _n, int _m, float _width, float _length, float _r, bool _cylinder) :BezierPatchC2() { n = _n; m = _m; width = _width; length = _length; r = _r; cylinder = _cylinder; }
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

