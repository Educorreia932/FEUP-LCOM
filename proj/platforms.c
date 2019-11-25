#include "platforms.h"
#include "geometry.h"
#include "sprite.h"

struct Platforms {
    Rect_t *rects;
    bool *is_wall;
    uint8_t size;
    SpriteDynamic_t *platform;
    SpriteDynamic_t *wall;
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

  plat->size = 7;

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

  plat->rects[0] = rect(0, 0, 1024, 24);
  plat->is_wall[0] = true;
  plat->rects[1] = rect(0, 0, 24, 768);
  plat->is_wall[1] = true;
  plat->rects[2] = rect(0, 768 - 24, 1024, 24);
  plat->is_wall[2] = true;
  plat->rects[3] = rect(1024 - 24, 0, 24, 768);
  plat->is_wall[3] = true;

  plat->rects[4] = rect(400, 700, 200, 16);
  plat->is_wall[4] = false;

  plat->rects[5] = rect(200, 600, 200, 16);
  plat->is_wall[5] = false;

  plat->rects[6] = rect(500, 500, 100, 100);
  plat->is_wall[6] = false;

  return plat; 
}

void free_platforms(Platforms_t *plat) {
  free(plat->rects);
  free(plat->is_wall);
  free_sprite_dynamic(plat->platform);
  free_sprite_dynamic(plat->wall);
  free(plat);
}

void render_platforms(Platforms_t *plat) {
  
  Rect_t* rects = (Rect_t*) plat->rects;
  bool* are_walls = (bool*) plat->is_wall;

  for (uint32_t i = 0; i < plat->size; ++i) {
    if (*are_walls)
      draw_sprite_dynamic(plat->wall, rects, COLOR_NO_MULTIPLY);
    else
      draw_sprite_dynamic(plat->platform, rects, COLOR_NO_MULTIPLY);
    
    ++rects;
    ++are_walls;
  }

}
