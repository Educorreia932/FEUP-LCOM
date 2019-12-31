#pragma once

#include <lcom/lcf.h>
#include "geometry.h"

#define MAX_POWERUPS 5

typedef struct PowerUp PowerUp_t;

PowerUp_t* new_power_up(const char* sprite_file_name, Vec2d_t pos, void (*function)());

void free_power_up(PowerUp_t *pu);

void update_power_up(PowerUp_t *pu, Rect_t *player_rect);

void render_power_up(PowerUp_t *pu);

void collect_powerup(PowerUp_t *pu);
void respawn_powerup(PowerUp_t *pu);

int gets_powerup(PowerUp_t *pu, Rect_t* rect);
