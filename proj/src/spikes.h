#pragma once

#include <lcom/lcf.h>

#include "geometry.h"

/** @file */

/** @addtogroup level @{ */

/**
 * @brief Structure that represents all the Spikes in a Level 
 */
typedef struct Spikes Spikes_t;

/**
 * @brief Creates the Spikes for the arcade mode
 * 
 * @return Spikes_t* Pointer to the fully built Spikes object on success\n
 * NULL otherwise
 */
Spikes_t* new_arcade_spikes();
/**
 * @brief Creates the Spikes for the prototype Campaign mode
 * 
 * @return Spikes_t* Pointer to the fully built Spikes object on success\n
 * NULL otherwise
 */
Spikes_t* prototype_spikes();
/**
 * @brief Frees the Spikes object from memory
 * 
 * @param spikes Pointer to the Spikes to be freed
 */
void free_spikes(Spikes_t* spikes);

/**
 * @brief Renders the Spikes onto the screen
 * @details Must be called every frame 
 * @param spikes Pointer to the Spikes to be rendered
 */
void render_spikes(Spikes_t* spikes);

/**
 * @brief Checks if the Player's position is colliding with any of the Spikes
 * 
 * @param spikes Pointer to the Spikes to check for collisions with
 * @param player Pointer to the Player's rect (hitbox)
 * @return True if the player "hitbox" is colliding with a spike\n
 * False otherwise
 */
bool player_touches_spike(Spikes_t* spikes, Rect_t* player);

/** @} */
