#pragma once
#include <vector>
#include "Camera.h"
#include "Program.h"
glm::ivec3 GetScreenPos(std::shared_ptr<Program> program, glm::vec4 pos);

