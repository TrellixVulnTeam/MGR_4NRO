#pragma once
#include "SomePatch.h"
#include "Point.h"
#include "PointsLine.h"
#include "Hole.h"
#include "Math.h"

class BezierPatchC0;
struct SinglePatch
{
	BezierPatchC0* bezier;
	std::vector<std::vector<Point*>> patch;
};

class BezierPatchC0 : public SomePatch
{
public:
	BezierPatchC0();
	BezierPatchC0(int _n, int _m) :BezierPatchC0() { n = _n; m = _m; generate = false; }
	BezierPatchC0(int _n, int _m, float _width, float _length, float _r, bool _cylinder) :BezierPatchC0() { n = _n; m = _m; width = _width; length = _length; r = _r; cylinder = _cylinder; }
	virtual void Draw() override;
	void virtual CleanUp() override;
	void virtual Initialize(Program* _program) override;
	void virtual RecalcFigure() override;
	void virtual ReplaceInParent(Point* oldPoint, Point* newPoint) override;
	std::vector<SinglePatch> GetAllPatches();
	void AddHole(Hole* h) { holes.push_back(h); }
	std::vector<Hole*> holes;
	glm::vec3 GetParametrizedPos(float u, float v);
	glm::vec3 GetParametrizedDer(float u, float v, bool du);
private:
	void AddPatch(int i, int j, float t, float t2, float from, float to, int splits, int& ii, float u_start, float v_start, float u_size, float v_size);
	void GeneratePoints();
	void ClearPoints();
	bool virtual Create() override;
};

