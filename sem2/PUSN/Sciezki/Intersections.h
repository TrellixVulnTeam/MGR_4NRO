#pragma once
#include "Program.h"
#include "BezierPatchC0.h"
#include "BezierPatchC2.h"
#include "Torus.h"
#include "Point.h"
#include "Cursor.h"

#define checkImageWidth 1024
#define checkImageHeight 1024
static GLubyte checkImage[checkImageHeight][checkImageWidth][4];

static GLubyte totalImageRL[checkImageHeight][checkImageWidth][4];
static GLubyte totalImageRP[checkImageHeight][checkImageWidth][4];
static GLubyte totalImageSL[checkImageHeight][checkImageWidth][4];
static GLubyte totalImageSP[checkImageHeight][checkImageWidth][4];
static GLubyte totalImageT [checkImageHeight][checkImageWidth][4];
static GLubyte totalImageP [checkImageHeight][checkImageWidth][4];

static bool added[checkImageHeight][checkImageWidth];
void FindPointsLoop(Program* program, Figure* f1, Figure* f2, std::vector<glm::vec2>& points1, std::vector<glm::vec2>& points2, std::vector<glm::vec3>& points, float dir);
void FindPointsLoopTool(Program* program, Figure* f1, Figure* f2, std::vector<glm::vec2>& points1, std::vector<glm::vec2>& points2, std::vector<glm::vec3>& points, float dir, int normDir1, int normDir2);
void GetClosestPoints(Program* program, Figure* f1, float& u1, float& v1, Figure* f2, float& u2, float& v2);
void GetClosestPointsTool(Program* program, Figure* f1, float& u1, float& v1, Figure* f2, float& u2, float& v2, int normDir1, int normDir2);
void FindStartPoints(Program* program, Figure* f1, float& u1, float& v1, Figure* f2, float& u2, float& v2);
void FindStartPointsTool(Program* program, Figure* f1, float& u1, float& v1, Figure* f2, float& u2, float& v2, int normDir1, int normDir2);
void FindClosestPoints(Program* program, Figure* f1, float& u1, float& v1, Figure* f2, float& u2, float& v2);
void FindClosestPointsTool(Program* program, Figure* f1, float& u1, float& v1, Figure* f2, float& u2, float& v2, int normDir1, int normDir2);
void FindPointClosestToCursor(Program* program, Figure* f, float& u_best, float& v_best);
void FindPointClosestToCursorTool(Program* program, Figure* f, float& u_best, float& v_best, int normDir);
void Intersect(Program* program);
void IntersectTool(Program* program, int normDir1, int normDir2);
std::vector<glm::vec3> Intersect2(Program* program, float dir);
std::vector<glm::vec3> Intersect2Tool(Program* program, float dir, int normDir1, int normDir2, unsigned int tex1, unsigned int tex2, std::vector<glm::vec2>& points1,std::vector<glm::vec2>& points2);
void FillImage(unsigned int& texName, Program* program);
void CreateTexBuffer(unsigned int& framebuffer, unsigned int& texName);
void FloodFill(bool wrapParametrizationFloodFill);
void Inverse();
void SetTexture(unsigned int& sourceTex, unsigned int& destTex, bool fill, bool inverse, bool wrapParametrizationFloodFill, Program* program);
glm::vec3 GetPos(Figure* f, float u, float v);
glm::vec3 GetDer(Figure* f, float u, float v, bool du);
glm::vec3 GetToolPos(Figure* f, float u, float v, int normDir);
glm::vec3 GetToolDer(Figure* f, float u, float v, bool du, int normDir);