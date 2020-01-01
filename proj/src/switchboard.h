#pragma once

#include <lcom/lcf.h>

#include "player.h"

typedef struct SwitchBoard SwitchBoard_t;

SwitchBoard_t* new_switchboard();
void free_switchboard(SwitchBoard_t* s_board);

void switchboard_set_no_lasers(SwitchBoard_t* s_board);
void switchboard_set_two_lasers(SwitchBoard_t* s_board);
void switchboard_set_three_lasers(SwitchBoard_t* s_board);

void switchboard_set_default_powers(SwitchBoard_t* sw, PlayerUnlockedPowers default_powers);
void switchboard_unlock_powers(SwitchBoard_t* sw, PlayerUnlockedPowers new_powers);
void switchboard_player_respawn(SwitchBoard_t* sw);

void update_switchboard(SwitchBoard_t* s_board);
void render_switchboard(SwitchBoard_t* s_board);
