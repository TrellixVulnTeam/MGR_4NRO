#pragma once
#include <vector>
#include "Camera.h"
#include "Program.h"

float DeCasteljau(std::vector<float> coeffs, float t);

glm::ivec3 GetScreenPos(Program* program, glm::vec4 pos);

void DeBoorToBernstein(float a,float b, float c, float d, glm::vec4& bernsteins);

void PowerToBernsteinBasis(glm::vec4& power, glm::vec4& bernstein);

void ThomasAlgorithm(std::vector<float>& a, std::vector<float>& b, std::vector<float>& c, std::vector<glm::vec3>& d, std::vector<glm::vec3>& res, int n);