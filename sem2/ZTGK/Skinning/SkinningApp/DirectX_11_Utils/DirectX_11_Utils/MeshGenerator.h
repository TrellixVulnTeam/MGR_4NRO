#ifndef MESH_GENERATOR_H
#define MESH_GENERATOR_H

#include "vertexDef.h"
#include <vector>
#include <tuple>

namespace Real_Engine
{
	class MeshGenerator
	{
	public:
		static std::tuple<std::vector<Vertex>, std::vector<UINT>> GetPlaneMesh();
		static std::tuple<std::vector<Vertex>, std::vector<UINT>> GetCubeMesh();
		static std::tuple<std::vector<Vertex>, std::vector<UINT>> GetCyllinderMesh();
	};
}
#endif // MESH_GENERATOR_H
