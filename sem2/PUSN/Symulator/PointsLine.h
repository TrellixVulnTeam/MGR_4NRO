
#pragma once
#include "Figure.h"
#include "Point.h"
class PointsLine : public Figure
{
public:
	PointsLine();
	bool virtual GetGuiInternal(std::shared_ptr<Figure> par) override { return false; };
	virtual void Draw() override;
	void AddPoint(glm::vec3 point);
	void RemoveAt(int i);
	void Clear();
	void Recalc() { Create(); };
	void virtual Initialize(std::shared_ptr<Program> _program) override;
	std::vector<glm::vec3> points;
	glm::vec3 Drill();
	glm::vec3 drillingPos;
private:
	bool virtual Create() override;
	float drillingSpeed = 28.0f;
	bool drillInitialized = false;
};

