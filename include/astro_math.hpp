# pragma once
#include "raylib.h"

namespace astro
{
    struct Vector2_d
    {
        double x{0.0}, y{0.0};
    };

    struct Vector3_d
    {
        double x{0.0}, y{0.0}, z{0.0};

        double length_squared() const;
        double length() const;
        Vector3_d normalize() const;

        Vector3_d& operator+=(const Vector3_d& other);
        Vector3_d& operator-=(const Vector3_d& other);
        Vector3_d& operator*=(double s);

        ::Vector3 to_raylib() const;
    };

    Vector3_d operator-(const Vector3_d& v1, const Vector3_d& v2);
    Vector3_d operator+(const Vector3_d& v1, const Vector3_d& v2);
    Vector3_d operator*(double s, const Vector3_d& v);
    Vector3_d operator*(const Vector3_d& v, double s);
    Vector3_d operator/(const Vector3_d& v, double s);
}