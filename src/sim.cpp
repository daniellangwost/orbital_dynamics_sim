#include <vector>
#include <iostream>
#include <cmath>
#include "astro_math.hpp"
#include "raymath.h"
#include "body.hpp"
#include "sim.hpp"

const double GRAVITY_CONST = 6.6743015e-11;
const int MAX_TRAIL_LENGTH = 4000;

void step(SimulationState& state)
{
  for (auto& body : state.bodies)
  {
    body.velocity += (body.acceleration * state.dt) / 2;
    body.pos += body.velocity * state.dt;
    body.acceleration = {0.0, 0.0, 0.0};

    body.trail.push_back(body.pos);
    if (body.trail.size() > MAX_TRAIL_LENGTH)
    {
      body.trail.pop_front();
    }

  }

  calculate_accelerations(state.bodies);

  for (auto& body : state.bodies)
  {
    body.velocity += (body.acceleration * state.dt) / 2;
  }

  Energies current_energies = compute_total_energy(state);
  // log_energies(current_energies);
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

Energies compute_total_energy(SimulationState& state)
{
  double kinetic_energy = 0;
  double potential_energy = 0;
  for (size_t i = 0; i < state.bodies.size(); i++)
  {
    kinetic_energy += (1.0/2.0) * state.bodies[i].mass * state.bodies[i].velocity.length() * state.bodies[i].velocity.length();
    for (size_t j = i+1; j < state.bodies.size(); j++)
    {
      double force = gravitational_force(state.bodies[i], state.bodies[j]).length();
      potential_energy += force * (state.bodies[i].pos - state.bodies[j].pos).length();
    }
  }
  Energies energies;
  energies.kinetic_energy = kinetic_energy;
  energies.potential_energy = potential_energy;
  return energies;
}

void log_energies(Energies& energies)
{
  TraceLog(LOG_INFO, "Current Potential Energy: %f", energies.potential_energy);
  TraceLog(LOG_INFO, "Current Kinetic Energy: %f", energies.kinetic_energy);
  TraceLog(LOG_INFO, "Current TOTAL Energy: %f", energies.potential_energy + energies.kinetic_energy);
}

void update_orbit_tracker(SimulationState& state)
{
  OrbitTracker& tracker = state.orbit_tracker;
  tracker.event_detected = 0;
  double current_distance = (state.bodies[tracker.orbiting_idx].pos - state.bodies[tracker.reference_idx].pos).length();
  if (tracker.state == 0)
  {
    tracker.r_prev = current_distance;
    tracker.state = 1;
  }
  else if (tracker.state == 1)
  {
    tracker.delta_r_prev = current_distance - tracker.r_prev;
    tracker.r_prev = current_distance;
    tracker.state = 2;
  }
  else
  {
    double delta_current = current_distance - tracker.r_prev;
    if (tracker.delta_r_prev < 0 && delta_current > 0) // periapsis
    {
      tracker.event_detected = 1;
      tracker.periapsis = tracker.r_prev;
    }
    else if (tracker.delta_r_prev > 0 && delta_current < 0) // apoapsis
    {
      tracker.event_detected = 2;
      tracker.apoapsis = tracker.r_prev;
    }
    tracker.delta_r_prev = delta_current;
    tracker.r_prev = current_distance;   
  }
}

void log_orbit_event(SimulationState& state)
{
  OrbitTracker& tracker = state.orbit_tracker;
  if (tracker.event_detected == 1)
    TraceLog(LOG_INFO, "Detected Periapsis: %f", tracker.periapsis);
  else if (tracker.event_detected == 2)
    TraceLog(LOG_INFO, "Detected Apoapsis: %f", tracker.apoapsis);
}

void create_body(SimulationState& state, const astro::Vector3_d pos, const astro::Vector3_d velocity, double mass)
{
  astro::Body b{pos.x, pos.y, pos.z, mass};
  b.velocity = velocity;
  b.color = WHITE;
  b.radius = cbrt(mass) * 1e-7;
  state.bodies.push_back(b);
}

void draw_bodies(SimulationState& state, ViewState& view)
{
  for (const auto& body : state.bodies)
  {
    Vector2 pos = world_to_screen(body.pos, state, view);

    double minimum_pixel_size = 2;
    if (body.mass > 1e29) minimum_pixel_size = 26.0;
    else if (body.mass > 1e25) minimum_pixel_size = 12.0;
    else if (body.mass > 1e22) minimum_pixel_size = 4.0;

    double scaled_radius = std::max(body.radius * view.zoom, minimum_pixel_size);

    DrawCircle(pos.x, pos.y, scaled_radius, body.color);
  }
}

Vector2 world_to_screen(const astro::Vector3_d& pos, const SimulationState& state, const ViewState& view)
{
  double x = pos.x - state.bodies[view.center_body_index].pos.x;
  double y = pos.y - state.bodies[view.center_body_index].pos.y;

  x *= view.zoom;
  y *= view.zoom;

  x += view.screenWidth / 2;
  y += view.screenHeight / 2;

  return {(float)x, (float)y};
}

// always sets z to 0 since there is no way to represent that on screen
astro::Vector3_d screen_to_world(const Vector2& screen_pos, const SimulationState& state, const ViewState& view)
{
  double x = (screen_pos.x - view.screenWidth / 2) / view.zoom;
  double y = (screen_pos.y - view.screenHeight / 2) / view.zoom;

  x += state.bodies[view.center_body_index].pos.x;
  y += state.bodies[view.center_body_index].pos.y;

  return {x, y, 0.0};
}

void draw_trails(SimulationState& state, ViewState& view)
{
  for (const auto& body : state.bodies)
  {
    for (size_t i = 1; i < body.trail.size(); i++)
    {
      Vector2 p1 = world_to_screen(body.trail[i-1], state, view);
      Vector2 p2 = world_to_screen(body.trail[i], state, view);

      DrawLine((int)p1.x, (int)p1.y, (int)p2.x, (int)p2.y, WHITE);
    }
  }
}

void draw_hud(SimulationState& state, ViewState& view)
{
  double distance = (state.bodies[0].pos - state.bodies[1].pos).length();
  DrawRectangleRounded({10, 10, 400, 180}, 0.3, 0, GRAY);
  DrawText(TextFormat("Speed: %.2f m/s", state.bodies[1].velocity.length()), 40, 30, 30, WHITE);
  DrawText(TextFormat("Distance: %.3e m", distance), 40, 60, 30, WHITE);

  // apoapsis and periapsis
  OrbitTracker& tracker = state.orbit_tracker;
  if (tracker.state == 2)
  {
    if (tracker.periapsis != 0) DrawText(TextFormat("Periapsis: %.3e m", tracker.periapsis), 40, 90, 30, WHITE);
    else DrawText("Periapsis: ---", 40, 90, 30, WHITE);
    if (tracker.apoapsis != 0) DrawText(TextFormat("Apoapsis: %.3e m", tracker.apoapsis), 40, 120, 30, WHITE);
    else DrawText("Apoapsis: ---", 40, 120, 30, WHITE);  
  }

}

void clear_bodies(SimulationState& state)
{
  // delete all bodies except the first one, which is the sun
  if (state.bodies.size() > 1)
  {
    state.bodies.erase(state.bodies.begin() + 1, state.bodies.end());
  }
}

void handle_input(SimulationState& state, ViewState& view)
{
  // body selection
  if (IsKeyDown(KEY_S)) view.center_body_index = 0;
  else if (IsKeyDown(KEY_E)) view.center_body_index = 1;
  else if (IsKeyDown(KEY_M)) view.center_body_index = 2;

  if (IsKeyDown(KEY_C)) clear_bodies(state);

  // zoom
  float mouse_wheel_movement = GetMouseWheelMove();
  if (mouse_wheel_movement != 0)
  {
    if (mouse_wheel_movement > 0) view.zoom *= view.zoom_speed;
    else if (view.zoom > view.min_zoom) view.zoom /= view.zoom_speed;
  }
  

  Vector2 mouse_pos = GetMousePosition();
  if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
  {
    view.creating_body = true;
    view.initial_mouse_pos = GetMousePosition();
  }

  if (view.creating_body)
  {
    DrawLineV(view.initial_mouse_pos, mouse_pos, RED);
    if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
    {
      float dx = view.initial_mouse_pos.x - mouse_pos.x;
      float dy = view.initial_mouse_pos.y - mouse_pos.y;
      astro::Vector3_d velocity = {(dx / view.zoom) * 0.000001, (dy / view.zoom) * 0.000001, 0.0};
      create_body(state, screen_to_world(view.initial_mouse_pos, state, view), velocity, 1e24);
      view.creating_body = false;
      view.initial_mouse_pos = {0.0, 0.0};
    }
  }

}

void render(SimulationState& state, ViewState view)
{
  BeginDrawing();
    ClearBackground(BLACK);
    draw_bodies(state, view);
    draw_trails(state, view);
    draw_hud(state, view);
  EndDrawing();
}

void update_sim(SimulationState& state)
{
  for (int i{}; i < state.sim_speed; i++)
  {
    step(state);
    update_orbit_tracker(state);
    if (state.orbit_tracker.event_detected)
    {
      log_orbit_event(state);
      state.orbit_tracker.event_detected = 0;
    }
  }
}