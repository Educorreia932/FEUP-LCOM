#include "spikes.h"
#include "sprite.h"

struct Spikes {
    SpriteDynamic_t* sprite;
    uint8_t num_spikes;
    Rect_t* render_rects;
    Rect_t* collision_rects;
};

Spikes_t* prototype_spikes() {
    Spikes_t* spikes = (Spikes_t*) malloc(sizeof(Spikes_t));
    if (spikes == NULL) {
        printf("new_prototype_spikes: Failed to allocate memory for the Spikes object\n");
        return NULL;
    }

    // Sprite
    spikes->sprite = new_sprite_dynamic("/home/lcom/labs/proj/assets/spike.bmp", 8, 0, 0);

    spikes->num_spikes = 4;

    // Rects
	spikes->render_rects = (Rect_t*) malloc(sizeof(Rect_t) * spikes->num_spikes);

    spikes->render_rects[0] = rect(296, 572, 120, 72);
    spikes->render_rects[1] = rect(296, 732, 120, 72);
    spikes->render_rects[2] = rect(404, 368, 72, 120);
    spikes->render_rects[3] = rect(476, 368, 72, 264);

    spikes->collision_rects = (Rect_t*) malloc(sizeof(Rect_t) * spikes->num_spikes);
    if (spikes->collision_rects == NULL) {
        printf("new_prototype_spikes: Failed to allocate memory for collision rects\n");
        return NULL;
    }

    for (uint8_t i = 0; i < spikes->num_spikes; ++i) {
        spikes->collision_rects[i] = rect(
            spikes->render_rects[i].x + 6,
            spikes->render_rects[i].y + 6,
            spikes->render_rects[i].w - 12,
            spikes->render_rects[i].h - 12
        );
    }

    return spikes;
}

void free_spikes(Spikes_t* spikes) {
    if (spikes == NULL) {
        printf("free_spikes: Cannot free a NULL pointer\n");
        return;
    }
    free_sprite_dynamic(spikes->sprite);
    free(spikes->render_rects);
    free(spikes->collision_rects);
    free(spikes);
}

void render_spikes(Spikes_t* spikes) {
    for (uint32_t i = 0; i < spikes->num_spikes; i++)
        draw_sprite_dynamic(spikes->sprite, &spikes->render_rects[i], COLOR_NO_MULTIPLY);
}

/**
 * @returns True if the player "hitbox" is colliding with a spike, false otherwise
 */
bool player_touches_spike(Spikes_t* spikes, Rect_t* player) {
    for (uint32_t i = 0; i < spikes->num_spikes; i++)
        if (rect_collision(&spikes->collision_rects[i], player))
            return true;

    return false;
}
