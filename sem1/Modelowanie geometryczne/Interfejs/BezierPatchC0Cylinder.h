#pragma once
#include "SomePatch.h"
#include "Point.h"
#include "PointsLine.h"

class BezierPatchC0Cylinder : public SomePatch
{
public:
	BezierPatchC0Cylinder();
	BezierPatchC0Cylinder(int _n, int _m, float _r, float _length);
	virtual void Draw() override;
	void CleanUp();
	void virtual Initialize(Program* _program) override;
private:
	bool virtual Create() override;
	int n, m;
	float r, length;
};

