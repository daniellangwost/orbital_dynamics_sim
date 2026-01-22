#include <vector>
#include <cmath>
#include <iostream>
#include "raylib.h"
#include "astro_math.hpp"
#include "body.hpp"
#include "main.hpp"

const double GRAVITY_CONST = 6.6743015e-11;

int main()
{
    const int screenWidth = 1600;
    const int screenHeight = 900;

    double zoom = 1e-6;

    InitWindow(screenWidth, screenHeight, "Orbital Mechanics Simulator");
    SetTargetFPS(60);

    std::vector<astro::Body> bodies;

    astro::Body sun{0.0, 0.0, 0.0, 1.989e30};
    sun.radius = 6.957e8;
    sun.color = YELLOW;

    astro::Body earth{1.496e11, 0.0, 0.0, 5.972e24};
    earth.radius = 6.371e6;
    earth.velocity = {0.0, 29780.0, 0.0};
    earth.color = BLUE;

    astro::Body moon{1.496e11 + 3.844e8, 0.0, 0.0, 7.348e22};
    moon.radius = 1.737e6;
    moon.velocity = {0.0, 29780 + 1022, 0.0};
    moon.color = GRAY;

    bodies.push_back(sun);
    bodies.push_back(earth);
    bodies.push_back(moon);

    calculate_accelerations(bodies);

    // set target to focus on
    astro::Vector3_d target_coord;

    int body_index = 0;
    const float zoom_speed = 1.1;
    while (!WindowShouldClose()) 
    {
      for (int i{}; i < 30; i++)
      {
        step(bodies, 60);
      }

      // set up center
      if (IsKeyDown(KEY_S)) body_index = 0;
      else if (IsKeyDown(KEY_E)) body_index = 1;
      else if (IsKeyDown(KEY_M)) body_index = 2;

      // zoom
      float mouse_wheel_movement = GetMouseWheelMove();
      if (mouse_wheel_movement != 0)
      {
        if (mouse_wheel_movement > 0) zoom *= zoom_speed;
        else zoom /= zoom_speed;
      }

      target_coord = bodies[body_index].pos;

      BeginDrawing();
        ClearBackground(BLACK);

        for (const auto& body : bodies)
        {
          std::cout << "Body position: (" << body.pos.x << "," << body.pos.y << ")\n"; 
          std::cout << "Target position: (" << target_coord.x << "," << target_coord.y << ")\n"; 
          // coordinates relative to center body
          double x = body.pos.x - target_coord.x;
          double y = body.pos.y - target_coord.y;
          
          std::cout << "Body Position after subtracting target: (" << x << "," << y << ")\n"; 

          // scale according to zoom
          x *= zoom;
          y *= zoom;

          std::cout << "Position after scaling factor: (" << x << "," << y << ")\n";

          double minimum_pixel_size = 2;
          if (body.mass > 1e29) minimum_pixel_size = 26.0;
          else if (body.mass > 1e25) minimum_pixel_size = 12.0;
          else if (body.mass > 1e22) minimum_pixel_size = 4.0;

          double scaled_radius = std::max(body.radius * zoom, minimum_pixel_size);


          std::cout << "Scaled radius: " << scaled_radius << "\n";

          // center on screen
          x += screenWidth / 2;
          y += screenHeight / 2;

          std::cout << "Position after centering on screen: (" << x << "," << y << ")\n";
          std::fflush(stdout);

          DrawCircle(x, y, scaled_radius, body.color);
        }
      EndDrawing();
    }

    CloseWindow();
    return 0;
}

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
  double softening_factor = 10e6;

  astro::Vector3_d displacement = attractor.pos - target.pos;
  double distance_squared = displacement.length_squared();
  astro::Vector3_d force_vector = displacement.normalize();
  
  double force = (GRAVITY_CONST * attractor.mass * target.mass) / (distance_squared + softening_factor);
  return force_vector *= force;
}