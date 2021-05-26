#pragma once
#include "Figure.h"
class Point : public Figure
{
public:
	Point(bool addIndex);
	Point() :Point(true) {};
	bool virtual GetGuiInternal(Figure* par) override;
	virtual void Draw() override;
	void Unpin(Figure* par);
	void RecalcParent();
	void AddParent(Figure* f);
	void virtual Initialize(Program* _program) override;
	bool virtualPoint = false;
	bool toDel = false;
private:
	bool virtual Create() override;
	bool firstTime = true;
};

