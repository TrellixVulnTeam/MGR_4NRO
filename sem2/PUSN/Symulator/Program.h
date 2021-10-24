#pragma once
#include <vector>
#include "Shader.h"
#define DEFAULT_WIDTH 1280
#define DEFAULT_HEIGHT 720
class Figure;
class MiddlePoint;
class Cursor;
class Cube;
class PointsLine;
class Camera;
class Point;

class Program
{
public:
	Program();
	~Program() = default;

	bool useCursor = false;
	bool drill = false;
	int current_width = 0;
	int current_height = 0;
	unsigned int colorTexture;
	std::vector<std::shared_ptr<Figure>> figures;
	//Cursor* cur;
	//Camera* cam;
	//MiddlePoint* mp;
	std::shared_ptr<MiddlePoint> mp = {};
	std::shared_ptr<Cursor> cur = {};
	std::shared_ptr<Camera> cam = {};
	std::shared_ptr<Shader> shader = {};
	std::shared_ptr<Shader> lightShader = {};
	std::shared_ptr<Shader> bezierShader = {};
	std::shared_ptr<Cube> cube = {};
	std::shared_ptr<PointsLine> pl = {};
	std::shared_ptr<Point> drillPoint = {};
};

