#pragma once

#include <lcom/lcf.h>
#include "geometry.h"

typedef struct Platforms Platforms_t;

Platforms_t* new_platforms();
Platforms_t* new_testing_platforms();
void free_platforms(Platforms_t *platforms);

bool does_collide_platforms(Platforms_t* plat, Rect_t* rect);
void render_platforms(Platforms_t *plat);
