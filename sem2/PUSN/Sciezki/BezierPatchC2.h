#pragma once
#include "SomePatch.h"
#include "Point.h"
#include "PointsLine.h"
#include "Math.h"

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
	void virtual ReplaceInParent(Point* oldPoint, Point* newPoint) override;
	glm::vec3 GetParametrizedPos(float u, float v);
	glm::vec3 GetParametrizedDer(float u, float v, bool du);
private:
	void AddPatch(int i, int j, float t, float t2, float from, float to, int splits, int& ii, float u_start, float v_start, float u_size, float v_size);	
	void GeneratePoints();
	void ClearPoints();
	bool virtual Create() override;
};

