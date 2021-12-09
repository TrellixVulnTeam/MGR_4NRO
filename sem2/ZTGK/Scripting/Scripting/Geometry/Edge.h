#pragma once

#include "Triangle.h"

namespace duckApp
{
    struct Edge
    {
    public:
        int vertex_position_1;
        int vertex_position_2;

        int triangle_index_1;
        int triangle_index_2;

        Edge(int v1, int v2, int t1, int t2);
    };
}
