#include <vector>
#include <iostream>
#include "body.hpp"
#include <astro_math.hpp>
#include "sim.hpp"

const double GRAVITY_CONST = 6.6743015e-11;
const int MAX_TRAIL_LENGTH = 4000;

void step(std::vector<astro::Body>& bodies, double dt)
{
  for (auto& body : bodies)
  {
    body.velocity += (body.acceleration * dt) / 2;
    body.pos += body.velocity * dt;
    body.acceleration = {0.0, 0.0, 0.0};

    body.trail.push_back(body.pos);
    if (body.trail.size() > MAX_TRAIL_LENGTH)
    {
      body.trail.pop_front();
    }

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
  for (const auto& body : state.bodies)
  {
    astro::Vector2_d pos = world_to_screen(body.pos, state);

    double minimum_pixel_size = 2;
    if (body.mass > 1e29) minimum_pixel_size = 26.0;
    else if (body.mass > 1e25) minimum_pixel_size = 12.0;
    else if (body.mass > 1e22) minimum_pixel_size = 4.0;

    double scaled_radius = std::max(body.radius * state.zoom, minimum_pixel_size);

    DrawCircle(pos.x, pos.y, scaled_radius, body.color);
  }
}

astro::Vector2_d world_to_screen(const astro::Vector3_d& pos, SimulationState& state)
{
  double x = pos.x - state.bodies[state.center_body_index].pos.x;
  double y = pos.y - state.bodies[state.center_body_index].pos.y;

  x *= state.zoom;
  y *= state.zoom;

  x += state.screenWidth / 2;
  y += state.screenHeight / 2;

  return {x, y};
}

void draw_trails(SimulationState& state)
{
  for (const auto& body : state.bodies)
  {
    for (size_t i = 1; i < body.trail.size(); i++)
    {
      astro::Vector2_d p1 = world_to_screen(body.trail[i-1], state);
      astro::Vector2_d p2 = world_to_screen(body.trail[i], state);

      DrawLine((int)p1.x, (int)p1.y, (int)p2.x, (int)p2.y, WHITE);
    }
  }
}

void handle_input(SimulationState& state)
{
  // body selection
  if (IsKeyDown(KEY_S)) state.center_body_index = 0;
  else if (IsKeyDown(KEY_E)) state.center_body_index = 1;
  else if (IsKeyDown(KEY_M)) state.center_body_index = 2;

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
    draw_trails(state);
  EndDrawing();
}

void update_sim(SimulationState& state)
{
  for (int i{}; i < state.sim_speed; i++)
  {
    step(state.bodies, state.dt);
  }
}