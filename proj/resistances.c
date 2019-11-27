#include "resistances.h"
#include "sprite.h"

struct Resistances {
    SpriteDynamic_t* sprite;
    uint8_t num_resistances;
    Rect_t* rects;
};

Resistances_t* new_testing_resistances() {
    Resistances_t* resistances = (Resistances_t*) malloc(sizeof(Resistances_t));

    if (resistances == NULL) {
        printf("new_testing_resistances: Failed to allocate memory for the resistances object\n");

        return NULL;
    }

    resistances->sprite = new_sprite_dynamic("home/lcom/labs/proj/assets/resistance.bmp", 22, 0, 0);

    resistances->num_resistances = 1;

    resistances->rects = (Rect_t*) malloc(sizeof(Rect_t) * resistances->num_resistances);

    resistances->rects[0] = rect(830, 500, 132, 44);    

    return resistances;
}

void render_resistances(Resistances_t* resistances) {
    for (uint32_t i = 0; i < resistances->num_resistances; i++)
        draw_sprite_dynamic(resistances->sprite, &resistances->rects[i], COLOR_NO_MULTIPLY);
}

bool player_touches_resistance(Resistances_t* resistances, Rect_t* player) {
    for (uint32_t i = 0; i < resistances->num_resistances; i++) {
        if (rect_collision(&resistances->rects[i], player))
            return true;
    }

    return false;
}
