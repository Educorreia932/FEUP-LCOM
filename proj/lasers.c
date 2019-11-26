#include "lasers.h"
#include "geometry.h"
#include "sprite.h"

struct Laser {
    uint8_t link_id;
    Rect_t rect;
}; // TODO: Erase

struct Lasers {
    Laser_t* Lasers_t; // TODO: Replace with Rects
    uint8_t size;
    uint32_t* colors;
    uint8_t num_colors;
    uint8_t cur_link_id;
    SpriteDynamic_t *s;
};

Laser_t laser(uint8_t link_id, Rect_t rect) {
    const Laser_t l = {link_id, rect};
    
    return l;
}

Lasers_t* new_testing_lasers() {
    Lasers_t *lasers = (Lasers_t*) malloc(sizeof(Lasers_t));

    lasers->s = new_sprite_dynamic("/home/lcom/labs/proj/assets/laser.bmp", 8, 0, 0);

    lasers->size = 1;

    lasers->Lasers_t = (Laser_t*) malloc(sizeof(Laser_t) * lasers->size);

    lasers->Lasers_t[0] = laser(0, rect(800, 504, 16, 240));

    return lasers;
}

void render_lasers(Lasers_t* lasers) {
    for (uint32_t i = 0; i < lasers->size; i++) 
        draw_sprite_dynamic(lasers->s, &lasers->Lasers_t[0].rect, 0xf800);
}

bool player_is_dead(Lasers_t* lasers, Rect_t* rect) {
    if (rect_collision(rect, &lasers->Lasers_t[0].rect))
        return true;

    return false;
}

