#pragma once

#include <lcom/lcf.h>
#include "geometry.h"

#define COLOR_NO_MULTIPLY 0xFFFF
#define PATH "/home/lcom/labs/proj/assets/"

typedef struct Sprite Sprite_t;

/* SPRITE */

Sprite_t* new_sprite(float x_offset, float y_offset, int num, ...);
void free_sprite(Sprite_t *s);

void draw_sprite(Sprite_t *s, Rect_t *r, uint32_t color_to_multiply, bool reversed);
void draw_sprite_floats(Sprite_t *s, float x, float y, uint32_t color_to_multiply, bool reversed);

uint16_t sprite_get_width(Sprite_t *s);
uint16_t sprite_get_height(Sprite_t *s);

void set_animation_state(Sprite_t *s, uint8_t state);
uint8_t get_animation_state(const Sprite_t *s);

/* SPRITE DYNAMIC */

typedef struct SpriteDynamic SpriteDynamic_t;

SpriteDynamic_t* new_sprite_dynamic(const char* file_name, uint16_t dynamic_slice_size, uint16_t x_offset, uint16_t y_offset);
void free_sprite_dynamic(SpriteDynamic_t* sd);

void draw_sprite_dynamic(SpriteDynamic_t *sd, Rect_t *r, uint32_t color_to_multiply);
