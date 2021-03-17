#pragma once
#include "Figure.h"
class Point : public Figure
{
public:
	Point(Shader _shader);
	bool virtual GetGui(int i) override;
	virtual void Draw(int transLoc) override;
private:
	bool virtual Create() override;
	bool firstTime = true;
};

