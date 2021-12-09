#pragma once

#include <vector>

#include <DirectXMath.h>

#include "Vector.h"

namespace duckApp
{
    struct Triangle
    {
    public:
        short vertex_indices[3];

        DirectX::XMVECTOR vector_1;
        DirectX::XMVECTOR vector_2;
        DirectX::XMVECTOR vector_3;

        DirectX::XMVECTOR normal;

        Triangle();

        Triangle(short p1, short p2, short p3);

        void SetXMVECTORs(std::vector<Vector> positions);

        //DirectX::XMVECTOR GetNormal(std::vector<Vector> positions);
        DirectX::XMVECTOR GetLightVector(std::vector<Vector> positions, DirectX::XMVECTOR light_position);
        bool IsBackFace(std::vector<Vector> positions, DirectX::XMMATRIX model_matrix, DirectX::XMVECTOR light_position);
    };
}
