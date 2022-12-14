#include "Intersections.h"
#include "IntersectionLine.h"
#include <list>

void FixPos(float& u, float& v)
{
	while (u >= 1.0f) u -= 1.0f;
	while (v >= 1.0f) v -= 1.0f;
	while (u < 0.0f) u += 1.0f;
	while (v < 0.0f) v += 1.0f;

}
void FixPos2(float& u, float& v, float u0, float v0)
{
	while (u >= 1.0f) u = u0;
	while (v >= 1.0f) v = v0;
	while (u < 0.0f) u = u0;
	while (v < 0.0f) v = v0;

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

glm::vec3 GetToolPos(Figure* f, float u, float v, int normDir)
{
	float r = 4.0f;
	glm::vec3 pos = GetPos(f, u, v);

	auto der1 = glm::normalize(GetDer(f, u, v, true));
	auto der2 = glm::normalize(GetDer(f, u, v, false));
	auto norm = (float)normDir * glm::normalize(glm::cross(der1, der2));

	auto mid = pos + r * norm;
	mid.y = mid.y - r;
	return mid;
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


glm::vec3 GetToolDer(Figure* f, float u, float v, bool du, int normDir)
{
	//	return GetDer(f, u, v, du);
	float u0 = u, v0 = v;
	float eps = 1e-3;
	glm::vec3 pos1;
	glm::vec3 pos2;
	if (du)
		u = u0 + eps;
	else
		v = v0 + eps;

	FixPos(u, v);
	pos1 = GetToolPos(f, u, v, normDir);

	if (du)
		u = u0 - eps;
	else
		v = v0 - eps;
	FixPos(u, v);
	pos2 = GetToolPos(f, u, v, normDir);

	auto der = pos1 - pos2;
	float inv = 0.5f / eps;
	return inv * der;
}

void BetterDist(Figure* f, float u, float v, float& u_better, float& v_better, float& d_better, glm::vec3& pos)
{
	FixPos(u, v);

	float d_new = glm::distance(pos, GetPos(f, u, v));
	if (d_new < d_better) { u_better = u; v_better = v; d_better = d_new; }
}

void BetterDistTool(Figure* f, float u, float v, float& u_better, float& v_better, float& d_better, glm::vec3& pos, int normDir)
{
	FixPos(u, v);

	float d_new = glm::distance(pos, GetToolPos(f, u, v, normDir));
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

void GetClosestPointsTool(Program* program, Figure* f1, float& u1, float& v1, Figure* f2, float& u2, float& v2, int normDir1, int normDir2)
{
	if (program->useCursor)
	{
		FindPointClosestToCursorTool(program, f1, u1, v1, normDir1);
		FindPointClosestToCursorTool(program, f2, u2, v2, normDir2);
	}
	else
	{
		FindStartPointsTool(program, f1, u1, v1, f2, u2, v2, normDir1, normDir2);
	}
	FindClosestPointsTool(program, f1, u1, v1, f2, u2, v2, normDir1, normDir2);

	auto pos1 = GetToolPos(f1, u1, v1, normDir1);
	auto pos2 = GetToolPos(f2, u2, v2, normDir2);
}

void FindStartPoints(Program* program, Figure* f1, float& u1, float& v1, Figure* f2, float& u2, float& v2)
{
	int splits = program->startPointsSplits;
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


void FindStartPointsTool(Program* program, Figure* f1, float& u1, float& v1, Figure* f2, float& u2, float& v2, int normDir1, int normDir2)
{
	int splits = program->startPointsSplits;
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
			float d = glm::distance(GetToolPos(f1, uu1, vv1, normDir1), GetToolPos(f2, uu2, vv2, normDir2));
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
	float windowSize = program->closestPointsStartWindowSize;

	std::vector<std::pair<float, float>> points1;
	std::vector<std::pair<float, float>> points2;
	bool better;

	float bestDist = glm::distance(GetPos(f1, u1, v1), GetPos(f2, u2, v2));
	while (windowSize > 1e-4)
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

void FindClosestPointsTool(Program* program, Figure* f1, float& u1, float& v1, Figure* f2, float& u2, float& v2, int normDir1, int normDir2)
{
	float windowSize = program->closestPointsStartWindowSize;

	std::vector<std::pair<float, float>> points1;
	std::vector<std::pair<float, float>> points2;
	bool better;

	float bestDist = glm::distance(GetToolPos(f1, u1, v1, normDir1), GetToolPos(f2, u2, v2, normDir2));
	while (windowSize > 1e-4)
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

				float d = glm::distance(GetToolPos(f1, uu1, vv1, normDir1), GetToolPos(f2, uu2, vv2, normDir2));
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

	int surfSplit = program->cursorPointsSplits;
	if (f->figureType == FigureType::Torus && surfSplit < 20) surfSplit = 20;
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

void FindPointClosestToCursorTool(Program* program, Figure* f, float& u_best, float& v_best, int normDir)
{
	auto pos = program->cur->GetPos();

	u_best = 0.0f;
	v_best = 0.0f;

	int surfSplit = program->cursorPointsSplits;
	if (f->figureType == FigureType::Torus && surfSplit < 20) surfSplit = 20;
	float surfStep = 1.0f / surfSplit;
	float searchStepStart = surfStep / 10;
	float d_best = glm::distance(pos, GetToolPos(f, u_best, v_best, normDir));
	float u_start = 0.0f;
	for (int i = 0; i <= surfSplit; ++i)
	{
		float v_start = 0.0f;
		for (int j = 0; j <= surfSplit; ++j)
		{
			float u = u_start;
			float v = v_start;
			FixPos(u, v);
			float dist = glm::distance(pos, GetToolPos(f, u, v, normDir));
			float searchStep = searchStepStart;

			while (searchStep >= 1e-4)
			{
				float u_better, u_new;
				float v_better, v_new;
				float d_better = 9999.0f;

				u_new = u + searchStep;
				v_new = v + searchStep;
				BetterDistTool(f, u_new, v_new, u_better, v_better, d_better, pos, normDir);

				u_new = u + searchStep;
				v_new = v;
				BetterDistTool(f, u_new, v_new, u_better, v_better, d_better, pos, normDir);

				u_new = u + searchStep;
				v_new = v - searchStep;
				BetterDistTool(f, u_new, v_new, u_better, v_better, d_better, pos, normDir);

				u_new = u;
				v_new = v + searchStep;
				BetterDistTool(f, u_new, v_new, u_better, v_better, d_better, pos, normDir);

				u_new = u;
				v_new = v - searchStep;
				BetterDistTool(f, u_new, v_new, u_better, v_better, d_better, pos, normDir);

				u_new = u - searchStep;
				v_new = v + searchStep;
				BetterDistTool(f, u_new, v_new, u_better, v_better, d_better, pos, normDir);

				u_new = u - searchStep;
				v_new = v;
				BetterDistTool(f, u_new, v_new, u_better, v_better, d_better, pos, normDir);

				u_new = u - searchStep;
				v_new = v - searchStep;
				BetterDistTool(f, u_new, v_new, u_better, v_better, d_better, pos, normDir);

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
	float d = program->newtonD;
	x_new = x_k;
	P0 = GetPos(f1, x_k.x, x_k.y);
	glm::mat4x4 jacobian;
	glm::mat4x4 inv;
	glm::vec4 F;
	int i = 0;

	auto du1 = glm::normalize(GetDer(f1, x_new.x, x_new.y, true));
	auto dv1 = glm::normalize(GetDer(f1, x_new.x, x_new.y, false));

	auto du2 = glm::normalize(GetDer(f2, x_new.z, x_new.w, true));
	auto dv2 = glm::normalize(GetDer(f2, x_new.z, x_new.w, false));
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
	} while (glm::distance(pos1, pos2) > program->pointsDistNewton && i < 20);
}

void NewtonMethodTool(Program* program, Figure* f1, Figure* f2, glm::vec3 P0, glm::vec4 x_k, glm::vec4& x_new, glm::vec3& pos1, glm::vec3& pos2, float dir, int normDir1, int normDir2)
{
	float d = program->newtonD;
	x_new = x_k;
	P0 = GetToolPos(f1, x_k.x, x_k.y, normDir1);
	auto P0test = GetToolPos(f2, x_k.z, x_k.w, normDir2);
	glm::mat4x4 jacobian;
	glm::mat4x4 inv;
	glm::vec4 F;
	int i = 0;

	auto du1 = glm::normalize(GetToolDer(f1, x_new.x, x_new.y, true, normDir1));
	auto dv1 = glm::normalize(GetToolDer(f1, x_new.x, x_new.y, false, normDir1));

	auto du2 = glm::normalize(GetToolDer(f2, x_new.z, x_new.w, true, normDir2));
	auto dv2 = glm::normalize(GetToolDer(f2, x_new.z, x_new.w, false, normDir2));
	auto n1 = glm::cross(du1, dv1);
	auto n2 = glm::cross(du2, dv2);
	auto t = dir * glm::cross(n1, n2);
	pos1 = GetToolPos(f1, x_new.x, x_new.y, normDir1);
	pos2 = GetToolPos(f2, x_new.z, x_new.w, normDir2);
	glm::vec3 oldPos;
	do {
		x_k = x_new;

		du1 = GetToolDer(f1, x_new.x, x_new.y, true, normDir1);
		dv1 = GetToolDer(f1, x_new.x, x_new.y, false, normDir1);

		du2 = GetToolDer(f2, x_new.z, x_new.w, true, normDir2);
		dv2 = GetToolDer(f2, x_new.z, x_new.w, false, normDir2);

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

		pos1 = GetToolPos(f1, x_new.x, x_new.y, normDir1);
		pos2 = GetToolPos(f2, x_new.z, x_new.w, normDir2);

		++i;
	} while (glm::distance(pos1, pos2) > program->pointsDistNewton && i < 20);
}

void FindPointsLoop(Program* program, Figure* f1, Figure* f2, std::vector<glm::vec2>& points1, std::vector<glm::vec2>& points2, std::vector<glm::vec3>& points, float dir)
{
	float d = program->stopSearchingPointsLoopD;
	glm::mat4x4 jacobian;
	glm::vec4 last_vec, new_vec;
	glm::vec3 last_pos, first_pos;
	glm::vec3 pos1, pos2;
	last_vec.x = points1[0].x;
	last_vec.y = points1[0].y;
	last_vec.z = points2[0].x;
	last_vec.w = points2[0].y;
	last_pos = points[0];
	first_pos = points[0];
	while (true)
	{
		NewtonMethod(program, f1, f2, last_pos, last_vec, new_vec, pos1, pos2, dir);


		if (glm::distance(pos1, pos2) > program->stopSearchingPointsFarD)
			break;


		last_vec = new_vec;
		points1.push_back(glm::vec2{ new_vec.x, new_vec.y });
		points2.push_back(glm::vec2{ new_vec.z, new_vec.w });
		last_pos = pos1;
		points.push_back(last_pos);
		if (points.size() > 100 && glm::distance(pos1, first_pos) < d)
			break;
	}
}

void FindPointsLoopTool(Program* program, Figure* f1, Figure* f2, std::vector<glm::vec2>& points1, std::vector<glm::vec2>& points2, std::vector<glm::vec3>& points, float dir, int normDir1, int normDir2)
{
	float d = program->stopSearchingPointsLoopD;
	glm::mat4x4 jacobian;
	glm::vec4 last_vec, new_vec;
	glm::vec3 last_pos, first_pos;
	glm::vec3 pos1, pos2;
	last_vec.x = points1[0].x;
	last_vec.y = points1[0].y;
	last_vec.z = points2[0].x;
	last_vec.w = points2[0].y;
	last_pos = points[0];
	first_pos = points[0];
	while (true)
	{
		NewtonMethodTool(program, f1, f2, last_pos, last_vec, new_vec, pos1, pos2, dir, normDir1, normDir2);


		if (glm::distance(pos1, pos2) > program->stopSearchingPointsFarD)
			break;

		last_vec = new_vec;
		points1.push_back(glm::vec2{ new_vec.x, new_vec.y });
		points2.push_back(glm::vec2{ new_vec.z, new_vec.w });
		last_pos = pos1;
		points.push_back(last_pos);
		if (points.size() > 10 && glm::distance(pos1, first_pos) < d)
			break;
		if (points.size() > 10 && glm::distance(pos1, points[points.size() - 10]) < 0.5f * program->pointsDistNewton)
			break;
	}
}

std::vector<glm::vec2> PrepareToDraw(Program* program, std::vector<glm::vec2> vec)
{
	std::vector<glm::vec2> toDraw;

	for (int i = 0; i < vec.size() - 1; ++i)
	{
		if (glm::distance(vec[i], vec[i + 1]) < 0.5f)
		{
			toDraw.push_back(vec[i]);
			toDraw.push_back(vec[i + 1]);
		}
		else
		{
			float u0 = vec[i].x;
			float u1 = 1.0f - vec[i].x;
			float v0 = vec[i].y;
			float v1 = 1.0f - vec[i].y;
			toDraw.push_back(vec[i]);
			if (u0 <= u1 && u0 <= v0 && u0 <= v1) toDraw.push_back(glm::vec2{ 0.0f, vec[i].y });
			else if (u1 <= u0 && u1 <= v0 && u1 <= v1) toDraw.push_back(glm::vec2{ 1.0f, vec[i].y });
			else if (v0 <= u1 && v0 <= u0 && v0 <= v1) toDraw.push_back(glm::vec2{ vec[i].x, 0.0f });
			else toDraw.push_back(glm::vec2{ vec[i].x,1.0f });


			u0 = vec[i + 1].x;
			u1 = 1.0f - vec[i + 1].x;
			v0 = vec[i + 1].y;
			v1 = 1.0f - vec[i + 1].y;
			if (u0 <= u1 && u0 <= v0 && u0 <= v1) toDraw.push_back(glm::vec2{ 0.0f, vec[i + 1].y });
			else if (u1 <= u0 && u1 <= v0 && u1 <= v1) toDraw.push_back(glm::vec2{ 1.0f, vec[i + 1].y });
			else if (v0 <= u1 && v0 <= u0 && v0 <= v1) toDraw.push_back(glm::vec2{ vec[i + 1].x, 0.0f });
			else toDraw.push_back(glm::vec2{ vec[i + 1].x,1.0f });
			toDraw.push_back(vec[i + 1]);
		}
	}

	auto first = vec[0];
	auto last = vec[vec.size() - 1];

	float eps = program->connectLineToWallEps;

	if (glm::distance(first, last) < 0.01f)
	{
		toDraw.push_back(first);
		toDraw.push_back(last);
	}
	else
	{
		float u0 = first.x;
		float u1 = 1.0f - first.x;
		float v0 = first.y;
		float v1 = 1.0f - first.y;
		if (u0 <= u1 && u0 <= v0 && u0 <= v1 && u0 < eps)
		{
			toDraw.push_back(first);
			toDraw.push_back(glm::vec2{ 0.0f, first.y });
		}
		else if (u1 <= u0 && u1 <= v0 && u1 <= v1 && u1 < eps)
		{
			toDraw.push_back(first);
			toDraw.push_back(glm::vec2{ 1.0f, first.y });
		}
		else if (v0 <= u1 && v0 <= u0 && v0 <= v1 && v0 < eps)
		{
			toDraw.push_back(first);
			toDraw.push_back(glm::vec2{ first.x, 0.0f });
		}
		else if (v1 < eps)
		{
			toDraw.push_back(first);
			toDraw.push_back(glm::vec2{ first.x,1.0f });
		}


		u0 = last.x;
		u1 = 1.0f - last.x;
		v0 = last.y;
		v1 = 1.0f - last.y;
		if (u0 <= u1 && u0 <= v0 && u0 <= v1 && u0 < eps)
		{
			toDraw.push_back(glm::vec2{ 0.0f, last.y });
			toDraw.push_back(last);
		}
		else if (u1 <= u0 && u1 <= v0 && u1 <= v1 && u1 < eps)
		{
			toDraw.push_back(glm::vec2{ 1.0f, last.y });
			toDraw.push_back(last);
		}
		else if (v0 <= u1 && v0 <= u0 && v0 <= v1 && v0 < eps)
		{
			toDraw.push_back(glm::vec2{ last.x, 0.0f });
			toDraw.push_back(last);
		}
		else if (v1 < eps)
		{
			toDraw.push_back(glm::vec2{ last.x,1.0f });
			toDraw.push_back(last);
		}

	}
	return toDraw;
}

void SetTexture(unsigned int& sourceTex, unsigned int& destTex, bool fill, bool inverse, bool wrapParametrizationFloodFill, Program* program)
{
	glBindTexture(GL_TEXTURE_2D, sourceTex);
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, checkImage);
	glBindTexture(GL_TEXTURE_2D, destTex);

	if (fill)FloodFill(wrapParametrizationFloodFill);
	if (inverse) Inverse();

	for (int i = 0; i < checkImageHeight; ++i)
		for (int j = 0; j < checkImageWidth; ++j)
		{
			if (destTex == program->texRL)
			{
				if (checkImage[i][j][0] == 0 || totalImageRL[i][j][0] == 0)
				{
					checkImage[i][j][0] = 0;
					checkImage[i][j][1] = 0;
					checkImage[i][j][2] = 0;
					totalImageRL[i][j][0] = 0;
					totalImageRL[i][j][1] = 0;
					totalImageRL[i][j][2] = 0;
				}
			}
			if (destTex == program->texRP)
			{
				if (checkImage[i][j][0] == 0 || totalImageRP[i][j][0] == 0)
				{
					checkImage[i][j][0] = 0;
					checkImage[i][j][1] = 0;
					checkImage[i][j][2] = 0;
					totalImageRP[i][j][0] = 0;
					totalImageRP[i][j][1] = 0;
					totalImageRP[i][j][2] = 0;
				}
			}
			if (destTex == program->texSL)
			{
				if (checkImage[i][j][0] == 0 || totalImageSL[i][j][0] == 0)
				{
					checkImage[i][j][0] = 0;
					checkImage[i][j][1] = 0;
					checkImage[i][j][2] = 0;
					totalImageSL[i][j][0] = 0;
					totalImageSL[i][j][1] = 0;
					totalImageSL[i][j][2] = 0;
				}
			}
			if (destTex == program->texSP)
			{
				if (checkImage[i][j][0] == 0 || totalImageSP[i][j][0] == 0)
				{
					checkImage[i][j][0] = 0;
					checkImage[i][j][1] = 0;
					checkImage[i][j][2] = 0;
					totalImageSP[i][j][0] = 0;
					totalImageSP[i][j][1] = 0;
					totalImageSP[i][j][2] = 0;
				}
			}
			if (destTex == program->texT)
			{
				if (checkImage[i][j][0] == 0 || totalImageT[i][j][0] == 0)
				{
					checkImage[i][j][0] = 0;
					checkImage[i][j][1] = 0;
					checkImage[i][j][2] = 0;
					totalImageT[i][j][0] = 0;
					totalImageT[i][j][1] = 0;
					totalImageT[i][j][2] = 0;
				}
			}
			if (destTex == program->texP)
			{
				if (checkImage[i][j][0] == 0 || totalImageP[i][j][0] == 0)
				{
					checkImage[i][j][0] = 0;
					checkImage[i][j][1] = 0;
					checkImage[i][j][2] = 0;
					totalImageP[i][j][0] = 0;
					totalImageP[i][j][1] = 0;
					totalImageP[i][j][2] = 0;
				}
			}
		}
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, checkImageWidth,
		checkImageHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE,
		checkImage);
}

void DrawPointsToTex(Program* program, std::vector<glm::vec2>& points, unsigned int& framebuffer, unsigned int& texBuffer)
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glViewport(0, 0, checkImageWidth, checkImageHeight);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	program->shader.use();

	glm::mat4 persp = glm::mat4(1.0f);
	glm::mat4 view = glm::mat4(1.0f);
	glm::mat4 trans = glm::mat4(1.0f);
	unsigned int perspLoc = glGetUniformLocation(program->shader.ID, "persp");
	unsigned int viewLoc = glGetUniformLocation(program->shader.ID, "view");
	int transLoc = glGetUniformLocation(program->shader.ID, "transform");

	glUniformMatrix4fv(perspLoc, 1, GL_FALSE, glm::value_ptr(persp));
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(transLoc, 1, GL_FALSE, glm::value_ptr(trans));
	unsigned int VBO;
	unsigned int VAO;
	unsigned int EBO;
	glGenBuffers(1, &EBO);
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	std::vector<float> vertices;
	std::vector<unsigned int> indices;

	for (int i = 0; i < points.size(); ++i)
	{
		vertices.push_back(2.0f * (points[i].x - 0.5f));
		vertices.push_back(2.0f * (points[i].y - 0.5f));
		vertices.push_back(0.0f);
		vertices.push_back(0.0f);
		vertices.push_back(0.0f);
		vertices.push_back(0.0f);
		indices.push_back(i);
	}

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

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glBindVertexArray(0);
	glBindVertexArray(VAO);
	glDrawElements(GL_LINES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	glViewport(0, 0, DEFAULT_WIDTH, DEFAULT_HEIGHT);
}

std::vector<glm::vec3> Intersect2(Program* program, float dir)
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

	std::vector<glm::vec3> points;
	if (figures.size() == 2)
	{
		float u1, v1, u2, v2;
		GetClosestPoints(program, figures[0], u1, v1, figures[1], u2, v2);

		std::vector<glm::vec2> points1;
		points1.push_back(glm::vec2{ u1, v1 });
		std::vector<glm::vec2> points2;
		points2.push_back(glm::vec2{ u2, v2 });
		points.push_back(GetPos(figures[0], u1, v1));

		FindPointsLoop(program, figures[0], figures[1], points1, points2, points, dir);
	}
	return points;
}

std::vector<glm::vec3> Intersect2Tool(Program* program, float dir, int normDir1, int normDir2, unsigned int tex1, unsigned int tex2,std::vector<glm::vec2>& points1, std::vector<glm::vec2>& points2)
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

	std::vector<glm::vec3> points;
	if (figures.size() == 2)
	{
		float u1, v1, u2, v2;
		GetClosestPointsTool(program, figures[0], u1, v1, figures[1], u2, v2, normDir1, normDir2);

		points1.push_back(glm::vec2{ u1, v1 });
		points2.push_back(glm::vec2{ u2, v2 });
		points.push_back(GetToolPos(figures[0], u1, v1, normDir1));

		FindPointsLoopTool(program, figures[0], figures[1], points1, points2, points, dir, normDir1, normDir2);

		if (tex1 > 0 || tex2 > 0)
		{
			std::vector<glm::vec2> finalPoints1, finalPoints2;

			for (int i = points.size() - 1; i >= 0; --i)
			{
				Point* p = new Point();
				finalPoints1.push_back(points1[i]);
				finalPoints2.push_back(points2[i]);
			}

			points1.clear();
			points2.clear();

			points1 = PrepareToDraw(program, finalPoints1);
			points2 = PrepareToDraw(program, finalPoints2);

			CreateTexBuffer(program->testFrame, program->testTex2);
			DrawPointsToTex(program, points1, program->testFrame, program->testTex2);
			SetTexture(program->testTex2, tex1, false, false, false, program);
			DrawPointsToTex(program, points2, program->testFrame, program->testTex2);
			SetTexture(program->testTex2, tex2, false, false, false, program);
		}
	}
	return points;
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

		std::vector<glm::vec2> points1;
		points1.push_back(glm::vec2{ u1, v1 });
		std::vector<glm::vec2> points2;
		points2.push_back(glm::vec2{ u2, v2 });
		std::vector<glm::vec3> points;
		points.push_back(GetPos(figures[0], u1, v1));

		FindPointsLoop(program, figures[0], figures[1], points1, points2, points, 1.0f);

		std::vector<glm::vec2> ppoints1;
		ppoints1.push_back(glm::vec2{ u1, v1 });
		std::vector<glm::vec2> ppoints2;
		ppoints2.push_back(glm::vec2{ u2, v2 });
		std::vector<glm::vec3> ppoints;
		ppoints.push_back(GetPos(figures[0], u1, v1));

		FindPointsLoop(program, figures[0], figures[1], ppoints1, ppoints2, ppoints, -1.0f);

		IntersectionLine* il = new IntersectionLine();
		il->Initialize(program);

		std::vector<glm::vec2> finalPoints1;
		std::vector<glm::vec2> finalPoints2;

		for (int i = points.size() - 1; i >= 0; --i)
		{
			Point* p = new Point();
			p->Initialize(program);
			p->MoveTo(points[i].x, points[i].y, points[i].z);
			program->figures.push_back(p);
			p->AddParent(il);
			il->AddPoint(p);
			finalPoints1.push_back(points1[i]);
			finalPoints2.push_back(points2[i]);
		}
		for (int i = 0; i < ppoints.size(); ++i)
		{
			Point* p = new Point();
			p->Initialize(program);
			p->MoveTo(ppoints[i].x, ppoints[i].y, ppoints[i].z);
			program->figures.push_back(p);
			p->AddParent(il);
			il->AddPoint(p);
			finalPoints1.push_back(ppoints1[i]);
			finalPoints2.push_back(ppoints2[i]);
		}
		program->figures.push_back(il);

		points1.clear();
		points2.clear();

		points1 = PrepareToDraw(program, finalPoints1);
		points2 = PrepareToDraw(program, finalPoints2);

		CreateTexBuffer(program->testFrame, program->testTex2);

		DrawPointsToTex(program, points1, program->testFrame, program->testTex2);
		SetTexture(program->testTex2, figures[0]->trimTex, true, false, figures[0]->wrapParametrizationFloodFill, program);
		SetTexture(program->testTex2, figures[0]->trimLine, false, false, figures[0]->wrapParametrizationFloodFill, program);

		DrawPointsToTex(program, points2, program->testFrame, program->testTex2);
		SetTexture(program->testTex2, figures[1]->trimTex, true, false, figures[1]->wrapParametrizationFloodFill, program);
		SetTexture(program->testTex2, figures[1]->trimLine, false, false, figures[1]->wrapParametrizationFloodFill, program);
	}
}

void IntersectTool(Program* program, int normDir1, int normDir2)
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
		GetClosestPointsTool(program, figures[0], u1, v1, figures[1], u2, v2, normDir1, normDir2);

		std::vector<glm::vec2> points1;
		points1.push_back(glm::vec2{ u1, v1 });
		std::vector<glm::vec2> points2;
		points2.push_back(glm::vec2{ u2, v2 });
		std::vector<glm::vec3> points;
		points.push_back(GetPos(figures[0], u1, v1));

		FindPointsLoopTool(program, figures[0], figures[1], points1, points2, points, 1.0f, normDir1, normDir2);

		std::vector<glm::vec2> ppoints1;
		ppoints1.push_back(glm::vec2{ u1, v1 });
		std::vector<glm::vec2> ppoints2;
		ppoints2.push_back(glm::vec2{ u2, v2 });
		std::vector<glm::vec3> ppoints;
		ppoints.push_back(GetPos(figures[0], u1, v1));

		FindPointsLoopTool(program, figures[0], figures[1], ppoints1, ppoints2, ppoints, -1.0f, normDir1, normDir2);

		IntersectionLine* il = new IntersectionLine();
		il->Initialize(program);

		std::vector<glm::vec2> finalPoints1;
		std::vector<glm::vec2> finalPoints2;

		for (int i = points.size() - 1; i >= 0; --i)
		{
			Point* p = new Point();
			p->Initialize(program);
			p->MoveTo(points[i].x, points[i].y, points[i].z);
			program->figures.push_back(p);
			p->AddParent(il);
			il->AddPoint(p);
			finalPoints1.push_back(points1[i]);
			finalPoints2.push_back(points2[i]);
		}
		for (int i = 0; i < ppoints.size(); ++i)
		{
			Point* p = new Point();
			p->Initialize(program);
			p->MoveTo(ppoints[i].x, ppoints[i].y, ppoints[i].z);
			program->figures.push_back(p);
			p->AddParent(il);
			il->AddPoint(p);
			finalPoints1.push_back(ppoints1[i]);
			finalPoints2.push_back(ppoints2[i]);
		}
		program->figures.push_back(il);

		points1.clear();
		points2.clear();

		points1 = PrepareToDraw(program, finalPoints1);
		points2 = PrepareToDraw(program, finalPoints2);

		CreateTexBuffer(program->testFrame, program->testTex2);

		DrawPointsToTex(program, points1, program->testFrame, program->testTex2);
		SetTexture(program->testTex2, figures[0]->trimTex, true, false, figures[0]->wrapParametrizationFloodFill, program);
		SetTexture(program->testTex2, figures[0]->trimLine, false, false, figures[0]->wrapParametrizationFloodFill, program);

		DrawPointsToTex(program, points2, program->testFrame, program->testTex2);
		SetTexture(program->testTex2, figures[1]->trimTex, true, false, figures[1]->wrapParametrizationFloodFill, program);
		SetTexture(program->testTex2, figures[1]->trimLine, false, false, figures[1]->wrapParametrizationFloodFill, program);
	}
}
void FillImage(unsigned int& texName, Program* program)
{
	int i, j, c;
	for (i = 0; i < checkImageHeight; i++) {
		for (j = 0; j < checkImageWidth; j++) {
			int w = 255;
			checkImage[i][j][0] = (GLubyte)w;
			checkImage[i][j][1] = (GLubyte)w;
			checkImage[i][j][2] = (GLubyte)w;
			checkImage[i][j][3] = (GLubyte)w;

			if (texName == program->texRL)
			{
				totalImageRL[i][j][0] = w;
				totalImageRL[i][j][1] = w;
				totalImageRL[i][j][2] = w;
				totalImageRL[i][j][3] = w;
			}
			if (texName == program->texRP)
			{
				totalImageRP[i][j][0] = w;
				totalImageRP[i][j][1] = w;
				totalImageRP[i][j][2] = w;
				totalImageRP[i][j][3] = w;
			}
			if (texName == program->texSL)
			{
				totalImageSL[i][j][0] = w;
				totalImageSL[i][j][1] = w;
				totalImageSL[i][j][2] = w;
				totalImageSL[i][j][3] = w;
			}
			if (texName == program->texSP)
			{
				totalImageSP[i][j][0] = w;
				totalImageSP[i][j][1] = w;
				totalImageSP[i][j][2] = w;
				totalImageSP[i][j][3] = w;
			}
			if (texName == program->texT)
			{
				totalImageT[i][j][0] = w;
				totalImageT[i][j][1] = w;
				totalImageT[i][j][2] = w;
				totalImageT[i][j][3] = w;
			}
			if (texName == program->texP)
			{
				totalImageP[i][j][0] = w;
				totalImageP[i][j][1] = w;
				totalImageP[i][j][2] = w;
				totalImageP[i][j][3] = w;
			}
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


void CreateTexBuffer(unsigned int& framebuffer, unsigned int& texName)
{
	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	// create a color attachment texture
	glGenTextures(1, &texName);
	glBindTexture(GL_TEXTURE_2D, texName);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, checkImageWidth, checkImageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texName, 0);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;

}

void FloodFill(bool wrapParametrizationFloodFill)
{
	bool found = false;
	int i_start, j_start;
	for (int i = 0; i < checkImageHeight; ++i)
		for (int j = 0; j < checkImageWidth; ++j)
		{
			added[i][j] = false;
		}
	for (int i = 0; i < checkImageHeight && !found; ++i)
		for (int j = 0; j < checkImageWidth && !found; ++j)
		{
			if (checkImage[i][j][0] == (GLubyte)255)
			{
				i_start = i;
				j_start = j;
				found = true;
			}
		}

	std::list<int> is;
	std::list<int> js;

	is.push_back(i_start);
	js.push_back(j_start);
	added[i_start][j_start] = true;
	int i, j, jm, jp, im, ip;
	while (is.size() > 0)
	{
		i = is.front();
		j = js.front();
		is.pop_front();
		js.pop_front();
		checkImage[i][j][0] = (GLubyte)0;
		checkImage[i][j][1] = (GLubyte)0;
		checkImage[i][j][2] = (GLubyte)0;

		ip = i + 1;
		jp = j + 1;
		im = i - 1;
		jm = j - 1;
		if (wrapParametrizationFloodFill)
		{
			if (ip == checkImageHeight) ip = 0;
			if (jp == checkImageHeight) jp = 0;
			if (im < 0) im = checkImageHeight - 1;
			if (jm < 0) jm = checkImageWidth - 1;
		}
		else
		{
			if (ip == checkImageHeight) ip--;
			if (jp == checkImageHeight) jp--;
			if (im < 0) im++;
			if (jm < 0) jm++;
		}

		if (checkImage[im][j][0] == (GLubyte)255 && !added[im][j])
		{
			is.push_back(im);
			js.push_back(j);
			added[im][j] = true;
		}
		if (checkImage[ip][j][0] == (GLubyte)255 && !added[ip][j])
		{
			is.push_back(ip);
			js.push_back(j);
			added[ip][j] = true;
		}
		if (checkImage[i][jm][0] == (GLubyte)255 && !added[i][jm])
		{
			is.push_back(i);
			js.push_back(jm);
			added[i][jm] = true;
		}
		if (checkImage[i][jp][0] == (GLubyte)255 && !added[i][jp])
		{
			is.push_back(i);
			js.push_back(jp);
			added[i][jp] = true;
		}
	}
}

void Inverse()
{
	for (int i = 0; i < checkImageHeight; ++i)
		for (int j = 0; j < checkImageWidth; ++j)
		{
			if (checkImage[i][j][0] == (GLubyte)255)
			{
				checkImage[i][j][0] = (GLubyte)0;
				checkImage[i][j][1] = (GLubyte)0;
				checkImage[i][j][2] = (GLubyte)0;
			}
			else
			{
				checkImage[i][j][0] = (GLubyte)255;
				checkImage[i][j][1] = (GLubyte)255;
				checkImage[i][j][2] = (GLubyte)255;
			}
		}
}