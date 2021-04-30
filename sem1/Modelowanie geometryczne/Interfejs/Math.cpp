#include"Math.h"
float DeCasteljau(std::vector<float> coeffs, float t)
{
	std::vector<float> coeffs_t(coeffs);

	for (int i = coeffs.size(); i > 0; --i)
	{
		for (int j = 0; j < i - 1; ++j)
		{
			coeffs_t[j] = (1 - t) * coeffs_t[j] + t * coeffs_t[j + 1];
		}
	}
	return coeffs_t[0];
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
		0,1.0f/3,2.0f/3,1,
		0,0,1.0f/3,1,
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
