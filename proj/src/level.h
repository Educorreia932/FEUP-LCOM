#pragma once

#include <lcom/lcf.h>
#include "sprite.h"
#include "player.h"
#include "platforms.h"
#include "lasers.h"
#include "spikes.h"
#include "input_events.h"

typedef struct Level {
    Sprite_t *background;
    Platforms_t *platforms;
    Player_t* player;
    Lasers_t *lasers;
    Spikes_t* spikes;
} Level_t;

// typedef struct Level Level_t;

Level_t* new_level();
Level_t* prototype_level(bool is_single_player);
void free_level(Level_t *level);

// Actual level stuff

void update_level(Level_t* level, KbdInputEvents_t* kbd_ev, MouseInputEvents_t* mouse_ev) ;

void render_level(Level_t *level);
