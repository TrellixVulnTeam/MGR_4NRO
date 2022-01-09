#pragma once
#include "Figure.h"

class Parameters {
public:
	Parameters();
	void Set(std::shared_ptr<Parameters> p1, float t, std::shared_ptr<Parameters> p2);
	float Distance(std::shared_ptr<Parameters> params);
	void Fix();
	float a1;
	float a2;
	float a3;
	float a4;
	float a5;
	float q2;
};