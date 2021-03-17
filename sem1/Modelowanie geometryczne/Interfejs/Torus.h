#pragma once
#include "Figure.h"
class Torus : public Figure
{
public:
	Torus(Shader _shader);
	bool virtual GetGui(int i) override;
	virtual void Draw(int transLoc) override;
private:
	bool virtual Create() override;
	int m_new = 8, n_new = 8;
	float r_new = 0.2f, R_new = 1.0f;
	int m_old = -1, n_old = -1;
	float r_old = -1.0f, R_old = -1.0f;
};

