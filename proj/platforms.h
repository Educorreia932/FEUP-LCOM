#pragma once

#include <lcom/lcf.h>

typedef struct Platforms Platforms_t;

Platforms_t* new_platforms();
Platforms_t* new_testing_platforms();
void free_platforms(Platforms_t *platforms);

void render_platforms(Platforms_t *plat);
