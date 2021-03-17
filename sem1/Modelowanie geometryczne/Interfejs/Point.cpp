#include "Point.h"
#include "imgui\imgui.h"
#include <string>

Point::Point(Shader _shader) : Figure(_shader)
{
	name = std::string("Point");
}

bool Point::GetGui(int i)
{
	bool b = false;

	b = Figure::GetGui(i);
	return b;
}

void Point::Draw(int transLoc)
{
	Figure::Draw(transLoc);
	glPointSize(2.0f);
	glDrawElements(GL_POINTS, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

bool Point::Create()
{
	bool fCreate = Figure::Create();
	if (firstTime || fCreate)
	{
		vertices.clear();
		for (int i = 0; i < 6; ++i) vertices.push_back(0.0f);
		vertices[0] = 0.0f;
		vertices[1] = 0.0f;
		vertices[2] = 0.0f;
		vertices[3] = selected ? 0.0f : 1.0f;
		vertices[4] = selected ? 1.0f : 0.0f;
		vertices[5] = 0.0f;
		
		indices.clear();
		indices.push_back(0);

		firstTime = false;
		return true;
	}

	return false;

}

