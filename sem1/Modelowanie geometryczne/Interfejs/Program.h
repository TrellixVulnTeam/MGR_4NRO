#pragma once
#include <vector>
#include "Shader.h"
#define DEFAULT_WIDTH 1280
#define DEFAULT_HEIGHT 720
class Figure;
class MiddlePoint;
class Cursor;
class Camera;

class Program
{
public:
	Program();
	~Program() = default;

	bool anaglyph = false;
	bool allGregorys = false;
	bool useCursor = false;
	int current_width = 0;
	int current_height = 0;
	float eyeDist = 0.01f;
	float d = 3.0f;
	float w = 15.0f;
	int patches_n = 1;
	int patches_m = 1;
	float bezierC0width = 1.0f;
	float bezierC0length = 1.0f;
	float bezierC0r = 1.0f;
	std::vector<Figure*> figures;
	MiddlePoint* mp = NULL;
	Cursor* cur = NULL;
	Camera* cam = NULL;
	Shader shader;
	Shader trimShader;
	Shader bezierShader;
	Shader patchShader;
	Shader patchShaderDeBoor;
	Shader patchGregoryShader;
	unsigned int testTex;
	unsigned int testFrame;
	unsigned int testTex2;
	int startPointsSplits = 10;
	int cursorPointsSplits = 10;
	float closestPointsStartWindowSize = 0.1;
	float newtonD = 0.1f;
	float stopSearchingPointsLoopD = 1e-3;
	float stopSearchingPointsFarD = 0.1f;
	float connectLineToWallEps = 1e-3f;
	float pointsDistNewton = 1e-3;
};

