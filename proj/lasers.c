#include "lasers.h"
#include "geometry.h"
#include "sprite.h"

struct Laser {
    uint8_t link_id;
    Rect_t rect;
    Sprite_t *s;
};

struct Lasers{
    Laser_t *Lasers_t;
    uint8_t size;
    uint32_t* colors;
    uint8_t num_colors;
    uint8_t cur_link_id;
};
