#pragma once

#include <lcom/lcf.h>
#include "input_events.h"

typedef struct MouseCursor MouseCursor_t;

MouseCursor_t* new_cursor(MouseInputEvents_t *mouse_ev, const char* mouse_bmp_file_name);

void free_cursor(MouseCursor_t* cursor);

void update_cursor(MouseCursor_t* cursor);
void render_cursor(MouseCursor_t* cursor);
