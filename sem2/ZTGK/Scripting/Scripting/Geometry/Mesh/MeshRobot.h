#pragma once

#include <vector>

#include <dxptr.h>
#include <effect.h>
#include <DirectXMath.h>

#include "..\UV.h"
#include "..\Edge.h"
#include "..\Vector.h"
#include "..\Triangle.h"

using namespace std;

namespace duckApp
{
    class MeshRobot
    {
    public:
        /* Only Vertices Positions */
        vector<Vector> vertices_positions;

        /* Real Vertices with Normals */
        vector<Vector> vertices;
        vector<Vector> normal_vectors;

        /* Texture UVs */
        vector<UV> uvs;

        /* Triangles built from Real Vertices */
        vector<Triangle> triangles;

        /* Edges for Shadow Solids */
        vector<Edge> edges;

        void AddVertexPosition(Vector vertex_position);
        void AddVertexPosition(float x, float y, float z);

        void AddVertex(int vertex_position_index, Vector normal_vector);
        void AddVertex(int vertex_position_index, float x, float y, float z);

        void AddTriangle(Triangle triangle);
        void AddTriangle(int p1, int p2, int p3);

        void AddUV(UV uv);

        void AddEdge(Edge edge);
        void AddEdge(int v1, int v2, int t1, int t2);
    };
}
