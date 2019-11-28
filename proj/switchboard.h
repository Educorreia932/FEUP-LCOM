#pragma once

#include <lcom/lcf.h>

#include "mouse_cursor.h"

typedef struct SwitchBoard SwitchBoard_t;

SwitchBoard_t* new_switchboard(MouseCursor_t* cursor);
void free_switchboard(SwitchBoard_t* s_board);

void switchboard_set_no_lasers(SwitchBoard_t* s_board);
void switchboard_set_two_lasers(SwitchBoard_t* s_board);
void switchboard_set_three_lasers(SwitchBoard_t* s_board);


void update_switchboard(SwitchBoard_t* s_board);
void render_switchboard(SwitchBoard_t* s_board);
