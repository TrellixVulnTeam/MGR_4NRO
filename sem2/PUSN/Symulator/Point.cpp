#include "Point.h"
#include "imgui\imgui.h"
#include <string>
#include "SomeCurve.h"

Point::Point(bool addIndex) : Figure()
{
	if (addIndex)
		sprintf_s(name, STRMAX, ("Point - " + std::to_string(idx++) + " " + gen_random(10, idx)).c_str());
	else
		sprintf_s(name, STRMAX, "Point");
	figureType = FigureType::Point;
}

void Point::Initialize(std::shared_ptr<Program> _program)
{
	Figure::Initialize(_program);
}

bool Point::GetGuiInternal(std::shared_ptr<Figure> par)
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
			(std::static_pointer_cast<SomeCurve>(program->figures[selectedBezier]))->AddPoint(std::shared_ptr<Point>(this));
		}
		ImGui::TreePop();
	}
	return to_ret;
}

void Point::Draw()
{
	Figure::Draw();
	glPointSize(4.0f);
	glDrawElements(GL_POINTS, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void Point::Unpin(std::shared_ptr<Figure> par)
{
	int j = -1;
	for (int i = 0; i < parents.size(); ++i)
		if (parents[i].get() == par.get()) j = i;
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
			(std::static_pointer_cast<SomeCurve>(parents[i]))->Recalc();
		}
	}
}

void Point::AddParent(std::shared_ptr<Figure> par)
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
		vertices[3] = 1.0f;
		vertices[4] = 0.0f;
		vertices[5] = 0.0f;

		indices.clear();
		indices.push_back(0);

		firstTime = false;
		return true;
	}

	return false;

}

