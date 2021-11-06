#include "MeshGenerator.h"

namespace Real_Engine
{
	std::tuple<std::vector<Vertex>, std::vector<UINT>> MeshGenerator::GetPlaneMesh()
	{
		return
		{
			{
				Vertex{XMFLOAT3(-1,0,1)},
				Vertex{XMFLOAT3(1,0,-1)},
				Vertex{XMFLOAT3(-1,0,-1)},
				Vertex{XMFLOAT3(1,0,1)}
			},
			{
				2,1,0/*, 1,3,0,
				0,1,2, 0,3,1,*/
			}
		};
	}

	std::tuple<std::vector<Vertex>, std::vector<UINT>> MeshGenerator::GetCubeMesh()
	{
		return
		{
			{
				//front
				Vertex{ XMFLOAT3(-1,2,-1)},
				Vertex{ XMFLOAT3(-1,0,-1)},
				Vertex{ XMFLOAT3(1,0,-1)},
				Vertex{ XMFLOAT3(1,2,-1)},
				//left
				Vertex{ XMFLOAT3(-1,2,1)},
				Vertex{ XMFLOAT3(-1,0,1)},
				Vertex{ XMFLOAT3(-1,0,-1)},
				Vertex{ XMFLOAT3(-1,2,-1)},
				//back
				Vertex{ XMFLOAT3(1,2,1)},
				Vertex{ XMFLOAT3(1,0,1)},
				Vertex{ XMFLOAT3(-1,0,1)},
				Vertex{ XMFLOAT3(-1,2,1)},
				//right
				Vertex{ XMFLOAT3(1,2,-1)},
				Vertex{ XMFLOAT3(1,0,-1)},
				Vertex{ XMFLOAT3(1,0,1)},
				Vertex{ XMFLOAT3(1,2,1)},
				//top
				Vertex{ XMFLOAT3(-1,2,1)},
				Vertex{ XMFLOAT3(-1,2,-1)},
				Vertex{ XMFLOAT3(1,2,-1)},
				Vertex{ XMFLOAT3(1,2,1)},
				//bottom
				Vertex{ XMFLOAT3(-1,0,-1)},
				Vertex{ XMFLOAT3(-1,0,1)},
				Vertex{ XMFLOAT3(1,0,1)},
				Vertex{ XMFLOAT3(1,0,-1)}
			},
			{
				//front
				0,1,2, 0,2,3,
				//left
				4,5,6, 4,6,7,
				//back
				8,9,10, 8,10,11,
				//right
				12,13,14, 12,14,15,
				//top
				16,17,18, 16,18,19,
				//bottom
				20,21,22, 20,22,23,
			}
		};
	}

	std::tuple<std::vector<Vertex>, std::vector<UINT>> MeshGenerator::GetCyllinderMesh()
	{
		const int N = 40;

		float angle_step = XM_2PI / N;

		std::vector<Vertex> vertices;
		std::vector<UINT> indices;

		//bottom
		for (int index = 0; index < N; index++)
		{
			float angle = index * angle_step;
			vertices.push_back(Vertex{ XMFLOAT3(cos(angle), 0, sin(angle)) });
		}

		//top
		for (int index = 0; index < N; index++)
		{
			float angle = index * angle_step;
			vertices.push_back(Vertex{ XMFLOAT3(cos(angle), 1, sin(angle)) });
		}

		//bottom
		UINT bottom = (UINT)vertices.size();
		vertices.push_back(Vertex{ XMFLOAT3(0, 0, 0) });
		//top
		UINT top = (UINT)vertices.size();
		vertices.push_back(Vertex{ XMFLOAT3(0, 1, 0) });

		//bottom
		for (UINT index = 0; index < N; index++)
		{
			indices.push_back(index);
			indices.push_back(bottom);
			indices.push_back((index + 1) % N);
		}

		//top
		for (UINT index = 0; index < N; index++)
		{
			indices.push_back(N + index);
			indices.push_back(N + (index + 1) % N);
			indices.push_back(top);
		}

		for (UINT index = 0; index < N; index++)
		{
			indices.push_back(N + index);
			indices.push_back(index);
			indices.push_back((index + 1) % N);

			indices.push_back(N + index);
			indices.push_back((index + 1) % N);
			indices.push_back(N + (index + 1) % N);
		}


		return { vertices, indices };
	}
}