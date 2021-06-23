#pragma once
#include "rapidxml/rapidxml.hpp"
#include "rapidxml/rapidxml_print.hpp"
#include "rapidxml/rapidxml_utils.hpp"
#include <map>
#include "Program.h"
#include "Figure.h"
#include "Point.h"
#include "SomeCurve.h"
#include "SomePatch.h"
#include "Torus.h"
#include "BezierCurveC0.h"
#include "BezierCurveC2.h"
#include "InterpolationCurveC2.h"
#include "BezierPatchC0.h"
#include "BezierPatchC2.h"
#include "Tools.h"

using namespace rapidxml;

void Serialize(Program* program);
void Deserialize(Program* program);