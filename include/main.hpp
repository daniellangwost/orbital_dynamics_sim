# pragma once
#include <vector>
#include "astro_math.hpp"
#include "body.hpp"


void step(std::vector<astro::Body>& bodies, double dt);
void calculate_accelerations(std::vector<astro::Body>& bodies);
astro::Vector3_d gravitational_force(const astro::Body& attractor, const astro::Body& target); 
