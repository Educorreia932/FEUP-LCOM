#pragma once

#include <lcom/lcf.h>
#include "platforms.h"
#include "player.h"
#include "lasers.h"
#include "sprite.h"
#include "input_events.h"

typedef struct Level {
    Sprite_t *background;
    Platforms_t *platforms;
    //Lasers_t *lasers;
    Player_t* player;
} Level_t;

// typedef struct Level Level_t;

Level_t* new_level(const char* background_file_name);
Level_t* new_testing_level();
void free_level(Level_t *level);

// Actual level stuff

void update_level(Level_t* level, KbdInputEvents_t* kbd_ev, MouseInputEvents_t* mouse_ev) ;

void render_level(Level_t *level);
