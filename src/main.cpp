#include <vector>
#include <cmath>
#include <iostream>
#include "raylib.h"
#include "astro_math.hpp"
#include "body.hpp"
#include "sim.hpp"
#include "init.hpp"

int main()
{
  SimulationState state;
  init_window(state);
  init_bodies(state);

  while (!WindowShouldClose()) 
  {
    handle_input(state);
    update_sim(state);
    render(state);
  }
  CloseWindow();
  return 0;
}