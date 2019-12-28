#pragma once

#include <lcom/lcf.h>
#include "input_events.h"
#include "platforms.h"
#include "lasers.h"
#include "spikes.h"

typedef struct Player Player_t;

Player_t* new_player(bool ui_controls, bool arcade_mode);

void free_player(Player_t* player);

void update_player(Player_t* player, Platforms_t* plat, Lasers_t* lasers, Spikes_t* spikes);

void animator_player(Player_t* player);
void render_player_background(Player_t* player);
void render_player_foreground(Player_t* player);
void render_player_ui(Player_t *player);
