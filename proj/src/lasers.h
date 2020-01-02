#pragma once

#include <lcom/lcf.h>
#include "geometry.h"

typedef struct Laser Laser_t;

typedef struct Lasers Lasers_t;

Lasers_t* new_arcade_lasers();
Lasers_t* prototype_lasers();
void free_lasers(Lasers_t* lasers);

/**
 * @returns False, if not sucessful
 */
bool lasers_set_link_id(Lasers_t *lasers, uint8_t link);
void lasers_cycle_link_id(Lasers_t *lasers);

void render_lasers(Lasers_t* lasers);

bool player_is_dead(Lasers_t* lasers, Rect_t* rect);

void arcade_move_lasers(Lasers_t *lasers);
void arcade_add_laser(Lasers_t *lasers, int* laser_pos);

bool arcade_player_passes_lasers(Lasers_t* lasers, Rect_t* rect);
