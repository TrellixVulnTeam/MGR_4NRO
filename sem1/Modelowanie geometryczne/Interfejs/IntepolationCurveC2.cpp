#include "InterpolationCurveC2.h"
#include "imgui\imgui.h"
#include <string>

InterpolationCurveC2::InterpolationCurveC2() : SomeCurve()
{
	sprintf_s(name, STRMAX, ("InterpolationCurveC2 - " + std::to_string(idx++) + " " + gen_random(10,idx)).c_str());
	figureType = FigureType::InterpolationCurveC2;
	bernsteinLine = new PointsLine();
}

void InterpolationCurveC2::Initialize(Program* _program)
{
	SomeCurve::Initialize(_program);
	pointsLine->Initialize(program);
	bernsteinLine->Initialize(program);
}

void InterpolationCurveC2::Draw()
{
	Figure::Draw();
	glDrawElements(GL_LINES_ADJACENCY, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	if (drawLine)
		pointsLine->Draw();
	if (drawBernsteinLine)
		bernsteinLine->Draw();
	if (drawBernsteinPoints)
	{
		for (int i = 0; i < bernsteinPoints.size(); ++i)
		{
			bernsteinPoints[i]->Draw();
		}
	}
}

void InterpolationCurveC2::AddPoint(Point* point)
{
	points.push_back(point);
	first = true;
	pointsLine->AddPoint(point);
}

void InterpolationCurveC2::RemovePoint(int to_del)
{
	pointsLine->RemoveAt(to_del);
	points.erase(points.begin() + to_del);
	first = true;
}

void InterpolationCurveC2::CleanUp()
{
	for (int i = 0; i < points.size(); ++i)
		points[i]->Unpin(this);
	delete pointsLine;
	delete bernsteinLine;

	for (int i = 0; i < bernsteinPoints.size(); ++i)
	{
		delete bernsteinPoints[i];
	}
}

bool InterpolationCurveC2::Create()
{
	bool fCreate = Figure::Create();
	if (!fCreate && !first) return false;
	first = false;
	pointsLine->RecalcFigure();
	for (int i = 0; i < bernsteinPoints.size(); ++i) delete bernsteinPoints[i];
	bernsteinPoints.clear();
	bernsteinLine->Clear();
	vertices.clear();
	indices.clear();
	if (points.size() < 3) return false;

	std::vector<float> dist;
	std::vector<glm::vec3> a;
	int n = points.size();
	for (int i = 1; i < n; ++i)
	{
		a.push_back(points[i - 1]->GetPos());
		dist.push_back(glm::distance(points[i - 1]->GetPos(), points[i]->GetPos()));
	}
	a.push_back(points[n - 1]->GetPos());

	std::vector<float> alpha;
	std::vector<float> beta;
	std::vector<float> twos;
	std::vector<glm::vec3> R;
	for (int i = 1; i < n-1; ++i)
	{
		twos.push_back(2.0f);
		alpha.push_back(dist[i - 1] / (dist[i - 1] + dist[i]));
		beta.push_back(dist[i] / (dist[i - 1] + dist[i]));
		R.push_back(3.0f * ((a[i + 1] - a[i]) / dist[i] - (a[i] - a[i - 1]) / dist[i - 1]) / (dist[i - 1] + dist[i]));
	}

	std::vector<glm::vec3> b,c,d;
	c.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
	ThomasAlgorithm(alpha, twos, beta, R, c, n - 2);
	c.push_back(glm::vec3(0.0f, 0.0f, 0.0f));

	for (int i = 0; i < n - 1; ++i)
	{
		d.push_back((c[i + 1] - c[i]) / (3 * dist[i]));
		b.push_back((a[i + 1] - a[i]) / dist[i] - dist[i] * (c[i] + d[i] * dist[i]));
	}
	int k = 0;
	for (int i = 0; i < n-1; ++i)
	{
		glm::vec4 bernsteinPointss[4];
		glm::vec4 bernstein;
		glm::vec4 power = { a[i].x,b[i].x,c[i].x,d[i].x };
		PowerToBernsteinBasis(power, bernstein);
		bernstein = (1 - dist[i]) * bernstein + dist[i] * bernstein;
		bernstein[3] = (1 - dist[i]) * bernstein[2] + dist[i] * bernstein[3];
		bernstein[2] = (1 - dist[i]) * bernstein[1] + dist[i] * bernstein[2];
		bernstein[1] = (1 - dist[i]) * bernstein[0] + dist[i] * bernstein[1];
		bernstein[3] = (1 - dist[i]) * bernstein[2] + dist[i] * bernstein[3];
		bernstein[2] = (1 - dist[i]) * bernstein[1] + dist[i] * bernstein[2];
		bernstein[3] = (1 - dist[i]) * bernstein[2] + dist[i] * bernstein[3];

		bernsteinPointss[0].x = bernstein[0];
		bernsteinPointss[1].x = bernstein[1];
		bernsteinPointss[2].x = bernstein[2];
		bernsteinPointss[3].x = bernstein[3];

		power = { a[i].y,b[i].y,c[i].y,d[i].y };
		PowerToBernsteinBasis(power, bernstein);
		bernstein = (1 - dist[i]) * bernstein + dist[i] * bernstein;
		bernstein[3] = (1 - dist[i]) * bernstein[2] + dist[i] * bernstein[3];
		bernstein[2] = (1 - dist[i]) * bernstein[1] + dist[i] * bernstein[2];
		bernstein[1] = (1 - dist[i]) * bernstein[0] + dist[i] * bernstein[1];
		bernstein[3] = (1 - dist[i]) * bernstein[2] + dist[i] * bernstein[3];
		bernstein[2] = (1 - dist[i]) * bernstein[1] + dist[i] * bernstein[2];
		bernstein[3] = (1 - dist[i]) * bernstein[2] + dist[i] * bernstein[3];

		bernsteinPointss[0].y = bernstein[0];
		bernsteinPointss[1].y = bernstein[1];
		bernsteinPointss[2].y = bernstein[2];
		bernsteinPointss[3].y = bernstein[3];

		power = { a[i].z,b[i].z,c[i].z,d[i].z };
		PowerToBernsteinBasis(power, bernstein);
		bernstein = (1 - dist[i]) * bernstein + dist[i] * bernstein;
		bernstein[3] = (1 - dist[i]) * bernstein[2] + dist[i] * bernstein[3];
		bernstein[2] = (1 - dist[i]) * bernstein[1] + dist[i] * bernstein[2];
		bernstein[1] = (1 - dist[i]) * bernstein[0] + dist[i] * bernstein[1];
		bernstein[3] = (1 - dist[i]) * bernstein[2] + dist[i] * bernstein[3];
		bernstein[2] = (1 - dist[i]) * bernstein[1] + dist[i] * bernstein[2];
		bernstein[3] = (1 - dist[i]) * bernstein[2] + dist[i] * bernstein[3];

		bernsteinPointss[0].z = bernstein[0];
		bernsteinPointss[1].z = bernstein[1];
		bernsteinPointss[2].z = bernstein[2];
		bernsteinPointss[3].z = bernstein[3];
		
		bernsteinPointss[0].w = 1.0f;
		bernsteinPointss[1].w = 1.0f;
		bernsteinPointss[2].w = 1.0f;
		bernsteinPointss[3].w = 1.0f;


		float from = 0.0f;
		float to = 1.0f;
		int n = 0;
		for (int j = 0; j < 4; ++j)
		{
			auto pos = bernsteinPointss[j];
			Point* p = new Point(false);
			p->Initialize(program);
			p->MoveTo(pos.x, pos.y, pos.z);
			bernsteinPoints.push_back(p);
			bernsteinLine->AddPoint(p);
			p->virtualPoint = true;
		}
		for (int j = 1; j < 4 ; ++j)
		{
			glm::ivec3 pos_a = GetScreenPos(program, bernsteinPointss[j - 1]);
			glm::ivec3 pos_b = GetScreenPos(program, bernsteinPointss[j]);
			int x, y, z;
			x = pos_b.x - pos_a.x;
			y = pos_b.y - pos_a.y;
			z = pos_b.z - pos_a.z;
			n += sqrt(x * x + y * y + z * z);
		}
		if (n > 100000 || n < 0) n = 100000;

		for (int l = 0; l < n; l += 250)
		{
			from = (float)l / n;
			to = (float)(l + 250) / n;
			if (to > 1.0f)to = 1.0f;
			for (int j = 0; j < 4; ++j)
			{
				auto pos = bernsteinPointss[j];
				vertices.push_back(pos.x);
				vertices.push_back(pos.y);
				vertices.push_back(pos.z);
				if (k % 4 == 0)
				{
					vertices.push_back(0.0f);
					vertices.push_back(0.0f);
					vertices.push_back(1.0f);
				}
				else
				{
					vertices.push_back(0.0f);
					vertices.push_back(from);
					vertices.push_back(to);
				}
				indices.push_back(k);
				++k;
			}
		}
	}

	bernsteinLine->RecalcFigure();
	return true;


}

