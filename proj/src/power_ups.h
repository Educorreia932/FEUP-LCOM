#pragma once

#include <lcom/lcf.h>
#include "geometry.h"

typedef struct PowerUp PowerUp_t;

PowerUp_t* new_power_up(const char* sprite_file_name, Rect_t rect, void (*function)());
void free_power_up(PowerUp_t *pu);

void update_power_up(PowerUp_t *pu, Rect_t *player_rect);
void render_power_up(PowerUp_t *pu);
