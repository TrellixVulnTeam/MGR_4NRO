#pragma once

#include <tuple>
#include <string>
#include <vector>
#include <fstream> 
#include <iostream>

#include <dxDevice.h>
#include <vertexDef.h>

#include "..\UV.h"
#include "..\Edge.h"
#include "..\Vector.h"
#include "..\Triangle.h"

#include "MeshRobot.h"

using namespace std;

namespace duckApp
{
    class MeshLoader
    {
    public:
        MeshLoader();

        MeshRobot LoadDuck(mini::Mesh &mesh, mini::DxDevice device);

        tuple<vector<mini::VertexPositionNormal>, vector<short>> CreateRectangle(float width, float height);
        tuple<vector<mini::VertexPositionNormal>, vector<short>> CreateTwoSideRectangle(float width, float height);
        tuple<vector<mini::VertexPositionNormal>, vector<short>> CreateCylinder(float radius, float height);
        tuple<vector<mini::VertexPositionNormal>, vector<short>> CreateBox(float width, float height, float depth);

    private:
        vector<string> file_names;

        MeshRobot LoadSingleMesh(string file_name);

        void LoadVerticesPositionsNormalsUVs(ifstream &file_stream, vector<Vector> *vertices_positions, vector<Vector> *vertices_normals, vector<UV> *uvs);
        void LoadTriangles(ifstream &file_stream, vector<Triangle> *triangles);

        vector<short> BoxIndices();
        vector<mini::VertexPositionNormal> BoxVertices(float width, float height, float depth);
    };
}
