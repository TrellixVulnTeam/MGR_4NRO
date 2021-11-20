#pragma once
#include "Figure.h"


class Cube : public Figure
{
public:
	Cube();
	bool virtual GetGuiInternal(std::shared_ptr<Figure> par) override;
	virtual void Draw() override;
	void virtual Initialize(std::shared_ptr<Program> _program) override;
	void virtual RecalcFigure() override;
private:
	bool virtual Create() override;
	void GenTexture();
	std::vector<float> data{};

	float drillRadius;
	bool genTexture = true;
	unsigned int heightTextureID;
	int zSplit, xSplit, xBias, yBias;
	float xDiff, zDiff;
	float width, length, height;
};

