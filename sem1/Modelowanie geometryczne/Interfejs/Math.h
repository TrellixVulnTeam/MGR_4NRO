#pragma once
#include <vector>
float DeCasteljau(std::vector<float> coeffs, float t)
{
	std::vector<float> coeffs_t(coeffs);

	for (int i = coeffs.size(); i > 0; --i)
	{
		for (int j = 0; j < i-1; ++j)
		{
			coeffs_t[j] = (1 - t) * coeffs_t[j] + t * coeffs_t[j + 1];
		}
	}
	return coeffs_t[0];
}