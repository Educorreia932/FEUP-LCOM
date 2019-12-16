#include "platforms.h"
#include "geometry.h"
#include "sprite.h"

#define PLATFORMS_COLOR 0xb60e
#define WALLS_COLOR 0x11e2

struct Platforms {
	SpriteDynamic_t* platform;
    SpriteDynamic_t* wall;
    Rect_t* rects;
    bool* is_wall;
    uint8_t size;    
};

// TODO: ALL OF THIS
Platforms_t* new_platforms() {
  	return NULL;
}

Platforms_t* prototype_platforms() {
	Platforms_t *plat = (Platforms_t*) malloc(sizeof(Platforms_t));

	// Sprites
	plat->wall = new_sprite_dynamic("wall_dynamic.bmp" , 8, 0, 0);

	plat->platform = new_sprite_dynamic("platform_dynamic.bmp", 8, 0, 0);

	plat->size = 13;

	// Rects
	plat->rects = (Rect_t*) malloc(sizeof(Rect_t) * plat->size);

	// Walls	
	plat->is_wall = (bool*) malloc(sizeof(bool) * plat->size);
	memset(plat->is_wall, false, plat->size);

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

	plat->rects[4] = rect(176, 560, 120, 72);
	plat->rects[5] = rect(296, 464, 120, 168);
	plat->rects[6] = rect(416, 368, 120, 264);
	plat->rects[7] = rect(504, 24, 152, 200);
	plat->rects[8] = rect(24, 148, 96, 24);
	plat->rects[9] = rect(656, 488, 344, 144);
	plat->rects[10] = rect(772, 24, 96, 124);
	plat->rects[11] = rect(728, 308, 24, 180);
	plat->rects[12] = rect(728, 284, 72, 24);

	return plat;
}

void free_platforms(Platforms_t *plat) {
	if (plat == NULL) {
			printf("free_platforms: Cannot free a NULL pointer\n");
			return;
	}
	
	free(plat->rects);
	free(plat->is_wall);
	free_sprite_dynamic(plat->platform);
	free_sprite_dynamic(plat->wall);
	free(plat);
}

// Actual platform stuff

bool does_collide_platforms(Platforms_t *plat, Rect_t *rect) {
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

