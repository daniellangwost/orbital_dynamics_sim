#include "astro_math.hpp"
#include "raylib.h"
#include <cmath>

namespace astro
{

    double Vector3_d::length_squared() const
    {
      return x*x + y*y + z*z;
    }

    double Vector3_d::length() const
    {
      return std::sqrt(x*x + y*y + z*z);
    }

    Vector3_d Vector3_d::normalize() const
    {
      double l = length();
      
      if (l > 0)
      {
        return {x / l, y / l, z / l};
      }
      return {0.0, 0.0, 0.0};
    }

    Vector3_d& Vector3_d::operator+= (const Vector3_d& other)
    {
      x += other.x;
      y += other.y;
      z += other.z;
      return *this;
    }

    Vector3_d& Vector3_d::operator*= (double s)
    {
      x *= s;
      y *= s;
      z *= s;
      return *this;
    }

    Vector3_d& Vector3_d::operator-= (const Vector3_d& other)
    {
      x -= other.x;
      y -= other.y;
      z -= other.z;
      return *this;
    }

    ::Vector3 Vector3_d::to_raylib() const
    {
      return {(float)x, (float)y, (float)z};
    }


  Vector3_d operator- (const Vector3_d& v1, const Vector3_d& v2)
  {
    return {v1.x - v2.x, v1.y - v2.y, v1.z - v2.z};
  }

  Vector3_d operator+ (const Vector3_d& v1, const Vector3_d& v2)
  {
    return {v1.x + v2.x, v1.y + v2.y, v1.z + v2.z};
  }

  Vector3_d operator* (double s, const Vector3_d& v)
  {
    return {v.x * s, v.y * s, v.z * s};
  }

  Vector3_d operator* (const Vector3_d& v, double s)
  {
    return s * v;
  }

  Vector3_d operator/ (const Vector3_d& v, double s)
  {
    if (s == 0) return {0.0, 0.0, 0.0};
    return {v.x * (1.0 / s), v.y * (1.0 / s), v.z * (1.0 / s)};
  }

}