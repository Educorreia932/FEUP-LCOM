#pragma once

#include <lcom/lcf.h>
#include "input_events.h"
#include "platforms.h"
#include "lasers.h"
#include "spikes.h"
#include "power_ups.h"

/** @file player.h */

/** @defgroup player
 *  @brief Represents the character Watt, be it the Campaign or Arcade mode, singleplayer or multiplayer
 *  @{ 
 */

/**
 * @brief Object containing all the information about the player
 */
typedef struct Player Player_t;

/**
 * @brief Powers that the player unlocked (and can control)
 */
typedef enum PlayerUnlockedPowers {
    UNLOCKED_NOTHING = 0,
    UNLOCKED_SPEED = 1,
    UNLOCKED_JUMP = 2,
    UNLOCKED_LASERS = 4,
    UNLOCKED_GRAVITY = 8,
    UNLOCKED_ALL = 15
} PlayerUnlockedPowers;

/**
 * @brief Gets the player's default powers
 * @return The unlocked powers by default (usually, jump and speed)
 */
PlayerUnlockedPowers player_get_default_powers();

/**
 * @brief Unlocks the laser control
 */
void player_unlock_lasers();

/**
 * @brief Unlocks the gravity control
 */
void player_unlock_gravity();

/**
 * @brief Handles the "win" of the player (when he reaches the exit)
 * 
 * @details Sets the level to render the winning screen that has the time the player took to complete the game
 */
void player_win();

/** 
 * @brief Changes the speed of the player
 * 
 * @param speed Speed multiplier to be set 
 */
void player_set_speed(uint8_t speed);

/**
 * @brief Changes the jump height of the player
 * 
 * @param jump Jump multiplier to be set 
 */
void player_set_jump(uint8_t jump);

/**
 * @brief Sets the gravity to be "normal" (the player is pulled downwards)
 */
void player_set_gravity_normal();

/**
 * @brief Sets the gravity to be "reversed" (the player is pulled upwards)
 */
void player_set_gravity_reversed();

/**
 * @brief Creates a Player object 
 * 
 * @param ui_controls Sets wheter the UI Controls should be rendered or not (they should be only rendered in Campaign - Singleplayer mode)
 * @param arcade_mode Sets wheter the player is playing in Campaign mode or Arcade
 * @param default_powers The powers the users starts with by default (but it's always jump and speed at the moment)
 * @return Player_t* Pointer to the fully buily Player object on sucess\n 
 * NULL otherwise
 */
Player_t* new_player(bool ui_controls, bool arcade_mode, PlayerUnlockedPowers default_powers);

/**
 * @brief Sets the main color of the Player
 * 
 * @param player Player to change the color of
 * @param main_color The color to change to
 */
void player_set_main_color(Player_t* player, uint16_t main_color);
/**
 * @brief Sets the death color of the Player
 * 
 * @param player Player to change the color of
 * @param main_color The color to change to
 */
void player_set_death_color(Player_t* player, uint16_t death_color);

/**
 * @brief Updates a player state (position on screen, animation, whether is dead or not, unlocked powerups...)
 * 
 * @param player Pointer to the Player that is being updated
 * @param plat Pointer to the Platforms of the level (to check collisions with the player)
 * @param lasers Pointer to the Lasers of the level (to check for any possible death)
 * @param spikes Pointer to the Spikes of the level (to check for any possible death)
 * @param pu Pointer to the Powerups of the level (to check if the Player obtained any one of them)
 */
void update_player(Player_t* player, Platforms_t* plat, Lasers_t* lasers, Spikes_t* spikes, PowerUp_t* pu[]);

/**
 * @brief Renders the background of the Player (that consists of sparks that appear whenever its anti-gravity is on)
 * 
 * @param player Pointer to the Player to be rendered
 */
void render_player_background(Player_t* player);

/**
 * @brief Renders the foreground of the Player (that consists of all of tis body)
 * 
 * @param player Pointer to the Player to be rendered
 */
void render_player_foreground(Player_t* player);

/**
 * @brief Renders the UI of the Player (the buttons and sliders necessary in Campaign - Singleplayer mode, its score in Arcade mode and the winning screen in Campaign mode)
 * 
 * @param player Pointer to the Player to be rendered
 */
void render_player_ui(Player_t *player);

/**
 * @brief Frees a given Player object from memory
 * 
 * @param player Player object to be freed
 */
void free_player(Player_t* player);

typedef struct PlayerTwo PlayerTwo_t;

/**
 * @brief Creates a PlayerTwo object 
 * 
 * @return PlayerTwo_t* Pointer to the fully buily PlayerTwo object on sucess\n 
 * NULL otherwise
 */
PlayerTwo_t* new_player_two();
/**
 * @brief Frees a PlayerTwo from memory
 * 
 * @param player_two PlayerTwo object to be freed
 */
void free_player_two(PlayerTwo_t* player_two);

/**
 * @brief Sets the main color of the PlayerTwo
 * 
 * @param player_two PlayerTwo to change the color of
 * @param main_color The color to change to
 */
void player_two_set_main_color(PlayerTwo_t* player_two, uint16_t main_color);
/**
 * @brief Sets the death color of the PlayerTwo
 * 
 * @param player_two PlayerTwo to change the color of
 * @param main_color The color to change to
 */
void player_two_set_death_color(PlayerTwo_t* player_two, uint16_t death_color);

/**
 * @brief Renders the background of the second Player (that consists of sparks that appear whenever its anti-gravity is on)
 * 
 * @param player_two Pointer to second the Player to be rendered
 */
void render_player_two_background(PlayerTwo_t* player_two);

/**
 * @brief Renders the foreground of the second Player (that consists of all of tis body)
 * 
 * @param player_two Pointer to the second Player to be rendered
 */
void render_player_two_foreground(PlayerTwo_t* player_two);

/**
 * @brief Updates player two with the information received via serial port
 * 
 * @param player_two The Player Two to update
 * @param bytes Information received via serial port\n
 * If not received in time for this frame, value must be NULL
 */
void update_player_two(PlayerTwo_t* player_two, uint8_t bytes[]);


/** @} */
