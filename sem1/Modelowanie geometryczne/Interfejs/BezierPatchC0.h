#pragma once
#include "SomePatch.h"
#include "Point.h"
#include "PointsLine.h"

class BezierPatchC0 : public SomePatch
{
public:
	BezierPatchC0();
	BezierPatchC0(int _n, int _m, float _width, float _length);
	virtual void Draw() override;
	void CleanUp();
	void virtual Initialize(Program* _program) override;
private:
	bool virtual Create() override;
	int n, m;
	float width, length;
};

