#include "Intersections.h"


void FixPos(float& u, float& v)
{
	while (u >= 1.0f) u = fmod(u, 1.0f);
	while (v >= 1.0f) v = fmod(v, 1.0f);
	while (u < 0.0f) u += 1.0f;
	while (v < 0.0f) v += 1.0f;

}

glm::vec3 GetPos(Figure* f, float u, float v)
{
	glm::vec3 pos;
	if (f->figureType == FigureType::BezierPatchC0)
	{
		pos = ((BezierPatchC0*)f)->GetParametrizedPos(u, v);
	}
	if (f->figureType == FigureType::BezierPatchC2)
	{
		pos = ((BezierPatchC2*)f)->GetParametrizedPos(u, v);
	}
	if (f->figureType == FigureType::Torus)
	{
		pos = ((Torus*)f)->GetParametrizedPos(u, v);
	}
	return pos;
}

void BetterDist(Figure* f, float u, float v, float& u_better, float& v_better, float& d_better, glm::vec3& pos)
{
	FixPos(u, v);

	float d_new = glm::distance(pos, GetPos(f, u, v));
	if (d_new < d_better) { u_better = u; v_better = v; d_better = d_new; }
}

void GetClosestPoints(Program* program, Figure* f1, float& u1, float& v1, Figure* f2, float& u2, float& v2)
{
	if (program->useCursor)
	{
		FindPointClosestToCursor(program, f1, u1, v1);
		FindPointClosestToCursor(program, f2, u2, v2);
	}
	else
	{
		FindStartPoints(program, f1, u1, v1, f2, u2, v2);
	}
	FindClosestPoints(program, f1, u1, v1, f2, u2, v2);

	auto pos1 = GetPos(f1, u1, v1);
	auto pos2 = GetPos(f2, u2, v2);

	Point* p1 = new Point();
	Point* p2 = new Point();
	p1->Initialize(program);
	p2->Initialize(program);
	p1->MoveTo(pos1.x, pos1.y, pos1.z);
	p2->MoveTo(pos2.x, pos2.y, pos2.z);
	program->figures.push_back(p1);
	program->figures.push_back(p2);

}

void FindStartPoints(Program* program, Figure* f1, float& u1, float& v1, Figure* f2, float& u2, float& v2)
{
	int splits = 10;
	int stepSize = 1.0f / splits;
	float u = 0.0f, v = 0.0f;
	std::vector<std::pair<float, float>> points;

	for (int i = 0; i < splits; ++i)
	{
		v = 0.0f;
		for (int j = 0; j < splits; ++j)
		{
			points.push_back(std::make_pair<float, float>((float)u, (float)v));
			v += stepSize;
		}
		u += stepSize;
	}

	float bestDist = 99999.0f;

	for (int i = 0; i < points.size(); ++i)
	{
		for (int j = 0; j < points.size(); ++j)
		{
			float uu1 = points[i].first;
			float vv1 = points[i].second;
			float uu2 = points[j].first;
			float vv2 = points[j].second;
			FixPos(uu1, vv1);
			FixPos(uu2, vv2);
			float d = glm::distance(GetPos(f1, uu1, vv1), GetPos(f2, uu2, vv2));
			if (d < bestDist)
			{
				bestDist = d;
				u1 = uu1;
				v1 = vv1;
				u2 = uu2;
				v2 = vv2;
			}
		}
	}

}

void FindClosestPoints(Program* program, Figure* f1, float& u1, float& v1, Figure* f2, float& u2, float& v2)
{
	float windowSize = 1e-1;

	std::vector<std::pair<float, float>> points1;
	std::vector<std::pair<float, float>> points2;
	bool better;

	float bestDist = glm::distance(GetPos(f1, u1, v1), GetPos(f2, u2, v2));
	while (windowSize > 1e-7)
	{
		points1.clear();
		points2.clear();
		better = false;

		points1.push_back(std::make_pair<float, float>(u1 - windowSize, v1 - windowSize));
		points1.push_back(std::make_pair<float, float>(u1 - windowSize, (float)v1));
		points1.push_back(std::make_pair<float, float>(u1 - windowSize, v1 + windowSize));
		points1.push_back(std::make_pair<float, float>((float)u1, v1 - windowSize));
		points1.push_back(std::make_pair<float, float>((float)u1, v1 + windowSize));
		points1.push_back(std::make_pair<float, float>(u1 + windowSize, v1 - windowSize));
		points1.push_back(std::make_pair<float, float>(u1 + windowSize, (float)v1));
		points1.push_back(std::make_pair<float, float>(u1 + windowSize, v1 + windowSize));

		points2.push_back(std::make_pair<float, float>(u2 - windowSize, v2 - windowSize));
		points2.push_back(std::make_pair<float, float>(u2 - windowSize, (float)v2));
		points2.push_back(std::make_pair<float, float>(u2 - windowSize, v2 + windowSize));
		points2.push_back(std::make_pair<float, float>((float)u2, v2 - windowSize));
		points2.push_back(std::make_pair<float, float>((float)u2, v2 + windowSize));
		points2.push_back(std::make_pair<float, float>(u2 + windowSize, v2 - windowSize));
		points2.push_back(std::make_pair<float, float>(u2 + windowSize, (float)v2));
		points2.push_back(std::make_pair<float, float>(u2 + windowSize, v2 + windowSize));

		for (int i = 0; i < points1.size(); ++i)
			for (int j = 0; j < points2.size(); ++j)
			{
				float uu1 = points1[i].first;
				float vv1 = points1[i].second;
				float uu2 = points2[j].first;
				float vv2 = points2[j].second;
				FixPos(uu1, vv1);
				FixPos(uu2, vv2);

				float d = glm::distance(GetPos(f1, uu1, vv1), GetPos(f2, uu2, vv2));
				if (d < bestDist)
				{
					better = true;
					bestDist = d;
					u1 = uu1;
					u2 = uu2;
					v1 = vv1;
					v2 = vv2;
				}
			}

		if (!better) windowSize /= 2;
	}
}

