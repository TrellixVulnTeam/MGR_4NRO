#pragma once
#include <vector>
#include "Program.h"
#include "BezierPatchC0.h"
#include "Point.h"

void Collapse(Program* program);
void CreateGregory(Program* program);
std::vector<std::vector<std::vector<Point*>>> CheckIfCanMerge(Program* program, std::vector<BezierPatchC0*>);
std::vector<std::vector<std::vector<Point*>>> CanMerge(std::vector<std::vector<Point*>> patch0, std::vector<std::vector<Point*>> patch1, std::vector<std::vector<Point*>> patch2);
std::vector<std::vector<Point*>> Rotate(std::vector<std::vector<Point*>> patch);
std::vector<std::vector<Point*>> Swap(std::vector<std::vector<Point*>> patch);