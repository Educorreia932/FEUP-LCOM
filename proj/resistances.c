#include "resistances.h"
#include "sprite.h"

struct Resistances {
    SpriteDynamic_t* sprite_vertical; /** Vertical sprite of a resistance */
    SpriteDynamic_t* sprite_horizontal; /** Horizontal sprite of a resistance */
    uint8_t num_resistances; /**< Number of resistances present on the level */
    Rect_t* rects; /**< "Hitboxes" of the various resistances of a level */
    bool* orientation; /**< Orientation in which the resistance will be rendered. True for vertical, false for horizontal */
};

Resistances_t* new_testing_resistances() {
    Resistances_t* resistances = (Resistances_t*) malloc(sizeof(Resistances_t));

    if (resistances == NULL) {
        printf("new_testing_resistances: Failed to allocate memory for the resistances object\n");
        return NULL;
    }
    resistances->sprite_horizontal = new_sprite_dynamic("home/lcom/labs/proj/assets/resistance_horizontal.bmp", 22, 0, 0);

    if (resistances->sprite_horizontal == NULL) {
        printf("new_testing_resistances: Failed to load horizontal resistance dynamic sprite\n");
        return NULL;
    }

    resistances->sprite_vertical = new_sprite_dynamic("home/lcom/labs/proj/assets/resistance_vertical.bmp", 22, 0, 0);

    if (resistances->sprite_vertical == NULL) {
        printf("new_testing_resistances: Failed to load vertical resistance dynamic sprite\n");
        return NULL;
    }

    resistances->num_resistances = 2;

    resistances->rects = (Rect_t*) malloc(sizeof(Rect_t) * resistances->num_resistances);
    resistances->orientation = (bool*) malloc(sizeof(bool) * resistances->num_resistances);

    resistances->rects[0] = rect(820, 500, 132, 44);
    resistances->orientation[0] = false;

    resistances->rects[1] = rect(400, 200, 44, 132); 
    resistances->orientation[1] = true;

    return resistances;
}

void render_resistances(Resistances_t* resistances) {
    bool* orientation = (bool*) resistances->orientation;

    for (uint32_t i = 0; i < resistances->num_resistances; i++)
        if (orientation[i])
            draw_sprite_dynamic(resistances->sprite_vertical, &resistances->rects[i], COLOR_NO_MULTIPLY);

        else
            draw_sprite_dynamic(resistances->sprite_horizontal, &resistances->rects[i], COLOR_NO_MULTIPLY);
}

/** @returns true if the player "hitbox" is olliding with a resistance, false otherwise */
bool player_touches_resistance(Resistances_t* resistances, Rect_t* player) {
    for (uint32_t i = 0; i < resistances->num_resistances; i++) {
        if (rect_collision(&resistances->rects[i], player))
            return true;
    }

    return false;
}
