#pragma once

#include <lcom/lcf.h>
#include "input_events.h"
#include "geometry.h"

typedef struct MouseCursor MouseCursor_t;

/* CONSTRUCTORS */

MouseCursor_t* new_cursor(MouseInputEvents_t *mouse_ev);
void free_cursor(MouseCursor_t* cursor);

/* Activate and deactivate Cursor rendering */

void cursor_hide(MouseCursor_t* cursor);
void cursor_show(MouseCursor_t* cursor);
// Returns true if the cursor is currently being rendered
bool cursor_is_shown(MouseCursor_t* cursor);


/* CURSOR METHODS */

void update_cursor(MouseCursor_t* cursor);
void render_cursor(MouseCursor_t* cursor);

bool is_cursor_inside_rect(MouseCursor_t* cursor, Rect_t* rect);
bool cursor_left_button_down(MouseCursor_t* cursor);
bool cursor_left_button(MouseCursor_t* cursor);
bool cursor_right_button_down(MouseCursor_t* cursor);
bool cursor_right_button(MouseCursor_t* cursor);
float cursor_get_x(MouseCursor_t* cursor);
float cursor_get_y(MouseCursor_t* cursor);
Vec2d_t cursor_get_pos(MouseCursor_t* cursor);
