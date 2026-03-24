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
  ViewState view;
  init_window(view);
  init_bodies(state);

  while (!WindowShouldClose()) 
  {
    handle_input(state, view);
    update_sim(state);
    render(state, view);
  }
  CloseWindow();
  return 0;
}