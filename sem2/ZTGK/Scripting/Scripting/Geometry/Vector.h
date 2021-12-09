#pragma once

namespace duckApp
{
    struct Vector
    {
    public:
        float x;
        float y;
        float z;

        Vector();

        Vector(float _x, float _y, float _z);

        Vector operator +(const Vector a);
        Vector operator *(const float a);

        static float Dot(Vector a, Vector b);
        static Vector Cross(Vector a, Vector b);
    };
}
