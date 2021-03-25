#include "Point.h"
#include "imgui\imgui.h"
#include <string>
#include "BezierCurve.h"

Point::Point(Shader _shader) : Figure(_shader)
{
	sprintf_s(name, STRMAX, "Point");
	_name = "Point";
	figureType = FigureType::Point;
}

bool Point::GetGuiInternal(std::vector<Figure*> figures, bool fromMainGui)
{
	bool to_ret = false;
	if (fromMainGui)
	{
		if (ImGui::Button("Pin to Bezier.."))
			ImGui::OpenPopup("my_select_popup");
		int selectedBezier = -1;
		if (ImGui::BeginPopup("my_select_popup"))
		{
			for (int i = 0; i < figures.size(); i++)
			{
				if (figures[i]->figureType == FigureType::BezierCurve)
					if (ImGui::Selectable(figures[i]->name))
						selectedBezier = i;
			}
			if (selectedBezier != -1)
			{
				AddParent(figures[selectedBezier]);
				((BezierCurve*)figures[selectedBezier])->AddPoint(this);
			}
			ImGui::EndPopup();
		}
	}
	else
	{
		if (ImGui::Button("Unpin"))
		{
			to_ret = true;
			parent = NULL;
			Unpin();
		}
	}
	return to_ret;
}

void Point::Draw(int transLoc)
{
	Figure::Draw(transLoc);
	glPointSize(2.0f);
	glDrawElements(GL_POINTS, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void Point::Unpin()
{
	showInMainGui = true;
}

void Point::RecalcParent()
{
	if (parent != NULL)
	{
		if (parent->figureType == FigureType::BezierCurve)
		{
			((BezierCurve*)parent)->Recalc();
		}
	}
}

void Point::AddParent(Figure* f)
{
	parent = f;
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
		vertices[4] = selected ? 1.0f : 0.0f;
		vertices[5] = 0.0f;
		
		indices.clear();
		indices.push_back(0);

		firstTime = false;
		return true;
	}

	return false;

}

