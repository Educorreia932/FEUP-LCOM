#pragma once

#include <lcom/lcf.h>
#include "geometry.h"

/* CONSTRUCTORS */

void initialize_cursor();
void free_cursor();

/* Activate and deactivate Cursor rendering */

void cursor_hide();
void cursor_show();
// Returns true if the cursor is currently being rendered
bool cursor_is_shown();


/* CURSOR METHODS */

void update_cursor();
void render_cursor();

bool is_cursor_inside_rect(Rect_t* rect);
float cursor_get_x();
float cursor_get_y();
Vec2d_t cursor_get_pos();
