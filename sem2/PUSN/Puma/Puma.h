#pragma once
#include "Figure.h"
#include "Cylinder.h"
#include "Cursor.h"
#include "Parameters.h"

class Puma : public Figure
{
public:
	Puma();
	bool virtual GetGuiInternal(std::shared_ptr<Figure> par) override;
	virtual void Draw() override;
	void virtual Initialize(std::shared_ptr<Program> _program) override;
	void virtual RecalcFigure() override;
	float l1 = 3.0f, l3 = 3.0f, l4 = 4.0f, q2_old = 3.0f;
	std::pair<std::shared_ptr<Parameters>, std::shared_ptr<Parameters>> InverseKinematics(glm::vec3 p5, glm::quat q);
	void ForceConfiguration(glm::vec3 p5, glm::quat q, int n);
	void SetParams(std::shared_ptr<Parameters> params);
	std::shared_ptr<Parameters> GetParams();
	std::shared_ptr<Parameters> params;
private:
	bool virtual Create() override;
	std::vector<std::shared_ptr<Cylinder>> cylinders;
	std::shared_ptr<Cursor> cursorp3;
	std::shared_ptr<Cursor> cursorp4;
	std::shared_ptr<Cursor> cursor;
};

