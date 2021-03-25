#pragma once
#include "Figure.h"
#include "Camera.h"
class Cursor : public Figure
{
public:
	Cursor(Shader _shader);
	void ForceRecalcScreenPos();
	bool virtual GetGuiInternal(std::vector<Figure*> figures, bool fromMainGui) override;
	virtual void Draw(int transLoc) override;
	Camera* cam;
	int cur_width;
	int cur_height;
private:
	bool virtual Create() override;
	bool firstTime = true;
	glm::vec3 pos;
	glm::vec3 posOld;
	glm::ivec3 posScreen;
	glm::ivec3 posScreenOld;
};

