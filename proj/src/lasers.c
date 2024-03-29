#include "lasers.h"
#include "geometry.h"
#include "sprite.h"
#include "hw_manager.h"
#include "game_manager.h"

#define ARCADE_LASER_HOLE_HEIGHT 160
#define ARCADE_LASER_MIN_HEIGHT 60
#define ARCADE_LASER_HOLE_HEIGHT_RANGE 500
#define ARCADE_LASER_WIDTH 20

#define ARCADE_LASER_LEFT_EDGE (0 + 24)
#define ARCADE_LASER_RIGHT_EDGE (1024 - 24)
#define ARCADE_LASER_TOP_EDGE (0 + 24)
#define ARCADE_LASER_BOTTOM_EDGE (768 - 24)

typedef struct Laser {
    Rect_t rect;
    uint8_t link_id;
} Laser_t;

Laser_t* new_laser(Rect_t rect, uint8_t link_id) {
    Laser_t* laser = (Laser_t*) malloc(sizeof(Laser_t));
    if (laser == NULL) {
        printf("Failed to create laser object\n");
        return NULL;
    }

    laser->rect = rect;
    laser->link_id = link_id;

    return laser;
}

void free_laser(Laser_t* laser) {
    if (laser == NULL) {
        printf("free_laser: Unable to free a NULL pointer\n");
        return;
    }

    free(laser);
}

struct Lasers {
    Laser_t** lasers;
    uint8_t num_lasers;
    uint8_t cur_link_id;
    /** @note The number of colors is also the number of possible link ids */
    uint16_t* colors;
    uint8_t num_colors;
    SpriteDynamic_t *sprite;

    // Arcade exclusive
    uint16_t next_laser; // Used for arcade mode
    uint8_t lasers_speed, lasers_delay;
};

Lasers_t* new_arcade_lasers() {
    Lasers_t* lasers = (Lasers_t*) malloc(sizeof(Lasers_t));
    if (lasers == NULL) {
        printf("new_arcade_lasers: Failed to allocate memory for the lasers\n");
        return NULL;
    }

    // Sprite
    lasers->sprite = new_sprite_dynamic("laser.bmp", 8, 0, 0);
    if (lasers->sprite == NULL) {
        printf("new_arcade_lasers: Failed to create the lasers' Sprite\n");
        free(lasers);
        return NULL;
    }

    lasers->num_lasers = 16;
    lasers->cur_link_id = 1;
    lasers->next_laser = 0;
    lasers->lasers_speed = 4;
    lasers->lasers_delay = 120;

    lasers->lasers = (Laser_t**) calloc(lasers->num_lasers, sizeof(Laser_t*));
    if (lasers->lasers == NULL) {
        printf("new_arcade_lasers: Failed to allocate memory for the individual Lasers' pointers\n");
        free_sprite_dynamic(lasers->sprite);
        free(lasers);
        return NULL;
    }

    // Colors
    lasers->num_colors = 1;

    lasers->colors = (uint16_t*) calloc(lasers->num_colors, sizeof(uint16_t));
    if (lasers->colors == NULL) {
        printf("new_arcade_lasers: Failed to allocate memory for the individual Lasers\n");
        free_sprite_dynamic(lasers->sprite);
        free(lasers->lasers);
        free(lasers);
        return NULL;
    }

    lasers->colors[0] = COLOR_RED;

    return lasers;
}

Lasers_t* prototype_lasers() {
    Lasers_t* lasers = (Lasers_t*) malloc(sizeof(Lasers_t));
    if (lasers == NULL) {
        printf("prototype_lasers: Failed to allocate memory for the lasers\n");
        return NULL;
    }

    // Sprite
    lasers->sprite = new_sprite_dynamic("laser.bmp", 8, 0, 0);
    if (lasers->sprite == NULL) {
        printf("prototype_lasers: Failed to create the lasers' Sprite\n");
        free(lasers);
        return NULL;
    }

    lasers->num_lasers = 4;
    lasers->cur_link_id = 0;
    lasers->next_laser = 0;
    lasers->lasers_speed = 4;
    lasers->lasers_delay = 120;

    lasers->lasers = (Laser_t**) calloc(lasers->num_lasers, sizeof(Laser_t*));
    if (lasers->lasers == NULL) {
        printf("prototype_lasers: Failed to allocate memory for the individual Lasers' pointers\n");
        free_sprite_dynamic(lasers->sprite);
        free(lasers);
        return NULL;
    }

    lasers->lasers[0] = new_laser(rect(680, 632, 16, 112), 0);
    lasers->lasers[1] = new_laser(rect(780, 632, 16, 112), 1);
    lasers->lasers[2] = new_laser(rect(880, 632, 16, 112), 0);
    lasers->lasers[3] = new_laser(rect(512, 224, 16, 144), 2);

    // Colors
    lasers->num_colors = 3;

    lasers->colors = (uint16_t*) calloc(lasers->num_colors, sizeof(uint16_t));
    if (lasers->colors == NULL) {
        printf("prototype_lasers: Failed to allocate memory for the individual Lasers\n");
        free_sprite_dynamic(lasers->sprite);
        free(lasers->lasers);
        free(lasers);
        return NULL;
    }

    lasers->colors[0] = COLOR_RED;
    lasers->colors[1] = COLOR_BLUE;
    lasers->colors[2] = COLOR_PINK;

    return lasers;
}

