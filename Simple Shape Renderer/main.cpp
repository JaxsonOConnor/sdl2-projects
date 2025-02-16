#include <iostream>
#include <SDL2/SDL.h>
#include <vector>
#include <cmath>

const int W = 500, H = 500;
const int FOV = 270;
SDL_Window *window; SDL_Renderer *renderer; SDL_Event event; bool running = true;

typedef struct point2d { float x, y; point2d(float x_, float y_) : x(x_), y(y_) {} } point2d;
typedef struct point3d { float x, y, z; point3d(float x_, float y_, float z_) : x(x_), y(y_), z(z_) {} } point3d;
typedef struct color { Uint8 r, g, b; color(Uint8 r_, Uint8 g_, Uint8 b_) : r(r_), g(g_), b(b_) {} } color;

point2d getProjected(point3d p) { return point2d((p.x * FOV) / (p.z + FOV), (p.y * FOV) / (p.z + FOV)); };
float getDistance(point3d p1, point3d p2) { return (float) std::sqrt(std::pow(p2.x - p1.x, 2) + std::pow(p2.y - p1.y, 2) + std::pow(p2.z - p1.z, 2)); };

void rotate3d(point3d* p, point3d c, float x, float y, float z) {
  x *= (float) (M_PI / 180.0f);
  y *= (float) (M_PI / 180.0f);
  z *= (float) (M_PI / 180.0f);

  float cosX = std::cos(x), sinX = std::sin(x), dx = p->x - c.x;
  float cosY = std::cos(x), sinY = std::sin(y), dy = p->y - c.y;
  float cosZ = std::cos(x), sinZ = std::sin(z), dz = p->z - c.z;

  p->x = static_cast<float>((dx * (cosY * cosZ)) + (dy * (cosY * sinZ)) + (dz * (-sinY)) + c.x);
  p->y = static_cast<float>((dx * (sinX * sinY * cosZ - cosX * sinZ)) + (dy * (sinX * sinY * sinZ + cosX * cosZ)) + (dz * (sinX * cosY)) + c.y);
  p->z = static_cast<float>((dx * (cosX * sinY * cosZ + sinX * sinZ)) + (dy * (cosX * sinY * sinZ - sinX * cosZ)) + (dz * (cosX * cosY)) + c.z);
}
void rotate3d(std::vector<point3d*>& pvec, point3d c, float x, float y, float z) { for (point3d* p : pvec) {rotate3d(p, c, x, y, z);}; };

void Line(point3d p1, point3d p2, color c) {
  point2d p1P = getProjected(p1); point2d p2P = getProjected(p2);
  SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, SDL_ALPHA_OPAQUE);
  SDL_RenderDrawLine(renderer, (W/2) + p1P.x, (H/2) + p1P.y, (W/2) + p2P.x, (H/2) + p2P.y);
};

void Line(point3d p1, point3d p2, color c, float distance) { if (getDistance(p1, p2) <= distance) { Line(p1, p2, c); } };

int main(int argc, char *argv[]) {
  int result = SDL_CreateWindowAndRenderer(W, H, SDL_WINDOW_ALLOW_HIGHDPI, &window, &renderer);
  if (result) { std::cerr << "Couldn't create window and/or renderer: " << SDL_GetError() << std::endl; return 1; }
  
  std::vector<point3d*> cube = {
    new point3d(-50, -50, 50),
    new point3d(50, -50, 50),
    new point3d(-50, 50, 50),
    new point3d(50, 50, 50),
    new point3d(-50, -50, -50),
    new point3d(50, -50, -50),
    new point3d(-50, 50, -50),
    new point3d(50, 50, -50)
  };
  
  point3d center(0, 0, 0);

  while (running) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);
    
    for (int i=0; i<cube.size(); i++) {
      for (int j=0; j<cube.size(); j++) {
        if (i != j) {
          std::cout << getDistance(*cube.at(i), *cube.at(j)) << std::endl;
          Line(*cube.at(i), *cube.at(j), color(255, 255, 255), 120);
        }
      }
    }

    rotate3d(cube, center, 0.5, 0.67, 0.9);

    if (SDL_PollEvent(&event)) {
      switch(event.type) {
        case SDL_QUIT:
          running = false;
        case SDL_KEYDOWN:
          if (event.key.keysym.scancode == 41) { running = false; }
      }
    }

    SDL_Delay(16);
    SDL_RenderPresent(renderer);
  }

  SDL_DestroyWindow(window);
  SDL_DestroyRenderer(renderer);
  SDL_Quit();

  return 0;
}