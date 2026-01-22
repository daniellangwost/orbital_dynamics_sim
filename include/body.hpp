# pragma once
#include "astro_math.hpp"

namespace astro
{
  struct Body
  {
    double mass{};
    double radius{1.0};
    astro::Vector3_d pos{0.0, 0.0, 0.0};
    astro::Vector3_d velocity{0.0, 0.0, 0.0};
    astro::Vector3_d acceleration{0.0, 0.0, 0.0};
    Color color{WHITE};

    Body(double x, double y, double z, double m) 
    : mass{m}
    , pos{x, y, z}
    {
    }
  };
}
