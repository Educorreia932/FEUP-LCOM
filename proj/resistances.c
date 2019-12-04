#include "resistances.h"
#include "sprite.h"

enum Orientation {
    horizontal,
    vertical
};

struct Resistances {
    SpriteDynamic_t* sprite_vertical; /**< @brief Vertical sprite of a resistance */
    SpriteDynamic_t* sprite_horizontal; /**< @brief Horizontal sprite of a resistance */
    uint8_t num_resistances; /**< @brief Number of resistances present on the level */
    Rect_t* rects; /**< @brief "Hitboxes" of the various resistances of a level */
    bool* orientation; /**< @brief Orientation in which the resistance will be rendered. True for vertical, false for horizontal */
};

Resistances_t* prototype_resistances() {
    Resistances_t* resistances = (Resistances_t*) malloc(sizeof(Resistances_t));

    // Spikes
    resistances->sprite_horizontal = new_sprite_dynamic("home/lcom/labs/proj/assets/resistance_horizontal.bmp", 22, 0, 0);

    resistances->sprite_vertical = new_sprite_dynamic("home/lcom/labs/proj/assets/resistance_vertical.bmp", 22, 0, 0);

    resistances->num_resistances = 0;

    return resistances;
}

void free_resistances(Resistances_t* resistances) {
    free_sprite_dynamic(resistances->sprite_horizontal);
    free_sprite_dynamic(resistances->sprite_vertical);
    free(resistances->rects);
    free(resistances->orientation);
    free(resistances);
}

void render_resistances(Resistances_t* resistances) {
    bool* orientation = (bool*) resistances->orientation;

    for (uint32_t i = 0; i < resistances->num_resistances; i++)
        if (orientation[i])
            draw_sprite_dynamic(resistances->sprite_vertical, &resistances->rects[i], COLOR_NO_MULTIPLY);

        else
            draw_sprite_dynamic(resistances->sprite_horizontal, &resistances->rects[i], COLOR_NO_MULTIPLY);
}

/**
 * @returns true if the player "hitbox" is olliding with a resistance, false otherwise
 */
bool player_touches_resistance(Resistances_t* resistances, Rect_t* player) {
    for (uint32_t i = 0; i < resistances->num_resistances; i++) {
        if (rect_collision(&resistances->rects[i], player))
            return true;
    }

    return false;
}
