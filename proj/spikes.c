#include "spikes.h"
#include "sprite.h"

struct Spikes {
    SpriteDynamic_t* sprite;
    uint8_t num_spikes;
    Rect_t* rects;
};

Spikes_t* prototype_spikes() {
    Spikes_t* spikes = (Spikes_t*) malloc(sizeof(Spikes_t));

    // Sprite
    spikes->sprite = new_sprite_dynamic("/home/lcom/labs/proj/assets/spike.bmp", 8, 0, 0);

    spikes->num_spikes = 4;

    // Rects
	spikes->rects = (Rect_t*) malloc(sizeof(Rect_t) * spikes->num_spikes);

    spikes->rects[0] = rect(296, 572, 120, 72);
    spikes->rects[1] = rect(296, 732, 120, 72);
    spikes->rects[2] = rect(404, 368, 72, 120);
    spikes->rects[3] = rect(476, 368, 72, 264);

    return spikes;
}

void render_spikes(Spikes_t* spikes) {
    for (uint32_t i = 0; i < spikes->num_spikes; i++)
        draw_sprite_dynamic(spikes->sprite, &spikes->rects[i], COLOR_NO_MULTIPLY);
}

/**
 * @returns True if the player "hitbox" is colliding with a spike, false otherwise
 */
bool player_touches_spike(Spikes_t* spikes, Rect_t* player) {
    for (uint32_t i = 0; i < spikes->num_spikes; i++)
        if (rect_collision(&spikes->rects[i], player))
            return true;

    return false;
}
