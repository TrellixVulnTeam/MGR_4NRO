#include "BezierCurveC0.h"
#include "imgui\imgui.h"
#include <string>

BezierCurveC0::BezierCurveC0() : SomeCurve()
{
	sprintf_s(name, STRMAX, "BezierCurveC0");
	_name = "BezierCurveC0";
	figureType = FigureType::BezierCurveC0;
}

void BezierCurveC0::Initialize(Program* _program)
{
	SomeCurve::Initialize(_program);
	pointsLine->Initialize(program);
}

void BezierCurveC0::Draw()
{
	Figure::Draw();
	glDrawElements(GL_LINES_ADJACENCY, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	if (drawLine)
		pointsLine->Draw();
}

void BezierCurveC0::AddPoint(Point* point)
{
	points.push_back(point);
	first = true;
	pointsLine->AddPoint(point);
}

void BezierCurveC0::RemovePoint(int to_del)
{
	pointsLine->RemoveAt(to_del);
	points.erase(points.begin() + to_del);
	first = true;
}

void BezierCurveC0::CleanUp()
{
	for (int i = 0; i < points.size(); ++i)
		points[i]->Unpin(this);
	delete pointsLine;
}

bool BezierCurveC0::Create()
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
	for (int i = 0; i < points.size()-1; i += 3)
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

