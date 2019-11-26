#pragma once

#include <lcom/lcf.h>
#include "geometry.h"

typedef struct Laser Laser_t;

typedef struct Lasers Lasers_t;

Lasers_t* new_testing_lasers();

void render_lasers(Lasers_t* lasers);

bool player_is_dead(Lasers_t* lasers, Rect_t* rect);
