#include <vector>
#include "init.hpp"
#include "body.hpp"
#include "sim.hpp"

void init_window(ViewState& view)
{
  InitWindow(view.screenWidth, view.screenHeight, "Orbital Mechanics Simulator");
  SetTargetFPS(60);
}

void init_bodies(SimulationState& state)
{
  astro::Body sun{0.0, 0.0, 0.0, 1.989e30};
  sun.radius = 6.957e8;
  sun.color = YELLOW;

  astro::Body earth{1.496e11, 0.0, 0.0, 5.972e24};
  earth.radius = 6.371e6;
  earth.velocity = {0.0, 29780.0, 0.0};
  earth.color = BLUE;

  // astro::Body moon{1.496e11 + 3.844e8, 0.0, 0.0, 7.348e22};
  // moon.radius = 1.737e6;
  // moon.velocity = {0.0, 29780 + 1022, 0.0};
  // moon.color = GRAY;

  state.bodies.push_back(sun);
  state.bodies.push_back(earth);
  // state.bodies.push_back(moon);

  calculate_accelerations(state.bodies);
}