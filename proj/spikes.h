#pragma once

#include <lcom/lcf.h>

#include "geometry.h"

typedef struct Spikes Spikes_t;

Spikes_t* prototype_spikes();

void render_spikes(Spikes_t* spikes);

bool player_touches_spike(Spikes_t* spikes, Rect_t* player);
