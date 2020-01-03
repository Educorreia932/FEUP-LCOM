#pragma once

#include <lcom/lcf.h>
#include "geometry.h"

/** @file power_ups.h */

/** @addtogroup level @{ */

/**
 * @brief The maximum number of power ups at once
 * @details The number 5 was chosen because there are, at max, 4 powers that can be unlocked and the exit
 */
#define MAX_POWERUPS 5

/**
 * @brief Represents a power up on a Level, such as unlocking the player's ability to change lasers or activate anti-gravity, and also the Level's exit
 * @details Every power up has a void function pointer that is called whenever it is collected
 * @note Could be expanded to create coin like collectibles on a Level
 */
typedef struct PowerUp PowerUp_t;

/**
 * @brief Creates a new Power Up object
 * 
 * @param sprite_file_name The bitmap's path (relative to the assets folder)
 * @param pos The coordinates of the point where the Power Up will be at
 * @param function The function to call when the Power Up is collected
 * @return PowerUp_t* Pointer to the fully built Power Up on success\n
 * NULL otherwise
 */
PowerUp_t* new_power_up(const char* sprite_file_name, Vec2d_t pos, void (*function)());
/**
 * @brief Frees a Power Up from memory
 * 
 * @param pu Pointer to the Power Up to be freed
 */
void free_power_up(PowerUp_t *pu);

/**
 * @brief Checks if a Player's Rect_t would collect the Power Up and collects it
 * @details Must be called every frame
 * @param pu Pointer to the Power UP object to be updated
 * @param player_rect The Player's Rect_t (hitbox)
 */
void update_power_up(PowerUp_t *pu, Rect_t *player_rect);
/**
 * @brief Renders a Power Up onto the screen
 * 
 * @param pu Pointer to the Power Up to be rendered
 */
void render_power_up(PowerUp_t *pu);

/**
 * @brief Collects a given Power Up
 * @details Makes the Power Up inactive and calls its function pointer
 * 
 * @param pu The Power Up to be collected
 */
void collect_powerup(PowerUp_t *pu);
/**
 * @brief Respawns a given Power Up
 * 
 * @param pu Pointer to the Power Up to be collected
 */
void respawn_powerup(PowerUp_t *pu);

/** @} */
