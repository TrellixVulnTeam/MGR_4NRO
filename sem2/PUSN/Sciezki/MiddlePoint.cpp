#include "MiddlePoint.h"
#include "imgui\imgui.h"
#include <string>
#include "Point.h"

MiddlePoint::MiddlePoint() : Figure()
{
	sprintf_s(name, STRMAX, "MiddlePoint");
	figureType = FigureType::MiddlePoint;
}

void MiddlePoint::Initialize(Program* _program)
{
	Figure::Initialize(_program);
}

bool MiddlePoint::GetGuiInternal(Figure* par)
{
	bool b = false;

	return b;
}

void MiddlePoint::Reset()
{
	count = 0;
	pos = glm::vec3(0.0f);
}

void MiddlePoint::Add(Figure* figure)
{
	if(figure->figureType == FigureType::Point)
		if(((Point*)figure)->special)
			 return;
	if (figure->GetSelected() && figure->CanMove())
	{
		count++;
		pos += figure->GetPos();
	}
}

void MiddlePoint::Draw()
{
	if (count > 0)
	{
		MoveTo(pos.x/count, pos.y/count, pos.z/count);
		Figure::Draw();
		glPointSize(3.0f);
		glDrawElements(GL_POINTS, indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}
}

bool MiddlePoint::Create()
{
	bool fCreate = Figure::Create();
	if (firstTime || fCreate)
	{
		vertices.clear();
		for (int i = 0; i < 6; ++i) vertices.push_back(0.0f);
		vertices[0] = 0.0f;
		vertices[1] = 0.0f;
		vertices[2] = 0.0f;
		vertices[3] = 1.0f;
		vertices[4] = 1.0f;
		vertices[5] = 0.0f;

		indices.clear();
		indices.push_back(0);

		firstTime = false;
		return true;
	}

	return false;

}

