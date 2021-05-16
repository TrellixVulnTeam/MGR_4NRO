#include "SomePatch.h"
#include "imgui\imgui.h"
#include <string>
#include "BezierPatchC0.h"
#include "BezierPatchC0Cylinder.h"

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
		ImGui::SliderInt("Split A", &(((BezierPatchC0*)this)->splitA), 1, 200);
		ImGui::SliderInt("Split B", &(((BezierPatchC0*)this)->splitB), 1, 200);
		ImGui::SliderInt("m", &(((BezierPatchC0*)this)->m), 1, 50);
		ImGui::SliderInt("n", &(((BezierPatchC0*)this)->n), 1, 50);
		ImGui::SliderFloat("width", &(((BezierPatchC0*)this)->width), 0.5f, 5.0f);
		ImGui::SliderFloat("length", &(((BezierPatchC0*)this)->length), 0.5f, 5.0f);
	}
	if (figureType == FigureType::BezierPatchC0Cylinder)
	{
		ImGui::SliderInt("Split A", &(((BezierPatchC0Cylinder*)this)->splitA), 1, 200);
		ImGui::SliderInt("Split B", &(((BezierPatchC0Cylinder*)this)->splitB), 1, 200);
		ImGui::SliderInt("m", &(((BezierPatchC0Cylinder*)this)->m), 1, 50);
		ImGui::SliderInt("n", &(((BezierPatchC0Cylinder*)this)->n), 1, 50);
		ImGui::SliderFloat("r", &(((BezierPatchC0Cylinder*)this)->r), 0.5f, 5.0f);
		ImGui::SliderFloat("length", &(((BezierPatchC0Cylinder*)this)->length), 0.5f, 5.0f);
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
	return b;
}

