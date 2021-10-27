#include "PointsLine.h"
#include <iostream>
#include "Cube.h"
#include <string>

PointsLine::PointsLine() : Figure()
{
	sprintf_s(name, STRMAX, "PointsLine");
	figureType = FigureType::PointsLine;
	canMove = true;
}

void PointsLine::Initialize(std::shared_ptr<Program> _program)
{
	Figure::Initialize(_program);
}

void PointsLine::Draw()
{
	if (program->showPath)
	{
		Figure::Draw();
		glDisable(GL_DEPTH_TEST);
		glDrawElements(GL_LINES, indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
		glEnable(GL_DEPTH_TEST);
	}
}

void PointsLine::AddPoint(glm::vec3 point)
{
	points.push_back(point);
}

void PointsLine::RemoveAt(int i)
{
	points.erase(points.begin() + i);
}

void PointsLine::Clear()
{
	points.clear();
}

bool PointsLine::Create()
{
	vertices.clear();
	indices.clear();
	if (points.size() < 1) return false;

	for (int i = 0; i < 6 * points.size(); ++i)vertices.push_back(0.0f);

	for (int i = 0; i < points.size(); i++)
	{
		auto pos = points[i];
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

	if (points.size() == 1)
	{
		for (int i = 0; i < 6; ++i) vertices.push_back(0.0f);
		auto pos = points[0];
		vertices[1 * 6] = pos.x;
		vertices[1 * 6 + 1] = pos.y;
		vertices[1 * 6 + 2] = pos.z;
		vertices[1 * 6 + 3] = 0.0f;
		vertices[1 * 6 + 4] = 1.0f;
		vertices[1 * 6 + 5] = 0.0f;
		indices.push_back(0);
		indices.push_back(1);
	}
	return true;
}

glm::vec3 PointsLine::Drill()
{
	if (!drillInitialized)
	{
		drillInitialized = true;
		drillingPos = points[0];
		startPoints = points.size() - 1;
	}
	if (points.size() < 2) return glm::vec3(0.0f, 0.0f, 0.0f);
	float drillLeft = program->drillingSpeed;
	while ((drillLeft > 0.0f || !program->showSimulation) && points.size() > 1 && (program->error.length() == 0 || program->ignoreErrors))
	{
		int p = round(100 * (1.0f - (((float)(points.size() - 1)) / startPoints)));
		if (p != lastP) {
			std::cout << p << "% done" << std::endl;
			lastP = p;
		}
		float dist = glm::distance(drillingPos, points[1]);
		if (dist > drillLeft && program->showSimulation)
		{
			glm::vec3 v = points[1] - drillingPos;
			v = v / dist * drillLeft;
			glm::vec3 newDrillingPos = drillingPos + v;
			program->cube->Drill(drillingPos, newDrillingPos);
			drillingPos = newDrillingPos;
			drillLeft = 0;
		}
		else
		{
			program->cube->Drill(drillingPos, points[1]);
			drillingPos = points[1];
			drillLeft -= dist;
			points.erase(points.begin());
		}
	}
	return glm::vec3(GetModel() * glm::vec4(drillingPos, 1.0f));
}