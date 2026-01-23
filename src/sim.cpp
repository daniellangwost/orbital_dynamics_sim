#include <vector>
#include <iostream>
#include "body.hpp"
#include "sim.hpp"

const double GRAVITY_CONST = 6.6743015e-11;

void step(std::vector<astro::Body>& bodies, double dt)
{
  for (auto& body : bodies)
  {
    body.velocity += (body.acceleration * dt) / 2;
    body.pos += body.velocity * dt;
    body.acceleration = {0.0, 0.0, 0.0};
  }

  calculate_accelerations(bodies);

  for (auto& body : bodies)
  {
    body.velocity += (body.acceleration * dt) / 2;
  }
}

void calculate_accelerations(std::vector<astro::Body>& bodies)
{
  int body_count = bodies.size();
  astro::Vector3_d force{};
  for (int i{}; i < body_count; i++)
  {
    for (int j{i+1}; j < body_count; j++)
    {
      force = gravitational_force(bodies[i], bodies[j]);
      bodies[j].acceleration += force / bodies[j].mass;
      bodies[i].acceleration -= force / bodies[i].mass;
    }
  }
}

astro::Vector3_d gravitational_force(const astro::Body& attractor, const astro::Body& target)
{
  double softening_factor = 5.0;

  astro::Vector3_d displacement = attractor.pos - target.pos;
  double distance_squared = displacement.length_squared();
  astro::Vector3_d force_vector = displacement.normalize();
  
  double force = (GRAVITY_CONST * attractor.mass * target.mass) / (distance_squared + softening_factor);
  return force_vector *= force;
}

void draw_bodies(SimulationState& state)
{
  astro::Vector3_d target_coord;
  target_coord = state.bodies[state.body_index].pos;
  for (const auto& body : state.bodies)
  {
    // coordinates relative to center body
    double x = body.pos.x - target_coord.x;
    double y = body.pos.y - target_coord.y;
    
    // scale according to zoom
    x *= state.zoom;
    y *= state.zoom;

    double minimum_pixel_size = 2;
    if (body.mass > 1e29) minimum_pixel_size = 26.0;
    else if (body.mass > 1e25) minimum_pixel_size = 12.0;
    else if (body.mass > 1e22) minimum_pixel_size = 4.0;

    double scaled_radius = std::max(body.radius * state.zoom, minimum_pixel_size);

    // center on screen
    x += state.screenWidth / 2;
    y += state.screenHeight / 2;

    DrawCircle(x, y, scaled_radius, body.color);
  }
}

void handle_input(SimulationState& state)
{
  // body selection
  if (IsKeyDown(KEY_S)) state.body_index = 0;
  else if (IsKeyDown(KEY_E)) state.body_index = 1;
  else if (IsKeyDown(KEY_M)) state.body_index = 2;

  // zoom
  float mouse_wheel_movement = GetMouseWheelMove();
  if (mouse_wheel_movement != 0)
  {
    if (mouse_wheel_movement > 0) state.zoom *= state.zoom_speed;
    else if (state.zoom > state.min_zoom) state.zoom /= state.zoom_speed;
  }
}

void render(SimulationState& state)
{
  BeginDrawing();
    ClearBackground(BLACK);
    draw_bodies(state);
  EndDrawing();
}

void update_sim(SimulationState& state)
{
  for (int i{}; i < state.sim_speed; i++)
  {
    step(state.bodies, state.dt);
  }
}