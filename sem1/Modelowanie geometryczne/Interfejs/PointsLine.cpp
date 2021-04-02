#include "PointsLine.h"
#include <string>

PointsLine::PointsLine() : Figure()
{
	sprintf_s(name, STRMAX, "PointsLine");
	_name = "PointsLine";
	figureType = FigureType::PointsLine;
	canMove = false;
}

void PointsLine::Initialize(Program* _program)
{
	Figure::Initialize(_program);
}

void PointsLine::Draw()
{
	Figure::Draw();
	glDrawElements(GL_LINES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void PointsLine::AddPoint(Point* point)
{
	points.push_back(point);
}

void PointsLine::RemoveAt(int i)
{
	points.erase(points.begin() + i);
}

bool PointsLine::Create()
{
	vertices.clear();
	indices.clear();
	if (points.size() < 2) return false;

	for (int i = 0; i < 6 * points.size();++i)vertices.push_back(0.0f);

	for (int i = 0; i < points.size(); i++)
	{		
		auto pos = points[i]->GetPos();
		vertices[i * 6] = pos.x;
		vertices[i * 6 + 1] = pos.y;
		vertices[i * 6 + 2] = pos.z;
		vertices[i * 6 + 3] = 0.0f;
		vertices[i * 6 + 4] = 1.0f;
		vertices[i * 6 + 5] = 0.0f;
	}
	for (int i = 1; i < points.size(); ++i)
	{
		indices.push_back(i - 1);
		indices.push_back(i);
	}
	return true;
}