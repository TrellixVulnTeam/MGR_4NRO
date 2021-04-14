#include "BezierCurveC2.h"
#include "imgui\imgui.h"
#include <string>

BezierCurveC2::BezierCurveC2() : SomeCurve()
{
	sprintf_s(name, STRMAX, "BezierCurveC2");
	_name = "BezierCurveC2";
	figureType = FigureType::BezierCurveC2;
}

void BezierCurveC2::Initialize(Program* _program)
{
	SomeCurve::Initialize(_program);
	pointsLine->Initialize(program);
}

void BezierCurveC2::Draw()
{
	Figure::Draw();
	glDrawElements(GL_LINES_ADJACENCY, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	if (drawLine)
		pointsLine->Draw();
}

void BezierCurveC2::AddPoint(Point* point)
{
	points.push_back(point);
	first = true;
	pointsLine->AddPoint(point);
}

void BezierCurveC2::CleanUp()
{
	for (int i = 0; i < points.size(); ++i)
		points[i]->Unpin(this);
	delete pointsLine;
}

bool BezierCurveC2::Create()
{
	bool fCreate = Figure::Create();
	if (!fCreate && !first) return false;
	first = false;
	pointsLine->RecalcFigure();
	vertices.clear();
	indices.clear();
	if (points.size() < 2) return false;

	int total_n = 0;

	int k = 0;
	for (int i = 0; i < points.size() - 1; i += 3)
	{
		for (int j = i; j < i + 4 && j < points.size(); ++j)
		{
			auto pos = points[j]->GetPos();
			vertices.push_back(pos.x);
			vertices.push_back(pos.y);
			vertices.push_back(pos.z);
			vertices.push_back(0.0f);
			vertices.push_back(0.0f);
			vertices.push_back(1.0f);
			indices.push_back(k);
			++k;
		}
	}
	while (indices.size() % 4 != 0)
	{
		vertices.push_back(0.0f);
		vertices.push_back(0.0f);
		vertices.push_back(0.0f);
		vertices.push_back(-1.0f);
		vertices.push_back(-1.0f);
		vertices.push_back(-1.0f);
		indices.push_back(k);
		++k;
	}
	return true;


}

