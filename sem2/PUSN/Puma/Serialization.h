#pragma once
#include "rapidxml/rapidxml.hpp"
#include "rapidxml/rapidxml_print.hpp"
#include "rapidxml/rapidxml_utils.hpp"
#include <map>
#include "Program.h"
#include "Figure.h"
#include "Point.h"
#include "SomeCurve.h"
#include "Tools.h"

using namespace rapidxml;

void Serialize(std::shared_ptr<Program> program);
void Deserialize(std::shared_ptr<Program> program);