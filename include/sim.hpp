# pragma once
#include <vector>
#include "astro_math.hpp"
#include "body.hpp"

struct SimulationState
{
  const int screenWidth = 1600;
  const int screenHeight = 900;
  double zoom = 2e-9;
  const double dt = 1000;
  int body_index = 0;
  const float zoom_speed = 1.1;
  std::vector<astro::Body> bodies;
  const int sim_speed = 30;
  const double min_zoom = 3e-10;
};

void step(std::vector<astro::Body>& bodies, double dt);
void calculate_accelerations(std::vector<astro::Body>& bodies);
astro::Vector3_d gravitational_force(const astro::Body& attractor, const astro::Body& target); 
void draw_bodies(SimulationState& state);
void handle_input(SimulationState& state);
void render(SimulationState& state);
void update_sim(SimulationState& state);