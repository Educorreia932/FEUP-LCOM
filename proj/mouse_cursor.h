#pragma once

#include <lcom/lcf.h>
#include "input_events.h"

typedef struct MouseCursor MouseCursor_t;

/* CONSTRUCTORS */

MouseCursor_t* new_cursor(MouseInputEvents_t *mouse_ev, const char* mouse_bmp_file_name);
void free_cursor(MouseCursor_t* cursor);

/* Activate and deactivate Cursor rendering */

void cursor_hide(MouseCursor_t* cursor);
void cursor_show(MouseCursor_t* cursor);
// Returns true if the cursor is currently being rendered
bool cursor_is_active(MouseCursor_t* cursor);


/* CURSOR METHODS */

void update_cursor(MouseCursor_t* cursor);
void render_cursor(MouseCursor_t* cursor);
