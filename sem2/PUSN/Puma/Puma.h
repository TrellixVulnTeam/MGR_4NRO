#pragma once
#include "Figure.h"
#include "Cylinder.h"
#include "Cursor.h"


struct Parameters {
public:
	float Distance(Parameters params);
	float a1 = M_PI / 4;
	float a2 = M_PI / 4;
	float a3 = M_PI / 8;
	float a4 = -M_PI / 4;
	float a5 = M_PI / 4;
	float q2 = 3.0f;
};

class Puma : public Figure
{
public:
	Puma();
	bool virtual GetGuiInternal(std::shared_ptr<Figure> par) override;
	virtual void Draw() override;
	void virtual Initialize(std::shared_ptr<Program> _program) override;
	void virtual RecalcFigure() override;
	float l1 = 3.0f, l3 = 3.0f, l4 = 3.0f, q2_old = 3.0f;
	std::pair<Parameters, Parameters> InverseKinematics(glm::vec3 p5, glm::quat q);
	void ForceConfiguration(glm::vec3 p5, glm::quat q, int n);
	void SetParams(Parameters params);
private:
	Parameters params;
	bool virtual Create() override;
	std::vector<std::shared_ptr<Cylinder>> cylinders;
	std::shared_ptr<Cursor> cursorp3;
	std::shared_ptr<Cursor> cursorp4;
	std::shared_ptr<Cursor> cursor;
};

