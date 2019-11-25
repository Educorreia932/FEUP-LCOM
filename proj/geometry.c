#include "geometry.h"
#include <math.h>
#include "math.h"

// Vec2d constructors
Vec2d_t vec2d(float x, float y) {
  const Vec2d_t v = {x, y};
  return v;
}

// Vec2d operations
float point_distance(Vec2d_t *p1, Vec2d_t *p2) {
  return Q_rsqrt( fsquare(p1->x - p2->x) + fsquare(p1->y - p2->y) );
}


/* RECT CONSTRUCTORS */

Rect_t rect(float x, float y, float w, float h) {
  const Rect_t r = {x, y, w, h};
  return r;
}

Rect_t rect_from_uints(uint16_t x,uint16_t y, uint16_t w, uint16_t h) {
  const Rect_t r = {(float) x, (float) y, (float) w, (float) h};
  return r;
}

Rect_t rect_from_vec2d(Vec2d_t point, Vec2d_t size) {
  const Rect_t r = {point.x, point.y, size.x, size.y};
  return r;
}

Rect_t rect_from_points(Vec2d_t p1, Vec2d_t p2) {
  float xmin = fmin(p1.x, p2.x);
  float xmax = fmax(p1.x, p2.x);
  float ymin = fmin(p1.y, p2.y);
  float ymax = fmax(p1.y, p2.y);

  const Rect_t r = {
      xmin, 
      ymin, 
      fmax(0.0f, xmax - xmin),
      fmax(0.0f, ymax - ymin)
    };

  return r;
}

Rect_t* new_rect(float x, float y, float w, float h) {
  Rect_t* rect = (Rect_t*) malloc(sizeof(Rect_t));
  if (rect == NULL) {
    printf("new_rect: Failed to allocate memory for the Rect object\n");
    return NULL;
  }

  rect->x = x;
  rect->y = y;
  rect->w = w;
  rect->h = h;

  return rect;
}

void free_rect(Rect_t* rect) {
  free(rect);
}

/* RECT METHODS */

inline bool rect_collision(Rect_t *r1, Rect_t *r2) {
  return r1->x < r2->x + r2->w
    && r1->x + r1->w > r2->x
    && r1->y < r2->y + r2->h
    && r1->y + r1->h > r2->y;
}