void FindPointClosestToCursor(Program* program, Figure* f, float& u_best, float& v_best)
{
	auto pos = program->cur->GetPos();

	u_best = 0.0f;
	v_best = 0.0f;

	int surfSplit = 10;
	if (f->figureType == FigureType::Torus) surfSplit = 20;
	float surfStep = 1.0f / surfSplit;
	float searchStepStart = surfStep / 10;
	float d_best = glm::distance(pos, GetPos(f, u_best, v_best));
	float u_start = 0.0f;
	for (int i = 0; i <= surfSplit; ++i)
	{
		float v_start = 0.0f;
		for (int j = 0; j <= surfSplit; ++j)
		{
			float u = u_start;
			float v = v_start;
			FixPos(u, v);
			float dist = glm::distance(pos, GetPos(f, u, v));
			float searchStep = searchStepStart;

			while (searchStep >= 1e-4)
			{
				float u_better, u_new;
				float v_better, v_new;
				float d_better = 9999.0f;

				u_new = u + searchStep;
				v_new = v + searchStep;
				BetterDist(f, u_new, v_new, u_better, v_better, d_better, pos);

				u_new = u + searchStep;
				v_new = v;
				BetterDist(f, u_new, v_new, u_better, v_better, d_better, pos);

				u_new = u + searchStep;
				v_new = v - searchStep;
				BetterDist(f, u_new, v_new, u_better, v_better, d_better, pos);

				u_new = u;
				v_new = v + searchStep;
				BetterDist(f, u_new, v_new, u_better, v_better, d_better, pos);

				u_new = u;
				v_new = v - searchStep;
				BetterDist(f, u_new, v_new, u_better, v_better, d_better, pos);

				u_new = u - searchStep;
				v_new = v + searchStep;
				BetterDist(f, u_new, v_new, u_better, v_better, d_better, pos);

				u_new = u - searchStep;
				v_new = v;
				BetterDist(f, u_new, v_new, u_better, v_better, d_better, pos);

				u_new = u - searchStep;
				v_new = v - searchStep;
				BetterDist(f, u_new, v_new, u_better, v_better, d_better, pos);

				if (d_better < dist)
				{
					u = u_better;
					v = v_better;
					dist = d_better;
				}
				else
				{
					searchStep /= 2;
				}
			}

			if (dist < d_best)
			{
				u_best = u;
				v_best = v;
				d_best = dist;
			}

			v_start += surfStep;
		}
		u_start += surfStep;
	}

}

void Intersect(Program* program)
{
	int n = program->figures.size();

	std::vector<Figure*> figures;

	for (int k = 0; k < n; ++k)
	{
		Figure* f = program->figures[k];
		if (f->GetSelected())
			if (
				f->figureType == FigureType::BezierPatchC0 ||
				f->figureType == FigureType::BezierPatchC2 ||
				f->figureType == FigureType::Torus
				)
			{
				figures.push_back(f);
			}
	}

	if (figures.size() == 2)
	{
		float u1, v1, u2, v2;
		GetClosestPoints(program, figures[0], u1, v1, figures[1], u2, v2);
	}
	else
	{
		std::cout << "Should be 2 figures selected";
	}
}

void FillImage(unsigned int& texName)
{
	int i, j, c;
	for (i = 0; i < checkImageHeight; i++) {
		for (j = 0; j < checkImageWidth; j++) {
			int w = 255;
			if (glm::distance(glm::vec2(500, 500), glm::vec2(i, j)) < 400)
				w = 0;
			checkImage[i][j][0] = (GLubyte)w;
			checkImage[i][j][1] = (GLubyte)w;
			checkImage[i][j][2] = (GLubyte)w;
			checkImage[i][j][3] = (GLubyte)w;
		}
	}

	glGenTextures(1, &texName);
	glBindTexture(GL_TEXTURE_2D, texName);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
		GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
		GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, checkImageWidth,
		checkImageHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE,
		checkImage);
}