void free_lasers(Lasers_t* lasers) {
    if (lasers == NULL) {
        printf("free_lasers: Cannot free a NULL pointer\n");
        return;
    }
    free_sprite_dynamic(lasers->sprite);
    
    for (uint8_t i = 0; i < lasers->num_lasers; ++i) {
        if (lasers->lasers[i] != NULL)
            free_laser(lasers->lasers[i]);
    }

    free(lasers->lasers);
    free(lasers->colors);
    free(lasers);
}

inline bool lasers_set_link_id(Lasers_t *lasers, uint8_t link) {
    if (link >= lasers->num_colors)
        return false;
    lasers->cur_link_id = link;
    return true;
}

// inline void lasers_cycle_link_id(Lasers_t *lasers) {
//     lasers->cur_link_id = (lasers->cur_link_id + 1) % lasers->num_colors;
// }

void render_lasers(Lasers_t* lasers) {
    Laser_t** aux = lasers->lasers;

    for (uint32_t i = 0; i < lasers->num_lasers; i++) {
        if (*aux != NULL && lasers->cur_link_id != (*aux)->link_id)
            draw_sprite_dynamic(lasers->sprite, &(*aux)->rect, lasers->colors[(*aux)->link_id]);
        
        ++aux;
    }

}

bool lasers_collide_player(Lasers_t* lasers, Rect_t* rect) {
    Laser_t** aux = lasers->lasers;

    for (uint32_t i = 0; i < lasers->num_lasers; i++) {
        if (*aux != NULL && lasers->cur_link_id != (*aux)->link_id)
            if (rect_collision(&(*aux)->rect, rect))
                return true;
        ++aux;
    }

    return false;
}

void arcade_update_laser_values(Lasers_t *lasers, uint32_t frames_since_start) {
    
    ++frames_since_start;

    if (frames_since_start <= 2 * 60) {
        lasers->lasers_speed = 4;
        lasers->lasers_delay = 120;
    }
    else if (frames_since_start <= 4 * 60) {
        lasers->lasers_speed = 4;
        lasers->lasers_delay = 110;
    }
    else if (frames_since_start <= 7 * 60) {
        lasers->lasers_speed = 4;
        lasers->lasers_delay = 100;
    }
    else if (frames_since_start <= 11 * 60) {
        lasers->lasers_speed = 4;
        lasers->lasers_delay = 90;
    }
    else if (frames_since_start <= 18 * 60) {
        lasers->lasers_speed = 5;
        lasers->lasers_delay = 90;
    }
    else if (frames_since_start <= 22 * 60) {
        lasers->lasers_speed = 5;
        lasers->lasers_delay = 80;
    }
    else if (frames_since_start <= 26 * 60) {
        lasers->lasers_speed = 5;
        lasers->lasers_delay = 70;
    }
    else if (frames_since_start <= 30 * 60) {
        lasers->lasers_speed = 5;
        lasers->lasers_delay = 60;
    }
    else {
        lasers->lasers_speed = 6;
        lasers->lasers_delay = 60;
    }

}

