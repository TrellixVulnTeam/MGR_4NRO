#include"Math.h"
#include"GregoryPatch.h"

glm::vec3 DeCasteljau(std::vector<glm::vec3> coeffs, float t)
{
	std::vector<glm::vec3> coeffs_t(coeffs);

	for (int i = coeffs.size(); i > 0; --i)
	{
		for (int j = 0; j < i - 1; ++j)
		{
			coeffs_t[j] = (1.0f - t) * coeffs_t[j] + t * coeffs_t[j + 1];
		}
	}
	return coeffs_t[0];
}

glm::vec3 DeCasteljauDerivative(std::vector<glm::vec3> coeffs, float t)
{
	std::vector<glm::vec3> coeffs_t;

	coeffs_t.push_back(3.0f * (coeffs[1] - coeffs[0]));
	coeffs_t.push_back(3.0f * (coeffs[2] - coeffs[1]));
	coeffs_t.push_back(3.0f * (coeffs[3] - coeffs[2]));

	return DeCasteljau(coeffs_t, t);
}

glm::vec3 DeBoor(float t, glm::vec3 B0_, glm::vec3 B1_, glm::vec3 B2_, glm::vec3 B3_)
{
	float T0 = -1.0f;
	float T1 = 0.0f;
	float T2 = 1.0f;
	float T3 = 2.0f;
	float T4 = 3.0f;
	float Tm1 = -2.0f;

	float A1 = T2 - t;
	float A2 = T3 - t;
	float A3 = T4 - t;
	float B1 = t - T1;
	float B2 = t - T0;
	float B3 = t - Tm1;

	float N1 = 1;
	float N2 = 0;
	float N3 = 0;
	float N4 = 0;

	float saved = 0.0f;
	float term = 0.0f;

	term = N1 / (A1 + B1);
	N1 = saved + A1 * term;
	saved = B1 * term;

	N2 = saved;
	saved = 0.0f;

	term = N1 / (A1 + B2);
	N1 = saved + A1 * term;
	saved = B2 * term;

	term = N2 / (A2 + B1);
	N2 = saved + A2 * term;
	saved = B1 * term;

	N3 = saved;
	saved = 0.0f;

	term = N1 / (A1 + B3);
	N1 = saved + A1 * term;
	saved = B3 * term;

	term = N2 / (A2 + B2);
	N2 = saved + A2 * term;
	saved = B2 * term;

	term = N3 / (A3 + B1);
	N3 = saved + A3 * term;
	saved = B1 * term;

	N4 = saved;

	return N1 * B0_ + N2 * B1_ + N3 * B2_ + N4 * B3_;
}

glm::vec3 TangentVecDeBoor(float t, glm::vec3 B0_, glm::vec3 B1_, glm::vec3 B2_, glm::vec3 B3_) {
	float T0 = -1.0f;
	float T1 = 0.0f;
	float T2 = 1.0f;
	float T3 = 2.0f;
	float T4 = 3.0f;
	float Tm1 = -2.0f;

	float A1 = T2 - t;
	float A2 = T3 - t;
	float A3 = T4 - t;
	float B1 = t - T1;
	float B2 = t - T0;
	float B3 = t - Tm1;

	float N1 = 1;
	float N2 = 0;
	float N3 = 0;
	float N4 = 0;

	float saved = 0.0f;
	float term = 0.0f;

	term = N1 / (A1 + B1);
	N1 = saved + A1 * term;
	saved = B1 * term;

	N2 = saved;
	saved = 0.0f;

	term = N1 / (A1 + B2);
	N1 = saved + A1 * term;
	saved = B2 * term;

	term = N2 / (A2 + B1);
	N2 = saved + A2 * term;
	saved = B1 * term;

	N3 = saved;

	glm::vec3 f1 = 3.0f * ((B1_ - B0_) / (T2 - Tm1));
	glm::vec3 f2 = 3.0f * ((B2_ - B1_) / (T3 - T0));
	glm::vec3 f3 = 3.0f * ((B3_ - B2_) / (T4 - T1));

	return N1 * f1 + N2 * f2 + N3 * f3;
}

glm::ivec3 GetScreenPos(Program* program, glm::vec4 pos)
{
	glm::ivec3 _posScreen;
	glm::vec4 posScreen = program->cam->GetProjectionMatrix() * program->cam->GetViewportMatrix() * pos;
	posScreen /= posScreen.w;
	posScreen = (posScreen + glm::vec4(1.0f)) / 2.0f;
	_posScreen.x = posScreen.x * program->current_width;
	_posScreen.y = posScreen.y * program->current_height;
	_posScreen.z = posScreen.z;
	return _posScreen;
}

void DeBoorToBernstein(float a, float b, float c, float d, glm::vec4& bernsteins)
{
	float p1 = a + 2.0f / 3.0f * (b - a);
	float p2 = b + 1.0f / 3.0f * (c - b);
	float p3 = b + 2.0f / 3.0f * (c - b);
	float p4 = c + 1.0f / 3.0f * (d - c);

	bernsteins.x = (p1 + p2) / 2.0f;
	bernsteins.y = p2;
	bernsteins.z = p3;
	bernsteins.w = (p3 + p4) / 2.0f;
}

void PowerToBernsteinBasis(glm::vec4& power, glm::vec4& bernstein)
{
	glm::mat4x4 matrix = {
		1,1,1,1,
		0,1.0f / 3,2.0f / 3,1,
		0,0,1.0f / 3,1,
		0,0,0,1
	};
	bernstein = matrix * power;
}

void ThomasAlgorithm(std::vector<float>& a, std::vector<float>& b, std::vector<float>& c, std::vector<glm::vec3>& d, std::vector<glm::vec3>& res, int n)
{
	std::vector<float> beta;
	beta.push_back(-c[0] / b[0]);
	for (int i = 1; i < n; ++i)
	{
		beta.push_back(-c[i] / (a[i] * beta[i - 1] + b[i]));
	}
	std::vector<glm::vec3>gamma;
	gamma.push_back(d[0] / b[0]);
	for (int i = 1; i < n; ++i)
	{
		gamma.push_back((d[i] - a[i] * gamma[i - 1]) / (a[i] * beta[i - 1] + b[i]));
	}

	glm::vec3* x = new glm::vec3[n];
	x[c.size() - 1] = gamma[n - 1];
	for (int i = n - 2; i >= 0; --i)
	{
		x[i] = beta[i] * x[i + 1] + gamma[i];
	}

	for (int i = 0; i < n; ++i)
	{
		res.push_back(x[i]);
	}
	delete[] x;
}