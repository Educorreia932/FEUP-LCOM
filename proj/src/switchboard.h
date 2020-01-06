#pragma once

#include <lcom/lcf.h>

#include "player.h"

/** @file switchboard.h */

/** @addtogroup ui
  * @{
  */

/**
 * @brief The Switchboard is a special menu that was developed to be used alongside a second instance of this game to play the co-op campaign\n
 * As such, it requires many information about the player, such as the powers he has unlocked (as only the switchboard may activate them)
 * 
 */
typedef struct SwitchBoard SwitchBoard_t;

/**
 * @brief Creates a new Switchboard object
 * 
 * @return SwitchBoard_t* Pointer to a fully built Switchboard on success\n
 * False otherwise
 */
SwitchBoard_t* new_switchboard();
/**
 * @brief Frees a given Switchboard object from memory
 * 
 * @param s_board Pointer to the Switchboard to be freed
 */
void free_switchboard(SwitchBoard_t* s_board);

/**
 * @brief Sets the default powers that the player has upon entering the level
 * 
 * @param sw Pointer to the Switchboard to operate over
 * @param default_powers The default powers the players has
 */
void switchboard_set_default_powers(SwitchBoard_t* sw, PlayerUnlockedPowers default_powers);
/**
 * @brief Gives the switchboar the ability to control extra powers
 * 
 * @param sw Pointer to the Switchboard to operate over
 * @param new_powers Adds all the powers of this enum to the switchboard
 */

void switchboard_unlock_powers(SwitchBoard_t* sw, PlayerUnlockedPowers new_powers);
/**
 * @brief Called when the player respawns, so that all the needed powers are reset
 * 
 * @param sw Pointer to the Switchboard to operate over
 */
void switchboard_player_respawn(SwitchBoard_t* sw);

/**
 * @brief The player won the game, now the winning screen must be displayed
 * 
 * @param sw Pointer to the Switchboard to operate over
 * @param seconds_difference The time in seconds the player took to complete the level
 */
void switchboard_win(SwitchBoard_t* sw, uint16_t seconds_difference);

/**
 * @brief Updates the needed information of the Switchboard
 * @details Must be called every frame
 * @param s_board Pointer to the Switchboard to operate over 
 */
void update_switchboard(SwitchBoard_t* s_board);
/**
 * @brief Renders the Switchboard's UI onto the screen
 * @details Must be called every frame * 
 * @param s_board Pointer to the Switchboard to operate over
 */
void render_switchboard(SwitchBoard_t* s_board);

/**
 * @brief Starts a new minigame on the Switchboard 
 * 
 * @param s_board Pointer to the Switchboard to operate over
 */
void switchboard_start_minigame(SwitchBoard_t* s_board);

/** @} */
