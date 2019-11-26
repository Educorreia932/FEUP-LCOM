#include "platforms.h"
#include "geometry.h"
#include "sprite.h"

#define PLATFORMS_COLOR 0xb60e
#define WALLS_COLOR 0x11e2

struct Platforms {
    Rect_t *rects;
    bool *is_wall;
    uint8_t size;
    SpriteDynamic_t* platform;
    SpriteDynamic_t* wall;
};

// TODO: ALL OF THIS
Platforms_t* new_platforms() {
  return NULL;
}

Platforms_t* new_testing_platforms() {
  Platforms_t *plat = (Platforms_t*) malloc(sizeof(Platforms_t));
  
  plat->wall = new_sprite_dynamic("/home/lcom/labs/proj/assets/wall_dynamic.bmp" , 8, 0, 0);
  if (plat->wall == NULL) {
    printf("new_testing_platforms: Failed to load wall dynamic sprite\n");
    return NULL;
  }

  plat->platform = new_sprite_dynamic("/home/lcom/labs/proj/assets/platform_dynamic.bmp", 8, 0, 0);
  if (plat->wall == NULL) {
    printf("new_testing_platforms: Failed to load platform dynamic sprite\n");
    return NULL;
  }

  plat->size = 8;

  plat->rects = (Rect_t*) malloc(sizeof(Rect_t) * plat->size);
  if (plat->wall == NULL) {
    printf("new_testing_platforms: Failed to allocate memory for the Rect objects\n");
    return NULL;
  }

  plat->is_wall = (bool*) malloc(sizeof(bool) * plat->size);
  if (plat->wall == NULL) {
    printf("new_testing_platforms: Failed to allocate memory for the is_wall bools\n");
    return NULL;
  }

  /* TESTING PLATFORMS & WALLS */

  // Top wall
  plat->rects[0] = rect(0, 0, 1024, 24);
  plat->is_wall[0] = true;
  // Left wall
  plat->rects[1] = rect(0, 24, 24, 768 - 24*2 + 1);
  plat->is_wall[1] = true;
  // Bottom wall
  plat->rects[2] = rect(0, 768 - 24 + 1, 1024, 24);
  plat->is_wall[2] = true;
  // Right wall
  plat->rects[3] = rect(1024 - 24, 24, 24, 768 - 24*2 + 1);
  plat->is_wall[3] = true;

  plat->rects[4] = rect(400, 700, 200, 16);
  plat->is_wall[4] = false;

  plat->rects[5] = rect(200, 600, 200, 16);
  plat->is_wall[5] = false;

  plat->rects[6] = rect(500, 500, 100, 100);
  plat->is_wall[6] = false;

  plat->rects[7] = rect(40, 500, 148, 120);
  plat->is_wall[7] = false;

  return plat; 
}

void free_platforms(Platforms_t *plat) {
  free(plat->rects);
  free(plat->is_wall);
  free_sprite_dynamic(plat->platform);
  free_sprite_dynamic(plat->wall);
  free(plat);
}


// Actual platform stuff

bool does_collide_platforms(Platforms_t* plat, Rect_t* rect) {

  Rect_t* rectangles = plat->rects;
  for (uint16_t i = 0; i < plat->size; ++i) {
    if (rect_collision(rect, rectangles))
      return 1;
    ++rectangles;
  }

  return 0;
}

void render_platforms(Platforms_t *plat) {
  
  Rect_t* rects = (Rect_t*) plat->rects;
  bool* are_walls = (bool*) plat->is_wall;

  for (uint32_t i = 0; i < plat->size; ++i) {
    if (*are_walls)
      draw_sprite_dynamic(plat->wall, rects, WALLS_COLOR);
    else
      draw_sprite_dynamic(plat->platform, rects, PLATFORMS_COLOR);
    
    ++rects;
    ++are_walls;
  }

}
