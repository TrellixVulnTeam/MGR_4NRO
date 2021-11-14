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
static bool added[checkImageHeight][checkImageWidth];
void FindPointsLoop(Program* program, Figure* f1, Figure* f2, std::vector<glm::vec2>& points1, std::vector<glm::vec2>& points2, std::vector<glm::vec3>& points, float dir);
void GetClosestPoints(Program* program, Figure* f1, float& u1, float& v1, Figure* f2, float& u2, float& v2);
void FindStartPoints(Program* program, Figure* f1, float& u1, float& v1, Figure* f2, float& u2, float& v2);
void FindClosestPoints(Program* program, Figure* f1, float& u1, float& v1, Figure* f2, float& u2, float& v2);
void FindPointClosestToCursor(Program* program, Figure* f, float& u_best, float& v_best);
void Intersect(Program* program);
std::vector<glm::vec3> Intersect2(Program* program, float dir);
void FillImage(unsigned int& texName, Program* program);
void CreateTexBuffer(unsigned int& framebuffer, unsigned int& texName);
void FloodFill(bool wrapParametrizationFloodFill);
void Inverse();
void SetTexture(unsigned int& sourceTex, unsigned int& destTex, bool fill, bool inverse, bool wrapParametrizationFloodFill);
glm::vec3 GetPos(Figure* f, float u, float v);