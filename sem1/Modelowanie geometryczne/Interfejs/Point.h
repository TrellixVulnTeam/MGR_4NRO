#pragma once
#include "Figure.h"
class Point : public Figure
{
public:
	Point(Shader _shader);
	bool virtual GetGuiInternal(std::vector<Figure*> figures, bool fromMainGui) override;
	virtual void Draw(int transLoc) override;
	void Unpin();
	void RecalcParent();
	void AddParent(Figure* f);
private:
	bool virtual Create() override;
	bool firstTime = true;
	Figure* parent = NULL;
};

