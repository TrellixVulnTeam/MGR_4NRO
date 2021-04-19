#pragma once
#include <vector>
#include "Camera.h"
#include "Program.h"

float DeCasteljau(std::vector<float> coeffs, float t);

glm::ivec3 GetScreenPos(Program* program, glm::vec4 pos);

void DeBoorToBernstein(float a,float b, float c, float d, glm::vec4& bernsteins);