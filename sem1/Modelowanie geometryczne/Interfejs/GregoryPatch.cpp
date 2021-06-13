#include "GregoryPatch.h"
#include "imgui\imgui.h"
#include <string>

GregoryPatch::GregoryPatch() : Figure()
{
	sprintf_s(name, STRMAX, ("GregoryPatch - " + std::to_string(idx++) + " " + gen_random(10, idx)).c_str());
	figureType = FigureType::GregoryPatch;
	isPatch = true;
	canMove = false;
	pointsLines = new PointsLines();
}

bool GregoryPatch::GetGuiInternal(Figure* par)
{
	ImGui::SliderInt("Split A", &splitA, 1, 100);
	ImGui::SliderInt("Split B", &splitB, 1, 100);
	return false;
}

void GregoryPatch::Initialize(Program* _program)
{
	Figure::Initialize(_program);
	shader = Shader(program->patchGregoryShader);
	pointsLines->Initialize(program);
	GeneratePoints();
}

void GregoryPatch::ClearPoints()
{
	//for (int i = 0; i < points.size(); ++i)
	//{
	//	points[i]->Unpin(this);
	//	if (!points[i]->HasParent()) points[i]->toDel = true;
	//}
	//int n = program->figures.size();
	//for (int i = 0; i < n; ++i)
	//{
	//	if (program->figures[i]->figureType == FigureType::Point)
	//	{
	//		if (((Point*)program->figures[i])->toDel)
	//		{
	//			delete program->figures[i];
	//			program->figures.erase(program->figures.begin() + i);
	//			i--;
	//			n--;
	//		}
	//	}
	//}
}

void GregoryPatch::GeneratePoints()
{
	first = true;
	ClearPoints();
	points.clear();
	pointsLines->Clear();
	Point* p;

	p = new Point();
	p->Initialize(program);
	p->MoveTo(0.0f, 0.0f, 0.0f);
	points.push_back(p);

	p = new Point();
	p->Initialize(program);
	p->MoveTo(0.0f, 0.25f, 0.0f);
	points.push_back(p);

	p = new Point();
	p->Initialize(program);
	p->MoveTo(0.0f, 0.5f, 0.0f);
	points.push_back(p);

	p = new Point();
	p->Initialize(program);
	p->MoveTo(0.0f, 0.75f, 0.0f);
	points.push_back(p);

	p = new Point();
	p->Initialize(program);
	p->MoveTo(0.25f, 0.0f, 0.0f);
	points.push_back(p);

	//5
	p = new Point();
	p->Initialize(program);
	p->MoveTo(0.25f, 0.25f, 0.5f);
	points.push_back(p);

	//6
	p = new Point();
	p->Initialize(program);
	p->MoveTo(0.25f, 0.25f, 0.0f);
	points.push_back(p);

	//7
	p = new Point();
	p->Initialize(program);
	p->MoveTo(0.25f, 0.5f, 0.0f);
	points.push_back(p);

	//8
	p = new Point();
	p->Initialize(program);
	p->MoveTo(0.25f, 0.5f, 0.0f);
	points.push_back(p);

	p = new Point();
	p->Initialize(program);
	p->MoveTo(0.25f, 0.75f, 0.0f);
	points.push_back(p);

	p = new Point();
	p->Initialize(program);
	p->MoveTo(0.5f, 0.0f, 0.0f);
	points.push_back(p);

	//11
	p = new Point();
	p->Initialize(program);
	p->MoveTo(0.5f, 0.25f, 0.0f);
	points.push_back(p);

	//12
	p = new Point();
	p->Initialize(program);
	p->MoveTo(0.5f, 0.25f, 0.0f);
	points.push_back(p);

	//13
	p = new Point();
	p->Initialize(program);
	p->MoveTo(0.5f, 0.5f, 0.0f);
	points.push_back(p);

	//14
	p = new Point();
	p->Initialize(program);
	p->MoveTo(0.5f, 0.5f, 0.0f);
	points.push_back(p);

	p = new Point();
	p->Initialize(program);
	p->MoveTo(0.5f, 0.75f, 0.0f);
	points.push_back(p);

	p = new Point();
	p->Initialize(program);
	p->MoveTo(0.75f, 0.0f, 0.0f);
	points.push_back(p);

	p = new Point();
	p->Initialize(program);
	p->MoveTo(0.75f, 0.25f, 0.0f);
	points.push_back(p);

	p = new Point();
	p->Initialize(program);
	p->MoveTo(0.75f, 0.5f, 0.0f);
	points.push_back(p);

	p = new Point();
	p->Initialize(program);
	p->MoveTo(0.75f, 0.75f, 0.0f);
	points.push_back(p);
}

void GregoryPatch::ReplaceInParent(Point* oldPoint, Point* newPoint)
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
	for (int i = 0; i < pointsLines->points.size(); ++i)
	{
		if (pointsLines->points[i] == oldPoint)
		{
			pointsLines->points[i] = newPoint;
		}
	}
}

void GregoryPatch::Draw()
{
	Figure::Draw();
	
	glPatchParameteri(GL_PATCH_VERTICES, 20);

	int xLoc = glGetUniformLocation(shader.ID, "x");
	glUniform1f(xLoc, splitA);

	int yLoc = glGetUniformLocation(shader.ID, "y");
	glUniform1f(yLoc, splitB);

	glDrawElements(GL_PATCHES, indices.size(), GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);

	for (int i = 0; i < points.size(); ++i) points[i]->Draw();
}

void GregoryPatch::CleanUp()
{
	ClearPoints();
	delete pointsLines;
}

bool GregoryPatch::Create()
{
	bool fCreate = Figure::Create();
	if (!fCreate && !first) return false;
	if (points.size() == 0) return false;
	first = false;
	//pointsLines->RecalcFigure();
	vertices.clear();
	indices.clear();
	for (int i = 0; i < 20; ++i)
	{
		auto pos = points[i]->GetPos();
		vertices.push_back(pos.x);
		vertices.push_back(pos.y);
		vertices.push_back(pos.z);

		vertices.push_back(pos.z);
		vertices.push_back(pos.z);
		vertices.push_back(pos.z);

		indices.push_back(i);
	}
	return true;
}