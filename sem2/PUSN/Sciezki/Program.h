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

	bool allGregorys = false;
	bool useCursor = false;
	int current_width = 0;
	int current_height = 0;
	float eyeDist = 0.01f;
	float d = 3.0f;
	float w = 15.0f;
	int patches_n = 1;
	int patches_m = 1;
	float bezierC0width = 180.0f;
	float bezierC0length = 180.0f;
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
	float newtonD = 1.0f;
	float stopSearchingPointsLoopD = 1.0f;
	float stopSearchingPointsFarD = 5.0f;
	float connectLineToWallEps = 0.0f;
	float pointsDistNewton = 1.0f;
	int split = 900;
	float smallMoveF= 1.0f;
	unsigned int texRP = 0;
	unsigned int texRL = 0;
	unsigned int texSP = 0;
	unsigned int texSL = 0;
	unsigned int texT = 0;
	unsigned int texP = 0;
	bool showTexRP = false;
	bool showTexRL = false;
	bool showTexSP = false;
	bool showTexSL = false;
	bool showTexT  = false;  
	bool showTexP  = false;  
	float r = 5.0f;
};

