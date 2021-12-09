#include "Vector.h"

namespace duckApp
{
    Vector::Vector() : x(0.0f), y(0.0f), z(0.0f) { }

    Vector::Vector(float _x, float _y, float _z) : x(_x), y(_y), z(_z) { }

    Vector Vector::operator +(const Vector a)
    {
        return Vector(this->x + a.x, this->y + a.y, this->z + a.z);
    }

    Vector Vector::operator *(const float a)
    {
        return Vector(this->x * a, this->y * a, this->z * a);
    }

    float Vector::Dot(Vector a, Vector b)
    {
        return a.x * b.x + a.y * b.y + a.z * b.z;
    }

    Vector Vector::Cross(Vector a, Vector b)
    {
        return Vector(a.y*b.z - a.z*b.y, a.z*b.x - a.x*b.z, a.x*b.y - a.y*b.x);
    }
}
