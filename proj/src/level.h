#pragma once

#include <lcom/lcf.h>
#include "sprite.h"
#include "player.h"
#include "platforms.h"
#include "lasers.h"
#include "spikes.h"
#include "power_ups.h"
#include "input_events.h"
#include "ui_elements.h"

/** @file */

/** @defgroup level Level */
/** @addtogroup level
  *
  * @brief Represents an entire level, be it the Campaign or the infinite Arcade mode
  *
  * @{
  */

/**
 * @brief Structure that defines an entire level, be it for Campaign or Arcade mode
 * 
 */
typedef struct Level {
	/**
	 * @brief When true, the level is over (render the victory screen) 
	 */
    bool level_over;
    /**
    * @brief The number of frames since the Level started
    */
    uint32_t frames_since_start;
    /**
     * @brief The Sprite used for this Level's background
     */
    Sprite_t* background;
    /**
     * @brief The sprite used for the victory screen
     */
    Sprite_t* win_screen;
    /**
     * @brief The various platforms and walls that make up the Level
     */
    Platforms_t* platforms;
    /**
     * @brief The player
     */
    Player_t* player;
    /**
     * @brief The various lasers that make up the Level
     */
    Lasers_t *lasers;
    /**
     * @brief The various spikes that make up the Level
     */
    Spikes_t* spikes;
    /**
     * @brief The various power ups that make up a level
     */
    PowerUp_t* pu[MAX_POWERUPS];
    /**
     * @brief The 'ghost' of the second player in versus arcade mode
     */
    PlayerTwo_t* player_two;
    /**
     * @brief The "master" on versus arcade mode
     */
    bool laser_master;

	/**
	 * @brief Multipurpouse scores
	 * @details In single arcade, they show the current score and the high score\n
	 * In arcade versus, they show the scores of both players
	 */
    Score_t *score_1, *score_2;

	/**
	 * @brief The timer to be shown on arcade versus mode
	 */
    Score_t *timer;

	/**
	 * @brief The time (as indicated by the RTC) of when the gamemode started
	 * 
	 */
	uint32_t time_upon_start;

} Level_t;

/**
 * @brief Creats a new level with the arcade mode's layout
 * 
 * @param is_single_player 
 * @return Level_t* Fully built Level object if successful\n
 * NULL otherwise
 */
Level_t* new_arcade_level(bool is_single_player);
/**
 * @brief Creates a new level with the configuration of the prototype level of the Campaign
 * 
 * @param is_single_player 
 * @return Level_t* Fully built Level object if successful\n
 * NULL otherwise
 */
Level_t* prototype_level(bool is_single_player);
/**
 * @brief Frees a level from memory
 * 
 * @param level Pointer to the level to be freed
 */
void free_level(Level_t *level);

/**
 * @brief Updates a regular Level's required parts
 * @details Must be called every frame
 * @param level Pointer to the Level to update
 */
void update_level(Level_t* level) ;

/**
 * @brief Updates an arcade mode (single player) Level's required parts
 * @details Must be called every frame
 * @param level Pointer to the Level to update
 */
void update_arcade_level(Level_t* level);
/**
 * @brief Updates an arcade mode (versus) Level's required parts
 * @details Must be called every frame
 * @param level Pointer to the Level to update
 * @param bytes The bytes sent via serial port to update the ghost player\n
 * Should there be no data to update the player, this value should be NULL
 */
void update_arcade_versus(Level_t* level, uint8_t bytes[]);

/**
 * @brief Resets the needed components of an arcade mode level, in an efficient manner
 * @details Used upon the player's death
 * 
 * @param level The level to be reset
 */
void reset_arcade_mode(Level_t* level);

/**
 * @brief Renders any non arcade versus Level onto the screen
 * @details Must be called every frame
 * 
 * @param level Pointer to the Level to be rendered
 */
void render_level(Level_t *level);

/**
 * @brief Renders an arcade single Level onto the screen
 * @details Must be called every frame
 * @param level Pointer to the Level to be rendered
 */
void render_arcade_single(Level_t* level);
/**
 * @brief Renders an arcade versus Level onto the screen
 * @details Must be called every frame
 * @param level Pointer to the Level to be rendererd
 */
void render_arcade_versus(Level_t* level);

/**
 * @brief Ends the arcade versus gamemode
 * @details To be called when the RTC interrupt is received
 * 
 * @param level Pointer to the level to terminate
 */
void arcade_versus_win(Level_t *level);

/** @} */
