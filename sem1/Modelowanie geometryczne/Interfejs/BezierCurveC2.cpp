#include "BezierCurveC2.h"
#include "imgui\imgui.h"
#include <string>

BezierCurveC2::BezierCurveC2() : SomeCurve()
{
	sprintf_s(name, STRMAX, "BezierCurveC2");
	_name = "BezierCurveC2";
	figureType = FigureType::BezierCurveC2;
	deBoorLine = new PointsLine();
}

void BezierCurveC2::Initialize(Program* _program)
{
	SomeCurve::Initialize(_program);
	pointsLine->Initialize(program);
	deBoorLine->Initialize(program);
}

void BezierCurveC2::BernsteinMoved(int j)
{
	if (j % 4 == 0)
	{
		int moved = j / 4 + 1;
		auto mid = (points[moved - 1]->GetPos() + points[moved + 1]->GetPos()) / 2.0f;
		auto vec = bernsteinPoints[j]->GetPos() - mid;
		auto newPos = mid + 3.0f / 2.0f * vec;
		points[moved]->MoveTo(newPos.x, newPos.y, newPos.z);
	}
	if (j % 4 == 1)
	{
		int moved = j / 4 + 2;
		int source = moved - 1;
		auto vec = bernsteinPoints[j]->GetPos() - points[source]->GetPos();
		auto newPos = points[source]->GetPos() + 3.0f * vec;
		points[moved]->MoveTo(newPos.x, newPos.y, newPos.z);
	}
	if (j % 4 == 2)
	{
		int moved = j / 4 + 2;
		int source = moved - 1;
		auto vec = bernsteinPoints[j]->GetPos() - points[source]->GetPos();
		auto newPos = points[source]->GetPos() + 3.0f / 2.0f * vec;
		points[moved]->MoveTo(newPos.x, newPos.y, newPos.z);
	}
	if (j % 4 == 3)
	{
		int moved = j / 4 + 2;
		auto mid = (points[moved - 1]->GetPos() + points[moved + 1]->GetPos()) / 2.0f;
		auto vec = bernsteinPoints[j]->GetPos() - mid;
		auto newPos = mid + 3.0f / 2.0f * vec;
		points[moved]->MoveTo(newPos.x, newPos.y, newPos.z);
	}
	first = true;
}

void BezierCurveC2::Draw()
{
	Figure::Draw();
	glDrawElements(GL_LINES_ADJACENCY, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	if (drawLine)
		pointsLine->Draw();
	if (drawDeBoorLine)
		deBoorLine->Draw();

	for (int i = 0; i < bernsteinPoints.size(); ++i)
	{
		bernsteinPoints[i]->Draw();
	}
}

void BezierCurveC2::AddPoint(Point* point)
{
	points.push_back(point);
	first = true;
}

void BezierCurveC2::RemovePoint(int to_del)
{
	points.erase(points.begin() + to_del);
	first = true;
}

void BezierCurveC2::CleanUp()
{
	for (int i = 0; i < points.size(); ++i)
		points[i]->Unpin(this);
	delete pointsLine;
	delete deBoorLine;

	for (int i = 0; i < bernsteinPoints.size(); ++i)
	{
		delete bernsteinPoints[i];
	}
}

bool BezierCurveC2::Create()
{
	bool fCreate = Figure::Create();
	if (!fCreate && !first) return false;
	first = false;
	for (int i = 0; i < bernsteinPoints.size(); ++i) delete bernsteinPoints[i];
	bernsteinPoints.clear();
	pointsLine->Clear();
	deBoorLine->Clear();
	vertices.clear();
	indices.clear();

	for (int i = 0; i < points.size(); ++i)
	{
		deBoorLine->AddPoint(points[i]);
	}
	deBoorLine->RecalcFigure();

	if (points.size() < 4) {
		pointsLine->RecalcFigure();
		return false;
	}
	for (int i = 3; i < points.size(); ++i)
	{
		glm::vec4 xs;
		glm::vec4 ys;
		glm::vec4 zs;

		glm::vec3 pos0 = points[i - 3]->GetPos();
		glm::vec3 pos1 = points[i - 2]->GetPos();
		glm::vec3 pos2 = points[i - 1]->GetPos();
		glm::vec3 pos3 = points[i]->GetPos();
		DeBoorToBernstein(pos0.x, pos1.x, pos2.x, pos3.x, xs);
		DeBoorToBernstein(pos0.y, pos1.y, pos2.y, pos3.y, ys);
		DeBoorToBernstein(pos0.z, pos1.z, pos2.z, pos3.z, zs);

		Point* p0 = new Point();
		p0->Initialize(program);
		p0->MoveTo(xs.x, ys.x, zs.x);
		Point* p1 = new Point();
		p1->Initialize(program);
		p1->MoveTo(xs.y, ys.y, zs.y);
		Point* p2 = new Point();
		p2->Initialize(program);
		p2->MoveTo(xs.z, ys.z, zs.z);
		Point* p3 = new Point();
		p3->Initialize(program);
		p3->MoveTo(xs.w, ys.w, zs.w);

		bernsteinPoints.push_back(p0);
		bernsteinPoints.push_back(p1);
		bernsteinPoints.push_back(p2);
		bernsteinPoints.push_back(p3);

		pointsLine->AddPoint(p0);
		pointsLine->AddPoint(p1);
		pointsLine->AddPoint(p2);
		pointsLine->AddPoint(p3);
	}

	int k = 0;
	float from=0.0f;
	float to=1.0f;
	for (int i = 0; i < bernsteinPoints.size() - 1; i += 4)
	{
		for (int j = i; j < i + 4 && j < bernsteinPoints.size(); ++j)
		{
			auto pos = bernsteinPoints[j]->GetPos();
			vertices.push_back(pos.x);
			vertices.push_back(pos.y);
			vertices.push_back(pos.z);
			if (k % 4 == 0)
			{
				vertices.push_back(0.0f);
				vertices.push_back(0.0f);
				vertices.push_back(1.0f);
			}
			else
			{
				vertices.push_back(0.0f);
				vertices.push_back(from);
				vertices.push_back(to);
			}
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
		vertices.push_back(from);
		vertices.push_back(to);
		indices.push_back(k);
		++k;
	}
	pointsLine->RecalcFigure();
	return true;


}

