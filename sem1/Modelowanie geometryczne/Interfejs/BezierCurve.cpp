#include "BezierCurve.h"
#include "imgui\imgui.h"
#include <string>

BezierCurve::BezierCurve() : Figure()
{
	sprintf_s(name, STRMAX, "BezierCurve");
	_name = "BezierCurve";
	figureType = FigureType::BezierCurve;
	canMove = false;
	pointsLine = new PointsLine();
}

void BezierCurve::Initialize(Program* _program)
{
	Figure::Initialize(_program);
	shader = Shader(program->bezierShader);
	pointsLine->Initialize(program);
}

bool BezierCurve::GetGuiInternal(Figure* par)
{
	bool b = false;

	ImGui::Checkbox("Draw line", &drawLine);
	if (ImGui::Button("Select all points"))
	{
		for (int i = 0; i < points.size(); ++i)points[i]->Select();
	}
	if (ImGui::Button("Unselect all points"))
	{
		for (int i = 0; i < points.size(); ++i)points[i]->Unselect();
	}
	int to_del = -1;
	if (ImGui::TreeNode("Points"))
	{
		for (int i = 0; i < points.size(); ++i)
		{
			if (points[i]->GetGui(i, this))
			{
				to_del = i;
			}
		}
		ImGui::TreePop();
	}
	if (to_del != -1)
	{
		pointsLine->RemoveAt(to_del);
		points.erase(points.begin() + to_del);
		first = true;
	}
	return b;
}

void BezierCurve::Draw()
{
	Figure::Draw();
	glDrawElements(GL_LINES_ADJACENCY, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	if (drawLine)
		pointsLine->Draw();
}

void BezierCurve::AddPoint(Point* point)
{
	points.push_back(point);
	first = true;
	pointsLine->AddPoint(point);
}

void BezierCurve::CleanUp()
{
	for (int i = 0; i < points.size(); ++i)
		points[i]->Unpin(this);
	delete pointsLine;
}

bool BezierCurve::Create()
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

