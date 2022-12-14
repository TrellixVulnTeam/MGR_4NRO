
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
	glm::vec3 Drill();
	std::vector<glm::vec3> points = {};
	glm::vec3 drillingPos = { 0,0,0 };
private:
	bool virtual Create() override;
	bool drillInitialized = false;
	int startPoints = 0;
	int lastP = -1;
};

