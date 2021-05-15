#include "SomePatch.h"
#include "imgui\imgui.h"
#include <string>
#include "BezierCurveC2.h"
#include "InterpolationCurveC2.h"

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
	return b;
}

