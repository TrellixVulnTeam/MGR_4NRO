#include "BezierPatchC0.h"
#include "imgui\imgui.h"
#include <string>

BezierPatchC0::BezierPatchC0() : SomePatch()
{
	sprintf_s(name, STRMAX, ("BezierPatchC0 - " + std::to_string(idx++)).c_str());
	figureType = FigureType::BezierPatchC0;
}

void BezierPatchC0::Initialize(Program* _program)
{
	SomePatch::Initialize(_program);
	pointsLines->Initialize(program);
	GeneratePoints();
}

void BezierPatchC0::ClearPoints()
{
	for (int i = 0; i < points.size(); ++i)
	{
		if (points[i]!= nullptr && !points[i]->toDel)
		{
			points[i]->Unpin(this);
			if (!points[i]->HasParent()) points[i]->toDel = true;

			for (int j = i + 1; j < points.size(); ++j)
			{
				if (points[j] == points[i]) points[j] = nullptr;
			}
		}
	}
	int n = program->figures.size();
	for (int i = 0; i < n; ++i)
	{
		if (program->figures[i]->figureType == FigureType::Point)
		{
			if (((Point*)program->figures[i])->toDel)
			{
				delete program->figures[i];
				program->figures.erase(program->figures.begin() + i);
				i--;
				n--;
			}
		}
	}
}

void BezierPatchC0::GeneratePoints()
{
	first = true;
	ClearPoints();
	points.clear();
	pointsLines->Clear();
	if (cylinder)
	{
		if (n < 3) n = 3;
		float z = 0.0f;
		float angle = 0.0f;
		float zDiff = length / (3 * m);
		float angleDiff = 2 * M_PI / (3 * n);
		int k = 0;
		for (int i = 0; i < 3 * m + 1; ++i)
		{
			angle = 0.0f;
			for (int j = 0; j < 3 * n; ++j)
			{
				Point* p = new Point();
				p->Initialize(program);
				p->MoveTo(r * sin(angle), r * cos(angle), z);
				points.push_back(p);
				angle += angleDiff;
				program->figures.push_back(p);
				p->AddParent(this);
				if (j != 0)
				{
					pointsLines->AddPoint(points[k]);
					pointsLines->AddPoint(points[k - 1]);
				}
				if (i != 0)
				{
					pointsLines->AddPoint(points[k - (3 * n + 1)]);
					pointsLines->AddPoint(points[k]);
				}
				++k;
			}
			points.push_back(points[k - 3 * n]);
			pointsLines->AddPoint(points[k]);
			pointsLines->AddPoint(points[k - 1]);
			k++;
			z += zDiff;
		}
	}
	else
	{
		float x = 0.0f;
		float y = 0.0f;
		float xdiff = width / (3 * m);
		float ydiff = length / (3 * n);
		int k = 0;
		for (int i = 0; i < 3 * m + 1; ++i)
		{
			y = 0.0f;
			for (int j = 0; j < 3 * n + 1; ++j)
			{
				Point* p = new Point();
				p->Initialize(program);
				p->MoveTo(x, y, 0.0f);
				points.push_back(p);
				y += ydiff;
				program->figures.push_back(p);
				p->AddParent(this);
				if (j != 0)
				{
					pointsLines->AddPoint(points[k - 1]);
					pointsLines->AddPoint(points[k]);
				}
				if (i != 0)
				{
					pointsLines->AddPoint(points[k - (3 * n + 1)]);
					pointsLines->AddPoint(points[k]);
				}
				++k;
			}
			x += xdiff;
		}
	}
}

void BezierPatchC0::RecalcFigure()
{
	if (Create()) {
		glBindVertexArray(VAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);

		glBufferData(
			GL_ARRAY_BUFFER,
			vertices.size() * sizeof(float),
			&vertices[0],
			GL_STATIC_DRAW
		);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

		glBufferData(
			GL_ELEMENT_ARRAY_BUFFER,
			indices.size() * sizeof(unsigned int),
			&indices[0],
			GL_STATIC_DRAW
		);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 15 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 15 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);

		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 15 * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(2);

		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 15 * sizeof(float), (void*)(9 * sizeof(float)));
		glEnableVertexAttribArray(3);

		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 15 * sizeof(float), (void*)(12 * sizeof(float)));
		glEnableVertexAttribArray(4);

		glBindVertexArray(0);
	}
}

void BezierPatchC0::Draw()
{
	Figure::Draw();
	glDrawElements(GL_LINES_ADJACENCY, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	for (int i = 0; i < points.size(); ++i) points[i]->Draw();
	if (drawLine)
		pointsLines->Draw();
}

void BezierPatchC0::CleanUp()
{
	ClearPoints();
	delete pointsLines;
}

bool BezierPatchC0::Create()
{
	if (splitAold != splitA || splitBold != splitB) first = true;
	bool fCreate = Figure::Create();
	if (!fCreate && !first) return false;
	if (points.size() == 0) return false;
	first = false;
	splitBold = splitB;
	splitAold = splitA;
	pointsLines->RecalcFigure();
	vertices.clear();
	indices.clear();
	int p = splitA;
	int q = splitB;
	int ii = 0;
	for (int i = 0; i < m; ++i)
	{
		for (int j = 0; j < n; ++j)
		{
			for (int k = 0; k < p; ++k)
			{
				for (int l = 0; l < q; l += 120)
				{
					float from = (float)l / q;
					float to = (float)(l + 120) / q;
					int splits = ((q - l) < 120) ? (q - l) : 120;
					if (to > 1.0f)to = 1.0f;

					AddPatch(i, j, (float)k / p, (float)(k + 1) / p, from, to, splits, ii);
				}
			}
		}
	}
	return true;
}

void BezierPatchC0::AddPatch(int i, int j, float t, float t2, float from, float to, int splits, int& ii)
{
	int w = 3 * n + 1;
	int start = 3 * i * w + 3 * j;
	int ii_start = ii;
	for (int k = 0; k < 4; ++k)
	{
		auto pos = points[start]->GetPos();
		vertices.push_back(pos.x);
		vertices.push_back(pos.y);
		vertices.push_back(pos.z);

		vertices.push_back(1.0f);
		vertices.push_back(0.0f);
		vertices.push_back(0.0f);
		pos = points[start + 1]->GetPos();
		vertices.push_back(pos.x);
		vertices.push_back(pos.y);
		vertices.push_back(pos.z);
		pos = points[start + 2]->GetPos();
		vertices.push_back(pos.x);
		vertices.push_back(pos.y);
		vertices.push_back(pos.z);
		pos = points[start + 3]->GetPos();
		vertices.push_back(pos.x);
		vertices.push_back(pos.y);
		vertices.push_back(pos.z);
		indices.push_back(ii);
		ii++;
		start += w;
	}
	vertices[15 * (ii_start + 1) + 3] = t;
	vertices[15 * (ii_start + 1) + 4] = t2;
	vertices[15 * (ii_start + 2) + 3] = from;
	vertices[15 * (ii_start + 2) + 4] = to;
	vertices[15 * (ii_start + 2) + 5] = splits;

}