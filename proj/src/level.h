#pragma once

#include <lcom/lcf.h>
#include "sprite.h"
#include "player.h"
#include "platforms.h"
#include "lasers.h"
#include "spikes.h"
#include "power_ups.h"
#include "input_events.h"

typedef struct Level {
    Sprite_t* background;
    Platforms_t* platforms;
    Player_t* player;
    Lasers_t *lasers;
    Spikes_t* spikes;
    PowerUp_t* pu[3];
} Level_t;

// typedef struct Level Level_t;

Level_t* new_level();
Level_t* new_arcade_level();
Level_t* prototype_level(bool is_single_player);
void free_level(Level_t *level);

// Actual level stuff

void update_level(Level_t* level) ;
void update_arcade_level(Level_t* level);

void render_level(Level_t *level);
