#pragma once

#include <lcom/lcf.h>
#include "geometry.h"

/** @file lasers.h */

/** @addtogroup level @{ */

/**
 * @brief Structure that represents all the Lasers in a Level
 * @details Every Lasers object has a current link ID, when an individual Laser's link ID is the same as the Lasers' one, it will not be rendered and it won't be considered harmful to the player
 * 
 */
typedef struct Lasers Lasers_t;

/**
 * @brief Creates a Lasers object with the layout for the prototype campaign Level 
 * 
 * @return Lasers_t* Pointer to the fully built Lasers object on success\n
 * NULL otherwise
 */
Lasers_t* prototype_lasers();
/**
 * @brief Creates a Lasers object with the layout for arcade mode
 * 
 * @return Lasers_t* Pointer to the fully built Lasers object on success\n
 * NULL otherwise
 */
Lasers_t* new_arcade_lasers();
/**
 * @brief Frees a Lasers object from memory
 * 
 * @param lasers Pointer to the Lasers object to be freed
 */
void free_lasers(Lasers_t* lasers);

/**
 * @brief Sets the link of a Lasers object to 'link'
 * @details A successful change requires an ID inside the range established in the Lasers object
 * @param lasers Pointer to the Lasers object to update
 * @param link The new link ID
 * @return True when the operation was successful\n
 * False otherwise 
 */
bool lasers_set_link_id(Lasers_t *lasers, uint8_t link);

/**
 * @brief Renders a Lasers object's lasers onto the screen
 * @details Must be called every frame
 * @param lasers The Lasers object to be rendered
 */
void render_lasers(Lasers_t* lasers);

/**
 * @brief Checks if a player collides with any of the Lasers' Lasers
 * 
 * @param lasers Pointer to the Lasers object to check
 * @param rect Pointer to the player's Rect_t (hitbox)
 * @return True when the player is colliding with an active Laser\n
 * False otherwise
 */
bool lasers_collide_player(Lasers_t* lasers, Rect_t* rect);

/**
 * @brief Updates the lasers spawn rate and speed to ramp up the difficulty the longer you play
 * @details Tailored for single player mode
 * 
 * @param lasers Pointer to the Lasers object to update 
 * @param frames_since_start The number of frames since the start of the level
 */
void arcade_update_laser_values(Lasers_t *lasers, uint32_t frames_since_start);
/**
 * @brief Updates the lasers spawn rate and speed to ramp up the difficulty the longer you play
 * @details Tailored for Versus mode
 * 
 * @param lasers Pointer to the Lasers object to update 
 * @param frames_since_start The number of frames since the start of the level
 */
void arcade_versus_update_laser_values(Lasers_t *lasers, uint32_t frames_since_start);

/**
 * @brief Moves all the lasers on screen onto the left, eliminating the ones that go offscreen\n 
 * Used exclusively on the arcade mode
 * @details Must be called every frame
 * 
 * @param lasers Pointer to the Lasers object to udpate
 */
void arcade_move_lasers(Lasers_t *lasers);

/**
 * @brief Generates the height for the next arcade Laser
 * 
 * @return uint16_t The height for the next new arcade Laser
 */
uint16_t arcade_generate_laser_height();
/**
 * @brief Handles the timing until the next Laser spawns
 * @details Must be called every frame to work as intended
 * @param lasers Pointer to the Lasers object to operate over
 * @return True when a laser should spawn\n
 * False otherwise
 */
bool arcade_spawn_next_laser(Lasers_t *lasers);
/**
 * @brief Sets the delay for the next Laser's spawn
 * 
 * @param lasers Pointer to the Lasers object to operate over
 */
void arcade_lasers_set_correct_delay(Lasers_t *lasers);
/**
 * @brief Creates and adds a new Laser to the Lasers object, with a random height (flappy bird like) onto the right edge of the screen\n 
 * Used exclusively on the arcade mode
 * 
 * @param lasers Pointer to the Lasers object to udpate
 * @param height The height for the new Laser
 */
void arcade_add_laser(Lasers_t *lasers, uint16_t height);

/**
 * @brief Keeps track of when the player passes through a new set of Lasers, to increase it's score\n 
 * Used exclusively on the arcade mode
 * 
 * @param lasers Pointer to the Lasers object to check
 * @param rect Player's Rect_t (hitbox)
 * @return True if the player just passed a Laser\n
 * False otherwise
 */
bool arcade_player_passes_lasers(Lasers_t* lasers, Rect_t* rect);

/**
 * @brief Resets all the lasers (arcade mode only)
 * @details Mainly used to reset the level upon the player's death
 * 
 * @param lasers Pointer to the Lasers to reset
 */
void arcade_reset_lasers(Lasers_t *lasers);

/**
 * @brief Manually sets the speed of the Lasers
 * 
 * @param lasers Pointer to the Lasers to change the speed of
 * @param new_speed The new speed for the Lasers
 */
void arcade_lasers_set_speed(Lasers_t *lasers, uint8_t new_speed);

/** @} */
