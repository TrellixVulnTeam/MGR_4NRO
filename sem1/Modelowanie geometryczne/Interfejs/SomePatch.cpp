#include "SomePatch.h"
#include "imgui\imgui.h"
#include <string>
#include "BezierPatchC0.h"
#include "BezierPatchC2.h"
#include "Intersections.h"

SomePatch::SomePatch() :Figure()
{
	isPatch = true;
	canMove = false;
	pointsLines = new PointsLines();
}

bool SomePatch::GetGuiInternal(Figure* par)
{
	bool b = false;

	ImGui::Checkbox("Draw line", &drawLine);
	if (figureType == FigureType::BezierPatchC0)
	{
		ImGui::SliderInt("Split A", &(((BezierPatchC0*)this)->splitA), 1, 100);
		ImGui::SliderInt("Split B", &(((BezierPatchC0*)this)->splitB), 1, 100);
	}
	if (figureType == FigureType::BezierPatchC2)
	{
		ImGui::SliderInt("Split A", &(((BezierPatchC2*)this)->splitA), 1, 100);
		ImGui::SliderInt("Split B", &(((BezierPatchC2*)this)->splitB), 1, 100);
	}
	if (ImGui::Button("Select all points"))
	{
		for (int i = 0; i < points.size(); ++i)points[i]->Select();
	}
	if (ImGui::Button("Unselect all points"))
	{
		for (int i = 0; i < points.size(); ++i)points[i]->Unselect();
	}
	if (figureType == FigureType::BezierPatchC0 || figureType == FigureType::BezierPatchC2)
	{
		if (ImGui::Button("Inverse"))
		{
			SetTexture(trimTex, trimTex, false, true);
		}
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
	return b;
}

