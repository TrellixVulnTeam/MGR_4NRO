#pragma once
#include "Figure.h"
class Point : public Figure
{
public:
	Point(bool addIndex);
	Point() :Point(true) {};
	bool virtual GetGuiInternal(std::shared_ptr<Figure> par) override;
	virtual void Draw() override;
	void Unpin(std::shared_ptr<Figure> par);
	void RecalcParent();
	void AddParent(std::shared_ptr<Figure> f);
	void virtual Initialize(std::shared_ptr<Program> _program) override;
	bool virtualPoint = false;
	bool toDel = false;
private:
	bool virtual Create() override;
	bool firstTime = true;
};

