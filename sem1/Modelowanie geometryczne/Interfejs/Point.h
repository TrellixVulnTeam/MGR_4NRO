#pragma once
#include "Figure.h"
class Point : public Figure
{
public:
	Point();
	bool virtual GetGuiInternal(bool fromMainGui) override;
	virtual void Draw() override;
	void Unpin();
	void RecalcParent();
	void AddParent(Figure* f);
	void virtual Initialize(Program* _program) override;
private:
	bool virtual Create() override;
	bool firstTime = true;
};

