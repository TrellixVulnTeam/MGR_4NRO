#include "SomeCurve.h"
#include "imgui\imgui.h"
#include <string>
#include "BezierCurveC2.h"
#include "InterpolationCurveC2.h"
#include "IntersectionLine.h"

SomeCurve::SomeCurve() :Figure()
{
	isCurve = true;
	canMove = false;
	pointsLine = new PointsLine();
}

bool SomeCurve::GetGuiInternal(Figure* par)
{
	bool b = false;

	if (figureType != FigureType::IntersectionLine)
		ImGui::Checkbox("Draw line", &drawLine);
	if (figureType == FigureType::IntersectionLine)
	{
		if (ImGui::Button("Convert to Interpolation"))
		{
			((IntersectionLine*)this)->ConvertToInterpolation();
		}
	}
	if (figureType == FigureType::BezierCurveC2)
	{
		ImGui::Checkbox("Draw DeBoor line", &(((BezierCurveC2*)this)->drawDeBoorLine));
	}
	if (figureType == FigureType::InterpolationCurveC2)
	{
		ImGui::Checkbox("Draw Bernstein line", &(((InterpolationCurveC2*)this)->drawBernsteinLine));
	}
	if (figureType == FigureType::InterpolationCurveC2)
	{
		ImGui::Checkbox("Draw Bernstein points", &(((InterpolationCurveC2*)this)->drawBernsteinPoints));
	}
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
		RemovePoint(to_del);
	}
	return b;
}

