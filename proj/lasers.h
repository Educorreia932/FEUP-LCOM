#pragma once

#include <lcom/lcf.h>
#include "geometry.h"

typedef struct Laser Laser_t;

typedef struct Lasers Lasers_t;

Lasers_t* prototype_lasers();
void free_lasers(Lasers_t* lasers);

// Returns false if not successful
bool lasers_set_link_id(Lasers_t *lasers, uint8_t link);
void lasers_cycle_link_id(Lasers_t *lasers);

void render_lasers(Lasers_t* lasers);

bool player_is_dead(Lasers_t* lasers, Rect_t* rect);
