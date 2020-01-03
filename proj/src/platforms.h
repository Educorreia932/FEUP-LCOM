#pragma once

#include <lcom/lcf.h>
#include "geometry.h"

/** @file platforms.h */

/** @addtogroup level @{ */

/**
 * @brief Object containing all the platforms and walls of a Level 
 */
typedef struct Platforms Platforms_t;

/**
 * @brief Creates a Platforms object with the layout needed for the arcade mode
 * 
 * @return Platforms_t* Pointer to the fully built Platforms object on success\n
 * NULL otherwise
 */
Platforms_t* new_arcade_platforms();
/**
 * @brief Creates a Platforms object with the layout of the prototype campaign Level
 * 
 * @return Platforms_t* Platforms_t* Pointer to the fully built Platforms object on success\n
 * NULL otherwise
 */
Platforms_t* prototype_platforms();
/**
 * @brief Frees a Platforms object from memory
 * 
 * @param platforms Pointer to the Platforms object to be freed 
 */
void free_platforms(Platforms_t *platforms);

/**
 * @brief Renders a Platforms' platforms and walls onto the screen
 * @details Must be called every frame 
 * @param plat 
 */
void render_platforms(Platforms_t *plat);

/**
 * @brief Checks if a player's rect (hitbox) collides with any platform or wall
 * 
 * @param plat Pointer to the Platforms object
 * @param rect The player's rect (hitbox)
 * @return True if the player collides with any of the platforms or walls\n
 * False otherwise
 */
bool does_collide_platforms(Platforms_t* plat, Rect_t* rect);

/** @} */
