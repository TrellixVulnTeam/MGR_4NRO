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
	void Drill(glm::vec3 from, glm::vec3 to);
	float drillSize;
private:
	bool virtual Create() override;
	void GenTexture();
	void DoBresenham(glm::ivec2 v1, glm::ivec2 v2, float h1, float h2);
	void PutDrill(int x, int y, float height);
	void SetHeight(int x, int y, float height);
	std::vector<float> data{};

	bool genTexture = true;
	unsigned int heightTextureID;
	int zSplit, xSplit, xBias, yBias;
	float xDiff, zDiff;
	float width, length, height;
	glm::ivec2 GetPos(glm::vec2 coords);
};

