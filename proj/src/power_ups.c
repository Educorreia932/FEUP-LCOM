#include "power_ups.h"
#include "sprite.h"
#include "game_manager.h"

struct PowerUp {
    Sprite_t* sprite;
    Rect_t rect;
    bool active;
    void (*func)();
};

PowerUp_t* new_power_up(const char* sprite_file_name, Rect_t rect, void (*function)()) {
    
    if (sprite_file_name == NULL) {
        printf("new_power_up: sprite_file_name must not be a NULL ptr\n");
        return NULL;
    }

    if (function == NULL) {
        printf("new_power_up: Function pointer must not be NULL ptr\n");
        return NULL;
    }
    
    PowerUp_t* pu = (PowerUp_t*) malloc(sizeof(PowerUp_t));
    if (pu == NULL) {
        printf("new_power_up: Failed to allocate memory for the power up\n");
        return NULL;
    }

    pu->sprite = new_sprite(0, 0, 1, sprite_file_name);
    if (pu->sprite == NULL) {
        printf("new_power_up: Failed to create the Sprite\n");
        free(pu);
        return NULL;
    }

    pu->active = true;
    pu->func = function;

    return pu;
}

void free_power_up(PowerUp_t *pu) {
    if (pu == NULL) {
        printf("free_power_up: Cannot free a NULL pointer\n");
        return;
    }

    free_sprite(pu->sprite);
    free(pu);
}

void update_power_up(PowerUp_t *pu, Rect_t *player_rect) {

    if (rect_collision(&(pu->rect), player_rect)) {
        pu->active = false;
        pu->func();
    }

}

void render_power_up(PowerUp_t *pu) {
    if (pu->active) {
        draw_sprite(pu->sprite, &(pu->rect), COLOR_NO_MULTIPLY, SPRITE_NORMAL);
    }
}
