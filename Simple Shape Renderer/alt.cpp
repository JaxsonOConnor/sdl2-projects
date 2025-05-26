#include <iostream>
#include <SDL.h>
#include <vector>

constexpr int WIN_WIDTH = 600, WIN_HEIGHT = 600;

class Point {
public:
  typedef struct Coord {
    double x, y;
    Coord() : x(0.0), y(0.0) {};
    Coord(double x, double y) : x(x), y(y) {};
  } Coord;
  typedef struct Vector {
    double x, y, z;
    Vector() : x(0.0), y(0.0), z(0.0) {};
    Vector(double x, double y, double z) : x(x), y(y), z(z) {};
  } Vector;
  static Point::Vector Rotate(Point::Vector* vec, Point::Vector about, double x_deg, double y_deg, double z_deg) {
    double x_rad = x_deg * (M_PI / 180);
    double y_rad = y_deg * (M_PI / 180);
    double z_rad = z_deg * (M_PI / 180);
    double cosX = std::cos(x_rad), sinX = std::sin(x_rad);
    double cosY = std::cos(y_rad), sinY = std::sin(y_rad);
    double cosZ = std::cos(z_rad), sinZ = std::sin(z_rad);
    double dx = vec->x - about.x;
    double dy = vec->y - about.y;
    double dz = vec->z - about.z;
    
    vec->x = (dx * cosY * cosZ) + (dy * cosY * sinZ) + (dz * -sinY) + about.x;
    vec->y = (dx * (sinX * sinY * cosZ - cosX * sinZ)) + (dy * (sinX * sinY * sinZ + cosX * cosZ)) + (dz * sinX * cosY) + about.y;
    vec->z = (dx * (cosX * sinY * cosZ + sinX * sinZ)) + (dy * (cosX * sinY * sinZ - sinX * cosZ)) + (dz * cosX * cosY) + about.z;
    
    return *vec;
  }
  static double GetDistance(Point::Vector start, Point::Vector end) {
    return std::sqrt(std::pow(end.x - start.x, 2) + std::pow(end.y - start.y, 2) + std::pow(end.z - start.z, 2));
  }
};

class StaticRenderer {
private:
  static Point::Coord AbsoluteCoord(Point::Coord point) {
    return Point::Coord((WIN_WIDTH / 2) + point.x, (WIN_HEIGHT / 2) + point.y);
  }
public:
  static void Line(SDL_Renderer* renderer, Point::Coord start, Point::Coord end, Uint8 r, Uint8 g, Uint8 b) {
    SDL_SetRenderDrawColor(renderer, r, g, b, SDL_ALPHA_OPAQUE);
    start = AbsoluteCoord(start);
    end = AbsoluteCoord(end);
    SDL_RenderDrawLine(renderer, start.x, start.y, end.x, end.y);
  }
  static void Point(SDL_Renderer* renderer, Point::Coord point, int thickness, Uint8 r, Uint8 g, Uint8 b) {
    SDL_SetRenderDrawColor(renderer, r, g, b, SDL_ALPHA_OPAQUE);
    point = AbsoluteCoord(point);
    SDL_Rect rect = { point.x - thickness / 2, point.y - thickness / 2, thickness, thickness};
    SDL_RenderFillRect(renderer, &rect);
  }
};

int main(int argc, char* argv[]) {
  SDL_Window* window = SDL_CreateWindow("New Window", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIN_WIDTH, WIN_HEIGHT, NULL);
  SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  SDL_Event event;

  bool running = true;

  std::vector<Point::Vector*> shape = {
    new Point::Vector(50, 50, 50),
    new Point::Vector(-50, 50, 50),
    new Point::Vector(50, -50, 50),
    new Point::Vector(-50, -50, 50),
    new Point::Vector(50, 50, -50),
    new Point::Vector(-50, 50, -50),
    new Point::Vector(50, -50, -50),
    new Point::Vector(-50, -50, -50)
  };

  Point::Vector origin(0, 0, 0);

  float speed = 1;

  while (running) {

    // clear screen
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);

    // render frame
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    for (Point::Vector* vec : shape) {
      Point::Coord coord(vec->x, vec->y);
      StaticRenderer::Point(renderer, coord, 5, 0, 0, 0);
    }

    for (Point::Vector* vec : shape) {
      Point::Rotate(vec, origin, 1, 0, 1);
      //std::cout << vec->x << ", " << vec->y << ", " << vec->y << std::endl;
    }

    for (Point::Vector* vec1 : shape) {
      for (Point::Vector* vec2 : shape) {
        if (Point::GetDistance(*vec1, *vec2) < 110) {
          StaticRenderer::Line(renderer, Point::Coord(vec1->x, vec1->y), Point::Coord(vec2->x, vec2->y), 0, 0, 0);
        }
      }
    }
    
    // listen for events
    if (SDL_PollEvent(&event) != 0) {
      switch (event.type) {
      case SDL_QUIT:
        running = false;
        break;
      case SDL_KEYDOWN:
        if (event.key.keysym.scancode == 41) {
          running = false;
          break;
        }
        else {
          int scancode = event.key.keysym.scancode;
          if (scancode < 40 && scancode >= 30) {
            speed = scancode - 29;
          }
          std::cout << event.key.keysym.scancode << std::endl; // 30, 31, 32
        }
      }
    }
    SDL_Delay(16 / speed);
    SDL_RenderPresent(renderer);
  }

  return 0;
}
