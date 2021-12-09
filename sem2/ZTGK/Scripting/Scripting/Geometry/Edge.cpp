#include "Edge.h"

namespace duckApp
{
    Edge::Edge(int v1, int v2, int t1, int t2)
    {
        vertex_position_1 = v1;
        vertex_position_2 = v2;

        triangle_index_1 = t1;
        triangle_index_2 = t2;
    }
}
