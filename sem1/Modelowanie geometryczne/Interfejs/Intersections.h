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

void GetClosestPoints(Program* program, Figure* f1, float& u1, float& v1, Figure* f2, float& u2, float& v2);
void FindStartPoints(Program* program, Figure* f1, float& u1, float& v1, Figure* f2, float& u2, float& v2);
void FindClosestPoints(Program* program, Figure* f1, float& u1, float& v1, Figure* f2, float& u2, float& v2);
void FindPointClosestToCursor(Program* program, Figure* f, float& u_best, float& v_best);
void Intersect(Program* program);
void FillImage(unsigned int& texName);