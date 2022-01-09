#include "Parameters.h"
Parameters::Parameters()
{
	a1 = M_PI / 4;
	a2 = M_PI / 4;
	a3 = M_PI / 8;
	a4 = -M_PI / 4;
	a5 = M_PI / 4;
	q2 = 3.0f;
	Fix();
}
void Parameters::Set(std::shared_ptr<Parameters> p1, float t, std::shared_ptr<Parameters> p2)
{
	a1 = p1->a1 + t * p2->a1;
	a2 = p1->a2 + t * p2->a2;
	a3 = p1->a3 + t * p2->a3;
	a4 = p1->a4 + t * p2->a4;
	a5 = p1->a5 + t * p2->a5;
	q2 = p1->q2 + t * p2->q2;
	Fix();
}
float Parameters::Distance(std::shared_ptr<Parameters> params)
{
	float d1 = abs(params->a1 - a1);
	float d2 = abs(params->a2 - a2);
	float d3 = abs(params->a3 - a3);
	float d4 = abs(params->a4 - a4);
	float d5 = abs(params->a5 - a5);
	float d6 = params->q2 - q2;

	if (d1 > M_PI) d1 = 2 * M_PI - d1;
	if (d2 > M_PI) d2 = 2 * M_PI - d2;
	if (d3 > M_PI) d3 = 2 * M_PI - d3;
	if (d4 > M_PI) d4 = 2 * M_PI - d4;
	if (d5 > M_PI) d5 = 2 * M_PI - d5;

	return d1 * d1 + d2 * d2 + d3 * d3 + d4 * d4 + d5 * d5 + d6 * d6;
}

void Parameters::Fix()
{
	while (a1 < 0) a1 += 2 * M_PI;
	while (a2 < 0) a2 += 2 * M_PI;
	while (a3 < 0) a3 += 2 * M_PI;
	while (a4 < 0) a4 += 2 * M_PI;
	while (a5 < 0) a5 += 2 * M_PI;

	while (a1 > 2 * M_PI) a1 -= 2 * M_PI;
	while (a2 > 2 * M_PI) a2 -= 2 * M_PI;
	while (a3 > 2 * M_PI) a3 -= 2 * M_PI;
	while (a4 > 2 * M_PI) a4 -= 2 * M_PI;
	while (a5 > 2 * M_PI) a5 -= 2 * M_PI;
}
