#include "Point.h"
#include "imgui\imgui.h"
#include <string>
#include "SomeCurve.h"
#include "SomePatch.h"
#include "Hole.h"

Point::Point(bool addIndex) : Figure()
{
	if (addIndex)
		sprintf_s(name, STRMAX, ("Point - " + std::to_string(idx++) + " " + gen_random(10,idx)).c_str());
	else
		sprintf_s(name, STRMAX, "Point");
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
			if (program->figures[i]->isCurve)
				if (ImGui::Button(program->figures[i]->name))
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
	if (par != nullptr && !par->isPatch)
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
	if (parents_cnt[j] == 1)
	{
		parents.erase(parents.begin() + j);
		parents_cnt.erase(parents_cnt.begin() + j);
	}
	else
	{
		parents_cnt[j]--;
	}
}

void Point::RecalcParent()
{
	for (int i = 0; i < parents.size(); ++i)
	{
		if (parents[i]->isCurve)
		{
			((SomeCurve*)parents[i])->Recalc();
		}
		if (parents[i]->isPatch)
		{
			((SomePatch*)parents[i])->Recalc();
		}
		if (parents[i]->figureType == FigureType::Hole)
		{
			((Hole*)parents[i])->Recalc();
		}
	}
}

void Point::AddParent(Figure* par)
{
	int j = -1;
	for (int i = 0; i < parents.size(); ++i)
		if (parents[i] == par) j = i;

	if (j == -1)
	{
		parents.push_back(par);
		parents_cnt.push_back(1);
	}
	else
	{
		parents_cnt[j]++;
	}
}

void Point::ReplaceInParents(Point* newPoint)
{
	for(int i=0;i<parents.size();++i)
	{
		if (parents[0]->isCurve)
		{
			((SomeCurve*)parents[0])->ReplaceInParent(this, newPoint);
		}
		if (parents[0]->isPatch)
		{
			((SomePatch*)parents[0])->ReplaceInParent(this, newPoint);
		}
	}
	parents.clear();
	parents_cnt.clear();
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

