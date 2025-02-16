#include <stdio.h>
#include <SDL2/SDL.h>
#include <math.h>

#define WIDTH 900
#define HEIGHT 600
#define COLOR_WHITE 0xffffffff
#define COLOR_BLACK 0x00000000
#define COLOR_RED 0xffff0000
#define RAYS_NUMBER 50

struct Circle {
  double x, y, radius;
};

struct Ray {
  double x_start, y_start;
  double angle;
};

void FillCircle(SDL_Surface *surface, struct Circle circle) {
  for (int x = circle.x - circle.radius; x <= circle.x + circle.radius; x++) {
    for (int y = circle.y - circle.radius; y <= circle.y + circle.radius; y++) {
      double distance = sqrt(pow(x - circle.x, 2) + pow(y - circle.y, 2));
      if (distance < circle.radius) {
        SDL_Rect pixel = (SDL_Rect) {x, y, 1, 1};
        SDL_FillRect(surface, &pixel, COLOR_WHITE);
      }
    }
  }
}

void generate_rays(struct Circle circle, struct Ray rays[RAYS_NUMBER]) {
  double radian_step = (2 * M_PI) / RAYS_NUMBER;
  for (int i=0; i<RAYS_NUMBER; i++) {
    struct Ray ray = {circle.x, circle.y, i * radian_step};
    rays[i] = ray;
  }
}

void FillRays(SDL_Surface *surface, struct Ray rays[RAYS_NUMBER], struct Circle object) {
  for (int i=0; i<RAYS_NUMBER; i++) {
    struct Ray ray = rays[i];
    
    int end_of_screen = 0;
    int object_hit = 0;
    
    double x_draw = ray.x_start;
    double y_draw = ray.y_start;

    int step = 1;

    while (!end_of_screen && !object_hit) {
      x_draw += step * cos(ray.angle);
      y_draw += step * sin(ray.angle);

      SDL_Rect pixel = (SDL_Rect) {(int) x_draw, (int) y_draw, 1, 1};
      SDL_FillRect(surface, &pixel, COLOR_RED);

      if (x_draw < 0 || x_draw > WIDTH || y_draw < 0 || y_draw > HEIGHT)
        end_of_screen = 1;
      
      // check for object & ray collision
      double distance = sqrt(pow(x_draw - object.x, 2) + pow(y_draw - object.y, 2));
      if (distance < object.radius)
        object_hit = 1;
    }
  }
}

int main(int argc, char *argv[]) {
  SDL_Init(SDL_INIT_VIDEO);
  SDL_Window *window = SDL_CreateWindow("SDL2 Raycasting", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, SDL_WINDOW_BORDERLESS);
  SDL_Surface *surface = SDL_GetWindowSurface(window);
  SDL_Event event;

  SDL_Rect erase_rect = (SDL_Rect) {0, 0, WIDTH, HEIGHT};
  struct Circle circle = {200, 200, 80};
  struct Circle object = {600, 250, 120};
  
  int simulation_running = 1;

  while (simulation_running) {
    // DRAW THINGS
    SDL_FillRect(surface, &erase_rect, COLOR_BLACK);

    struct Ray rays[RAYS_NUMBER];
    generate_rays(circle, rays);
    FillRays(surface, rays, object);

    FillCircle(surface, circle);
    FillCircle(surface, object);
    
    // LISTEN FOR EVENTS
    if (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
        simulation_running = 0;
      } else if (event.type == SDL_KEYDOWN) {
        if (event.key.keysym.scancode == 41) {
          simulation_running = 0;
        }
      } else if (event.type == SDL_MOUSEMOTION) {
        if (event.motion.state != 0) {
          circle.x = event.motion.x;
          circle.y = event.motion.y;
        }
      }
    }
    SDL_UpdateWindowSurface(window);
  }

  SDL_DestroyWindow(window);
  SDL_Quit();
  
  return 0;
}
