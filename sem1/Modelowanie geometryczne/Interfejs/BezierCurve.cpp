#include "BezierCurve.h"
#include "imgui\imgui.h"
#include <string>
#include "Math.h"

BezierCurve::BezierCurve(Shader _shader) : Figure(_shader)
{
	sprintf_s(name, STRMAX, "BezierCurve");
	_name = "BezierCurve";
	figureType = FigureType::BezierCurve;
	canMove = false;
	pointsLine = new PointsLine(_shader);
	pointsLine->Initialize();
}

bool BezierCurve::GetGuiInternal(std::vector<Figure*> figures, bool fromMainGui)
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
			if (points[i]->GetGui(i, figures, false))
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

void BezierCurve::Draw(int transLoc)
{
	Figure::Draw(transLoc);
	glDrawElements(GL_POINTS, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	if (drawLine)
		pointsLine->Draw(transLoc);
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
		points[i]->Unpin();
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
	int n = 200;



	for (int i = 0; i < points.size(); i += 3)
	{
		int it = i / 3;
		std::vector<float> coeffs_x;
		std::vector<float> coeffs_y;
		std::vector<float> coeffs_z;
		for (int j = i; j < i + 4 && j < points.size(); ++j)
		{
			auto pos = points[j]->GetPos();
			coeffs_x.push_back(pos.x);
			coeffs_y.push_back(pos.y);
			coeffs_z.push_back(pos.z);
		}
		if (coeffs_x.size() > 1)
		{

			for (int k = it * 6 * n; k < (it + 1) * 6 * n; ++k) vertices.push_back(0.0f);
			for (int k = it * n; k < (it + 1) * n; ++k)indices.push_back(k);

			for (int l = 0; l < n; ++l)
			{
				float t = (float)l / n;

				int bias = it * 6 * n;
				vertices[bias + l * 6] = DeCasteljau(coeffs_x, t);
				vertices[bias + l * 6 + 1] = DeCasteljau(coeffs_y, t);
				vertices[bias + l * 6 + 2] = DeCasteljau(coeffs_z, t);
				vertices[bias + l * 6 + 3] = 0.0f;
				vertices[bias + l * 6 + 4] = 0.0f;
				vertices[bias + l * 6 + 5] = 1.0f;
			}
		}
	}
	return true;


}

