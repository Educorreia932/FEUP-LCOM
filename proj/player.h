#pragma once

#include <lcom/lcf.h>

typedef struct Player Player_t;

Player_t* new_player();
Player_t* new_testing_player();

void free_player(Player_t* player);

void render_player(Player_t* player);
