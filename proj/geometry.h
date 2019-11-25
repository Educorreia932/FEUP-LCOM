#pragma once

#include <lcom/lcf.h>a

#define deltatime 0.03333333f // Time between frames

typedef struct Vec2d{
    float x, y;
} Vec2d_t;

// Vec2d constructors
Vec2d_t vec2d(float x, float y);

// Vec2d methods
float point_distance(Vec2d_t *p1, Vec2d_t *p2);

typedef struct Rect {
    float x, y, w, h;
} Rect_t;

// Rect constructors
Rect_t rect(float x, float y, float w, float h);
Rect_t rect_from_uints(uint16_t x,uint16_t y, uint16_t w, uint16_t h);
Rect_t rect_from_vec2d(Vec2d_t point, Vec2d_t size) ;
Rect_t rect_from_points(Vec2d_t p1, Vec2d_t p2);

Rect_t* new_rect(float x, float y, float w, float h);
void free_rect(Rect_t* rect);

// Rect methods
bool rect_collision(Rect_t *r1, Rect_t *r2);