void arcade_versus_update_laser_values(Lasers_t *lasers, uint32_t frames_since_start) {
    
    ++frames_since_start;

    if (frames_since_start <= 10 * 60) {
        hw_manager_uart_send_char(HEADER_ARCADE_LASER_SPEED);
        hw_manager_uart_send_char(4);
        hw_manager_uart_send_char(HEADER_TERMINATOR);
        lasers->lasers_speed = 4;
        lasers->lasers_delay = 120;
    }
    else if (frames_since_start <= 20 * 60) {
        lasers->lasers_speed = 4;
        lasers->lasers_delay = 110;
    }
    else if (frames_since_start <= 25 * 60) {
        lasers->lasers_speed = 4;
        lasers->lasers_delay = 100;
    }
    else if (frames_since_start <= 30 * 60) {
        lasers->lasers_speed = 4;
        lasers->lasers_delay = 90;
    }
    else if (frames_since_start <= 35 * 60) {
        hw_manager_uart_send_char(HEADER_ARCADE_LASER_SPEED);
        hw_manager_uart_send_char(5);
        hw_manager_uart_send_char(HEADER_TERMINATOR);
        lasers->lasers_speed = 5;
        lasers->lasers_delay = 90;
    }
    else {
        lasers->lasers_speed = 5;
        lasers->lasers_delay = 80;
    }

}

void arcade_move_lasers(Lasers_t *lasers) {
    Laser_t** aux = lasers->lasers;

    for (uint32_t i = 0; i < lasers->num_lasers; i++) {
        if (*aux != NULL) {
            (*aux)->rect.x -= lasers->lasers_speed;

            if ((*aux)->rect.x < ARCADE_LASER_LEFT_EDGE) {
                free_laser(*aux);
                *aux = NULL;
            }
        }
            
        ++aux;
    }

}

uint16_t arcade_generate_laser_height() {
    return (uint16_t) ((rand() % ARCADE_LASER_HOLE_HEIGHT_RANGE) + ARCADE_LASER_MIN_HEIGHT);
}

bool arcade_spawn_next_laser(Lasers_t *lasers) {
    if (lasers->next_laser) {
        --lasers->next_laser;
        return false;
    }
    else
        return true;
}

void arcade_lasers_set_correct_delay(Lasers_t *lasers) {
    lasers->next_laser = lasers->lasers_delay;
}

void arcade_add_laser(Lasers_t *lasers, uint16_t height) {

    Laser_t **aux = lasers->lasers;
    Laser_t **top = NULL, **bottom = NULL;

    // Is there a suitable free pair of lasers? 
    for (uint32_t i = 0; i < lasers->num_lasers; i++) {
        if (*aux == NULL) {
            if (top == NULL) 
                top = aux;

            else {
                bottom = aux;
                break;
            }
        }
            
        ++aux;
    }

    if (top != NULL && bottom != NULL) {

        *top = new_laser(
            rect_from_uints(
                ARCADE_LASER_RIGHT_EDGE - ARCADE_LASER_WIDTH,
                ARCADE_LASER_TOP_EDGE,
                ARCADE_LASER_WIDTH,
                height - ARCADE_LASER_TOP_EDGE
            ),
            0
        );

        *bottom = new_laser(
            rect_from_uints(
                ARCADE_LASER_RIGHT_EDGE - ARCADE_LASER_WIDTH,
                height + ARCADE_LASER_HOLE_HEIGHT,
                ARCADE_LASER_WIDTH,
                ARCADE_LASER_BOTTOM_EDGE - height
                    - ARCADE_LASER_HOLE_HEIGHT
            ),
            0
        );

    }
}

static bool in_empty_space = false;

bool arcade_player_passes_lasers(Lasers_t* lasers, Rect_t* rect) {
    Laser_t** aux = lasers->lasers;

    for (uint32_t i = 0; i < lasers->num_lasers; i += 2) {
        if (aux[i] != NULL) {
            float x = aux[i]->rect.x;
            float y = aux[i]->rect.y;
            float w = aux[i]->rect.w;
            float h = aux[i + 1]->rect.y - y;

            Rect_t* empty_space = new_rect(x, y, w , h);

            if (rect_collision(empty_space, rect)) {
                free(empty_space);

                if (in_empty_space)
                    return false;

                in_empty_space = true;

                return true;
            } 
        }
    }

    in_empty_space = false;

    return false;
}

void arcade_reset_lasers(Lasers_t *lasers) {
    lasers->next_laser = 0;

    lasers->lasers_delay = 120;
    lasers->lasers_speed = 4;

    Laser_t **aux = lasers->lasers;

    // Free all lasers 
    for (uint32_t i = 0; i < lasers->num_lasers; i++) {
        if (*aux != NULL) {
            free_laser(*aux);
            *aux = NULL;
        }
        ++aux;
    }

}

void arcade_lasers_set_speed(Lasers_t *lasers, uint8_t new_speed) {
    lasers->lasers_speed = new_speed;
}
