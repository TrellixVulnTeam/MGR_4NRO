#include "Point.h"
#include "imgui\imgui.h"
#include <string>
#include "SomeCurve.h"

Point::Point() : Figure()
{
	sprintf_s(name, STRMAX, "Point");
	_name = "Point";
	figureType = FigureType::Point;
}

void Point::Initialize(Program* _program)
{
	Figure::Initialize(_program);
}

bool Point::GetGuiInternal(Figure* par)
{
	bool to_ret = false;

	int selectedBezier = -1;
	if (ImGui::TreeNode("Pin to Bezier..."))
	{
		for (int i = 0; i < program->figures.size(); i++)
		{
			if (program->figures[i]->isCurve
				&& !(std::find(parents.begin(), parents.end(), program->figures[i]) != parents.end()))
				if (ImGui::Button(program->figures[i]->gui_name))
				{
					selectedBezier = i;
				}
		}
		if (selectedBezier != -1)
		{
			AddParent(program->figures[selectedBezier]);
			((SomeCurve*)program->figures[selectedBezier])->AddPoint(this);
		}
		ImGui::TreePop();
	}
	if (par != nullptr)
	{
		if (ImGui::Button("Unpin"))
		{
			to_ret = true;
			Unpin(par);
		}
	}
	return to_ret;
}

void Point::Draw()
{
	Figure::Draw();
	glPointSize(2.0f);
	glDrawElements(GL_POINTS, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void Point::Unpin(Figure* par)
{
	int j = -1;
	for (int i = 0; i < parents.size(); ++i)
		if (parents[i] == par) j = i;
	parents.erase(parents.begin() + j);
	if (parents.size() == 0)
		showInMainGui = true;
}

void Point::RecalcParent()
{
	for (int i = 0; i < parents.size(); ++i)
	{
		if (parents[i]->isCurve)
		{
			((SomeCurve*)parents[i])->Recalc();
		}
	}
}

void Point::AddParent(Figure* f)
{
	parents.push_back(f);
	showInMainGui = false;
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
		vertices[4] = selected ? 1.0f : 1.0f;
		vertices[5] = !virtualPoint ? 0.0f : 1.0f;

		indices.clear();
		indices.push_back(0);

		firstTime = false;
		return true;
	}

	return false;

}

