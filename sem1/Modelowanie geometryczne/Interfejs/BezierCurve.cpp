#include "BezierCurve.h"
#include "imgui\imgui.h"
#include <string>

BezierCurve::BezierCurve(Shader _shader) : Figure(_shader)
{
	sprintf_s(name, STRMAX, "BezierCurve");
	_name = "BezierCurve";
	figureType = FigureType::BezierCurve;
	canMove = false;
	pointsLine = new PointsLine(_shader);
	pointsLine->Initialize(program);
}

bool BezierCurve::GetGuiInternal(bool fromMainGui)
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
			if (points[i]->GetGui(i, false))
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
	glDrawElements(GL_LINES, indices.size(), GL_UNSIGNED_INT, 0);
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

	int total_n = 0;

	for (int i = 0; i < points.size(); i += 3)
	{
		int it = i / 3;
		std::vector<float> coeffs_x;
		std::vector<float> coeffs_y;
		std::vector<float> coeffs_z;
		std::vector<glm::ivec3> poss;
		for (int j = i; j < i + 4 && j < points.size(); ++j)
		{
			auto pos = points[j]->GetPos();
			coeffs_x.push_back(pos.x);
			coeffs_y.push_back(pos.y);
			coeffs_z.push_back(pos.z);
			poss.push_back(GetScreenPos(program, glm::vec4(pos, 1.0f)));
		}
		if (coeffs_x.size() > 1)
		{
			int n = 0;
			for (int k = 1; k < poss.size(); ++k)
			{
				int x, y, z;
				x = poss[k].x - poss[k - 1].x;
				y = poss[k].y - poss[k - 1].y;
				z = poss[k].z - poss[k - 1].z;
				n += sqrt(x * x + y * y + z * z);
			}
			if (n > 100000 || n < 0) n = 100000;
			for (int k = 0; k < 6 * n; ++k) vertices.push_back(0.0f);
			for (int k = total_n + 1; k < total_n + n; ++k) { indices.push_back(k - 1); indices.push_back(k); }

			for (int l = 0; l < n; ++l)
			{
				float t = (float)l / n;

				vertices[total_n * 6 + l * 6] = DeCasteljau(coeffs_x, t);
				vertices[total_n * 6 + l * 6 + 1] = DeCasteljau(coeffs_y, t);
				vertices[total_n * 6 + l * 6 + 2] = DeCasteljau(coeffs_z, t);
				vertices[total_n * 6 + l * 6 + 3] = 0.0f;
				vertices[total_n * 6 + l * 6 + 4] = 0.0f;
				vertices[total_n * 6 + l * 6 + 5] = 1.0f;
			}

			total_n += n;
		}
	}
	return true;


}

