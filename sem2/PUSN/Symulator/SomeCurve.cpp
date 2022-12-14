#include "SomeCurve.h"
#include "imgui\imgui.h"
#include <string>

SomeCurve::SomeCurve() :Figure()
{
	isCurve = true;
	canMove = false;
	pointsLine = new PointsLine();
}

bool SomeCurve::GetGuiInternal(std::shared_ptr<Figure> par)
{
	bool b = false;

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
			if (points[i]->GetGui(i, std::shared_ptr<Figure>(this)))
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

