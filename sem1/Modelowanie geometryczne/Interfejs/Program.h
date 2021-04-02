#pragma once
#include <vector>
#include "Shader.h"
class Figure;
class MiddlePoint;
class Cursor;
class Camera;

class Program
{
public:
	Program();
	~Program() = default;

	int current_width = 0;
	int current_height = 0;
	std::vector<Figure*> figures;
	MiddlePoint* mp = NULL;
	Cursor* cur = NULL;
	Camera* cam = NULL;
	Shader shader;
	Shader bezierShader;
};

