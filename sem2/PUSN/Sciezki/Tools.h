#pragma once
#include "Program.h"
#include "BezierPatchC0.h"
#include "Point.h"

void Collapse(Program* program);
void CreateGregory(Program* program);
std::vector<std::vector<SinglePatch>> CheckIfCanMerge(Program* program, std::vector<BezierPatchC0*>);
std::vector<std::vector<SinglePatch>> CanMerge(SinglePatch patch0, SinglePatch patch1, SinglePatch patch2);
std::vector<std::vector<Point*>> Rotate(std::vector<std::vector<Point*>> patch);
std::vector<std::vector<Point*>> Swap(std::vector<std::vector<Point*>> patch);
void Clear(Program* program);