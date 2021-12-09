#include "MeshRobot.h"

namespace duckApp
{
    void MeshRobot::AddVertexPosition(Vector vertex_position)
    {
        vertices_positions.push_back(vertex_position);
    }

    void MeshRobot::AddVertexPosition(float x, float y, float z)
    {
        vertices_positions.push_back(Vector(x, y, z));
    }

    void MeshRobot::AddVertex(int vertex_position_index, Vector normal_vector)
    {
        vertices.push_back(vertices_positions[vertex_position_index]);
        normal_vectors.push_back(normal_vector);
    }

    void MeshRobot::AddVertex(int vertex_position_index, float x, float y, float z)
    {
        vertices.push_back(vertices_positions[vertex_position_index]);
        normal_vectors.push_back(Vector(x, y, z));
    }

    void MeshRobot::AddTriangle(Triangle triangle)
    {
        triangles.push_back(triangle);
    }

    void MeshRobot::AddTriangle(int p1, int p2, int p3)
    {
        triangles.push_back(Triangle(p1, p2, p3));
    }

    void MeshRobot::AddEdge(Edge edge)
    {
        edges.push_back(edge);
    }

    void MeshRobot::AddEdge(int v1, int v2, int t1, int t2)
    {
        edges.push_back(Edge(v1, v2, t1, t2));
    }

    void MeshRobot::AddUV(UV uv)
    {
        uvs.push_back(uv);
    }
}
