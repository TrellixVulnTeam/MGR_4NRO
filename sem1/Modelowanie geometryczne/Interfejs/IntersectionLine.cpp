#include "IntersectionLine.h"
#include "Math.h"
#include "imgui\imgui.h"
#include <string>
#include "Intersections.h"

IntersectionLine::IntersectionLine() : SomeCurve()
{
	sprintf_s(name, STRMAX, ("IntersectionLine - " + std::to_string(idx++) + " " + gen_random(10, idx)).c_str());
	figureType = FigureType::IntersectionLine;
}

void IntersectionLine::Initialize(Program* _program)
{
	SomeCurve::Initialize(_program);
	shader = Shader(program->shader);
}

void IntersectionLine::ReplaceInParent(Point* oldPoint, Point* newPoint)
{
	for (int i = 0; i < points.size(); ++i)
	{
		if (points[i] == oldPoint)
		{
			points[i] = newPoint;
			Recalc();
			newPoint->AddParent(this);
		}
	}
}

void IntersectionLine::ConvertToInterpolation()
{
	InterpolationCurveC2* ic = new InterpolationCurveC2();
	ic->Initialize(program);

	for (int i = 0; i < points.size(); ++i)
	{
		points[i]->AddParent(ic);
		ic->AddPoint(points[i]);
	}

	program->figures.push_back(ic);
}

void IntersectionLine::Draw()
{
	Figure::Draw();
	glDrawElements(GL_LINES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void IntersectionLine::AddPoint(Point* point)
{
	points.push_back(point);
	first = true;
}

void IntersectionLine::RemovePoint(int to_del)
{
	points.erase(points.begin() + to_del);
	first = true;
}

void IntersectionLine::CleanUp()
{
	for (int i = 0; i < points.size(); ++i)
		points[i]->Unpin(this);
	delete pointsLine;
}

bool IntersectionLine::Create()
{
	if (first) {
		vertices.clear();
		indices.clear();
		if (points.size() < 2) return false;

		for (int i = 0; i < 6 * points.size(); ++i)vertices.push_back(0.0f);

		for (int i = 0; i < points.size(); i++)
		{
			auto pos = points[i]->GetPos();
			vertices[i * 6] = pos.x;
			vertices[i * 6 + 1] = pos.y;
			vertices[i * 6 + 2] = pos.z;
			vertices[i * 6 + 3] = 0.0f;
			vertices[i * 6 + 4] = 0.0f;
			vertices[i * 6 + 5] = 1.0f;
		}
		for (int i = 1; i < points.size(); ++i)
		{
			indices.push_back(i - 1);
			indices.push_back(i);
		}
		return true;
	}
	return false;
}

