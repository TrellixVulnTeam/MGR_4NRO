#include "Intersections.h"


void FixPos(float& u, float& v)
{
	while (u >= 1.0f) u -= 1.0f;
	while (v >= 1.0f) v -= 1.0f;
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

glm::vec3 GetDer(Figure* f, float u, float v, bool du)
{
	glm::vec3 pos;
	if (f->figureType == FigureType::BezierPatchC0)
	{
		pos = ((BezierPatchC0*)f)->GetParametrizedDer(u, v, du);
	}
	if (f->figureType == FigureType::BezierPatchC2)
	{
		pos = ((BezierPatchC2*)f)->GetParametrizedDer(u, v, du);
	}
	if (f->figureType == FigureType::Torus)
	{
		pos = ((Torus*)f)->GetParametrizedDer(u, v, du);
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

void NewtonMethod(Program* program, Figure* f1, Figure* f2, glm::vec3 P0, glm::vec4 x_k, glm::vec4& x_new, glm::vec3& pos1, glm::vec3& pos2, float dir)
{
	float d = 0.1f;
	x_new = x_k;
	P0 = GetPos(f1, x_k.x, x_k.y);
	glm::mat4x4 jacobian;
	glm::mat4x4 inv;
	glm::vec4 F;
	int i = 0;

	auto du1 = GetDer(f1, x_new.x, x_new.y, true);
	auto dv1 = GetDer(f1, x_new.x, x_new.y, false);

	auto du2 = GetDer(f2, x_new.z, x_new.w, true);
	auto dv2 = GetDer(f2, x_new.z, x_new.w, false);
	auto n1 = glm::cross(du1, dv1);
	auto n2 = glm::cross(du2, dv2);
	auto t = dir * glm::cross(n1, n2);
	pos1 = GetPos(f1, x_new.x, x_new.y);
	pos2 = GetPos(f2, x_new.z, x_new.w);
	glm::vec3 oldPos;
	do {
		x_k = x_new;

		du1 = GetDer(f1, x_new.x, x_new.y, true);
		dv1 = GetDer(f1, x_new.x, x_new.y, false);

		du2 = GetDer(f2, x_new.z, x_new.w, true);
		dv2 = GetDer(f2, x_new.z, x_new.w, false);

		jacobian[0][0] = du1.x;
		jacobian[1][0] = dv1.x;
		jacobian[2][0] = -du2.x;
		jacobian[3][0] = -dv2.x;

		jacobian[0][1] = du1.y;
		jacobian[1][1] = dv1.y;
		jacobian[2][1] = -du2.y;
		jacobian[3][1] = -dv2.y;

		jacobian[0][2] = du1.z;
		jacobian[1][2] = dv1.z;
		jacobian[2][2] = -du2.z;
		jacobian[3][2] = -dv2.z;

		jacobian[0][3] = glm::dot(du1, t);
		jacobian[1][3] = glm::dot(dv1, t);
		jacobian[2][3] = 0.0f;
		jacobian[3][3] = 0.0f;

		inv = glm::inverse(jacobian);

		F = { pos1 - pos2,dot(pos1 - P0,t) - d };

		x_new = x_k - inv * F;

		FixPos(x_new.x, x_new.y);
		FixPos(x_new.z, x_new.w);

		pos1 = GetPos(f1, x_new.x, x_new.y);
		pos2 = GetPos(f2, x_new.z, x_new.w);

		++i;
	} while (glm::distance(pos1, pos2) > 0.001f && i < 20);
}

void FindPointsLoop(Program* program, Figure* f1, Figure* f2, std::vector<std::pair<float, float>>& points1, std::vector<std::pair<float, float>>& points2, std::vector<glm::vec3>& points, float dir)
{
	float d = 1e-3;
	glm::mat4x4 jacobian;
	glm::vec4 last_vec, new_vec;
	glm::vec3 last_pos,first_pos;
	glm::vec3 pos1, pos2;
	last_vec.x = points1[0].first;
	last_vec.y = points1[0].second;
	last_vec.z = points2[0].first;
	last_vec.w = points2[0].second;
	last_pos = points[0];
	first_pos = points[0];
	while (true)
	{
		NewtonMethod(program, f1, f2, last_pos, last_vec, new_vec, pos1, pos2, dir);


		if (glm::distance(pos1, pos2) > 0.1f)
			break;


		last_vec = new_vec;
		points1.push_back(std::make_pair(new_vec.x, new_vec.y));
		points2.push_back(std::make_pair(new_vec.z, new_vec.w));
		last_pos = pos1;
		points.push_back(last_pos);
		if (glm::distance(pos1, first_pos) <d)
			break;
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

		std::vector<std::pair<float, float>> points1;
		points1.push_back(std::make_pair(u1, v1));
		std::vector<std::pair<float, float>> points2;
		points2.push_back(std::make_pair(u2, v2));
		std::vector<glm::vec3> points;
		points.push_back(GetPos(figures[0], u1, v1));

		FindPointsLoop(program, figures[0], figures[1], points1, points2, points, 1.0f);

		std::vector<std::pair<float, float>> ppoints1;
		ppoints1.push_back(std::make_pair(u1, v1));
		std::vector<std::pair<float, float>> ppoints2;
		ppoints2.push_back(std::make_pair(u2, v2));
		std::vector<glm::vec3> ppoints;
		ppoints.push_back(GetPos(figures[0], u1, v1));

		FindPointsLoop(program, figures[0], figures[1], ppoints1, ppoints2, ppoints, -1.0f);

		for (int i = 0; i < points.size(); ++i)
		{
			Point* p = new Point();
			p->Initialize(program);
			p->MoveTo(points[i].x, points[i].y, points[i].z);
			program->figures.push_back(p);
		}
		for (int i = 0; i < ppoints.size(); ++i)
		{
			Point* p = new Point();
			p->Initialize(program);
			p->MoveTo(ppoints[i].x, ppoints[i].y, ppoints[i].z);
			program->figures.push_back(p);
		}
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
			//if (glm::distance(glm::vec2(500, 500), glm::vec2(i, j)) < 400)
			//	w = 0;
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