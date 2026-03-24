# pragma once
#include <vector>
#include <deque>
#include "raylib.h"
#include "astro_math.hpp"
#include "body.hpp"



struct OrbitTracker
{
  int reference_idx{};
  int orbiting_idx{};
  double periapsis{};
  double apoapsis{};
  double delta_r_prev{};
  double r_prev{};
  int event_detected = 0; // 0 = no event, 1 = periapsis, 2 = apoapsis
  int state{}; // 0 = no data yet, 1 = one distance tracker, 2 = fully initialized
};

struct SimulationState
{
  const int screenWidth = 1600;
  const int screenHeight = 900;
  double zoom = 2e-9;
  const double dt = 1000;
  int center_body_index = 0;
  const float zoom_speed = 1.1;
  std::vector<astro::Body> bodies;
  const int sim_speed = 50;
  const double min_zoom = 3e-10;
  bool creating_body = false;
  Vector2 initial_mouse_pos{0.0, 0.0};
  OrbitTracker orbit_tracker {.reference_idx = 0, .orbiting_idx = 1};
};

struct Energies
{
  double potential_energy{};
  double kinetic_energy{};
};

void step(std::vector<astro::Body>& bodies, double dt);
void calculate_accelerations(std::vector<astro::Body>& bodies);
astro::Vector3_d gravitational_force(const astro::Body& attractor, const astro::Body& target); 
Energies compute_total_energy(SimulationState& state);
void log_energies(Energies& energies);
void update_orbit_tracker(SimulationState& state);
void log_orbit_event(SimulationState& state);
void draw_bodies(SimulationState& state);
void handle_input(SimulationState& state);
void render(SimulationState& state);
void update_sim(SimulationState& state);
Vector2 world_to_screen(const astro::Vector3_d& pos, SimulationState& state);
