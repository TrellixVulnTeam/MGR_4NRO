#pragma once
#include "Figure.h"
class Cursor : public Figure
{
public:
	Cursor(Shader _shader);
	bool virtual GetGui(int i) override;
	virtual void Draw(int transLoc) override;
private:
	bool virtual Create() override;
	bool firstTime = true;
};

