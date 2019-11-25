#pragma once

#include <lcom/lcf.h>
#include "input_events.h"
#include "platforms.h"

typedef struct Player Player_t;

Player_t* new_player();
Player_t* new_testing_player();

void free_player(Player_t* player);

void player_movement(Player_t* player, Platforms_t* plat, KbdInputEvents_t* kbd_ev, MouseInputEvents_t* mouse_ev);

void render_player(Player_t* player);

