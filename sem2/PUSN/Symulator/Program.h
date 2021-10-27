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
	bool showPath = true;
	bool drill = false;
	bool showSimulation = true;
	int current_width = 0;
	int current_height = 0;
	float drillingSpeed = 8.0f;
	unsigned int colorTexture = 0;
	float width = 180.0f;
	float length = 180.0f;
	float height = 60.0f;
	int xSplit = 120;
	int ySplit = 120;
	float drillHeight = 50.0f;
	float minimumHeight = 20.0f;
	bool ignoreErrors = false;
	std::vector<std::shared_ptr<Figure>> figures = {};
	std::string error = "";
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

