#include "BezierPatchC0.h"
#include "imgui\imgui.h"
#include <string>

BezierPatchC0::BezierPatchC0() : SomePatch()
{
	sprintf_s(name, STRMAX, "BezierPatchC0");
	_name = "BezierPatchC0";
	figureType = FigureType::BezierPatchC0;
}

BezierPatchC0::BezierPatchC0(int _n, int _m, float _width, float _length) : BezierPatchC0()
{
	n = _n;
	m = _m;
	width = _width;
	length = _length;
}

void BezierPatchC0::Initialize(Program* _program)
{
	SomePatch::Initialize(_program);

	float x = 0.0f;
	float z = 0.0f;
	float xdiff = width / (3 * n);
	float zdiff = length / (3 * m);
	int k = 0;
	for (int i = 0; i < 3 * n + 1; ++i)
	{
		z = 0.0f;
		for (int j = 0; j < 3 * m + 1; ++j)
		{
			Point* p = new Point();
			p->Initialize(_program);
			p->MoveTo(x, 0.0f, z);
			points.push_back(p);
			z += zdiff;
			program->figures.push_back(p);
			p->AddParent(this);
			if (j != 0)
			{
				pointsLines->AddPoint(points[k - 1]);
				pointsLines->AddPoint(points[k]);
			}
			if (i != 0)
			{
				pointsLines->AddPoint(points[k - (3 * m + 1)]);
				pointsLines->AddPoint(points[k]);
			}
			++k;
		}
		x += xdiff;
	}

	pointsLines->Initialize(program);
}

void BezierPatchC0::Draw()
{
	//Figure::Draw();
	//glDrawElements(GL_LINES_ADJACENCY, indices.size(), GL_UNSIGNED_INT, 0);
	//glBindVertexArray(0);
	for (int i = 0; i < points.size(); ++i) points[i]->Draw();
	if (drawLine)
		pointsLines->Draw();
}

void BezierPatchC0::CleanUp()
{
	for (int i = 0; i < points.size(); ++i)
		points[i]->Unpin(this);
	delete pointsLines;
}

bool BezierPatchC0::Create()
{
	bool fCreate = Figure::Create();
	if (!fCreate && !first) return false;
	first = false;
	pointsLines->RecalcFigure();
	return false;
	//vertices.clear();
	//indices.clear();
	//if (points.size() < 2) return false;

	//int total_n = 0;

	//int k = 0;
	//float from = 0.0f;
	//float to = 1.0f;
	//for (int i = 0; i < points.size() - 1; i += 3)
	//{
	//	int n = 0;
	//	for (int j = i + 1; j < i + 4 && j < points.size(); ++j)
	//	{
	//		glm::ivec3 pos_a = GetScreenPos(program, glm::vec4(points[j - 1]->GetPos(), 1.0f));
	//		glm::ivec3 pos_b = GetScreenPos(program, glm::vec4(points[j]->GetPos(), 1.0f));
	//		int x, y, z;
	//		x = pos_b.x - pos_a.x;
	//		y = pos_b.y - pos_a.y;
	//		z = pos_b.z - pos_a.z;
	//		n += sqrt(x * x + y * y + z * z);
	//	}
	//	if (n > 100000 || n < 0) n = 100000;

	//	for (int l = 0; l < n; l += 250)
	//	{
	//		from = (float)l / n;
	//		to = (float)(l + 250) / n;
	//		if (to > 1.0f)to = 1.0f;
	//		for (int j = i; j < i + 4 && j < points.size(); ++j)
	//		{
	//			auto pos = points[j]->GetPos();
	//			vertices.push_back(pos.x);
	//			vertices.push_back(pos.y);
	//			vertices.push_back(pos.z);
	//			if (k % 4 == 0)
	//			{
	//				vertices.push_back(0.0f);
	//				vertices.push_back(0.0f);
	//				vertices.push_back(1.0f);
	//			}
	//			else
	//			{
	//				vertices.push_back(0.0f);
	//				vertices.push_back(from);
	//				vertices.push_back(to);
	//			}
	//			indices.push_back(k);
	//			++k;
	//		}
	//		while (indices.size() % 4 != 0)
	//		{
	//			vertices.push_back(0.0f);
	//			vertices.push_back(0.0f);
	//			vertices.push_back(0.0f);
	//			vertices.push_back(-1.0f);
	//			vertices.push_back(from);
	//			vertices.push_back(to);
	//			indices.push_back(k);
	//			++k;
	//		}
	//	}
	//}
	return true;


}

