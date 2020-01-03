#pragma once

#include <lcom/lcf.h>

#include "level.h"
#include "switchboard.h"
#include "main_menu.h"

/** @file game_manager.h */

/** @defgroup gm Game Manager */

/** @addtogroup gm
  *
  * @brief	The essential Game Manager singleton
  *
  * @{
  */

/**
 * @brief The absolute path to the assets, can be changed as a command line argument
 */
extern char* assets_rel_path;

#define UART_GRAVITY_NORMAL 0 /** <@brief Content of the gravity byte */
#define UART_GRAVITY_REVERSED 1 /** <@brief Content of the gravity byte */

// number of bytes of each kind of message
#define HEADER_SYNCED_SIZE 2 /** <@brief Total size of the synced header */
#define HEADER_AVAILABLE_LEVEL_SIZE 2 /** <@brief Total size of the available level header */
#define HEADER_AVAILABLE_ARCADE_SIZE 2 /** <@brief Total size of the available arcade header */

// Campaign
#define HEADER_SPEED_MULT_SIZE 3 /** <@brief Total size of the speed mult header */
#define HEADER_JUMP_MULT_SIZE 3 /** <@brief Total size of the jump mult header */
#define HEADER_LASER_SIZE 3 /** <@brief Total size of the laser header */
#define HEADER_GRAVITY_SIZE 3 /** <@brief Total size of the gravity header */
#define HEADER_PLAYER_RESPAWN_SIZE 2  /** <@brief Total size of the player respawn header */
#define HEADER_PLAYER_UPDATE_SIZE 3 /** <@brief Total size of the player update header */
#define HEADER_REQUEST_POWERS_SIZE 2 /** <@brief Total size of the request powers header */
#define HEADER_RESPONSE_POWERS_SIZE 3 /** <@brief Total size of the response powers header */

// Arcade
#define HEADER_PLAYER_TWO_UPDATE_SIZE 8 /** <@brief Total size of the player two update header */
#define HEADER_ARCADE_READY_SIZE 2 /** <@brief Total size of the arcade ready header */

/**
 * @brief Enum with all the serial port headers
 * 
 */
enum UART_EVENT_HEADER {
    HEADER_TERMINATOR = 0xFF,

    // Sync
    HEADER_SYNCED = 's',
    HEADER_AVAILABLE_LEVEL = 'l',
    HEADER_AVAILABLE_ARCADE = 'a',

    // Campaign
    HEADER_SPEED_MULT = 'S',
    HEADER_JUMP_MULT = 'J',
    HEADER_LASER = 'L',
    HEADER_GRAVITY = 'G',
    HEADER_PLAYER_RESPAWN = 'R',
    HEADER_PLAYER_UPDATE = 'U',
    HEADER_REQUEST_POWERS = 'P',
    HEADER_RESPONSE_POWERS = 'p',

    // Arcade
    HEADER_PLAYER_TWO_UPDATE = 'T',
    HEADER_ARCADE_READY = 'A'
};

/**
 * @brief An enum representing the current gamemode
 * @details Used to select which update and render function pointer is the correct one\n
 * If at any time we need to check if we are in a multiplayer 'state', we need only to check the BIT(0) 
 */
typedef enum GameMode {
    GM_UART = 1,
    GM_LEVEL = 2,
    GM_SWITCHBOARD = 4,
    GM_ARCADE = 8,
    // Specific combinations
    GM_LEVEL_UART = 3,
    GM_ARCADE_UART = 9,
    GM_SWITCHBOARD_UART = 5, // This one is dumb and works as a backup
    GM_MAIN_MENU = 16
} GameModeEnum;

/**
 * @brief The class responsible for handling everything. Instanced as a Singleton to provide acess from anywhere on the files
 * @details This is responsible for switching between menus and gamemodes and some of the most important variables to guarantee that two PCs are still connected via serial port\n
 * Only one of the level, s_board or main_menu is actually instantiated at a time, meaning that when they are not in use, they are NULL pointers.
 * 
 */
typedef struct GameManager {
	Level_t *level; /** <@brief Pointer to the level */
	SwitchBoard_t *s_board; /** <@brief Pointer to the switchboard */
    MainMenu_t *main_menu; /** <@brief Pointer to the main menu */
    void (*update_function[32])(); /** <@brief Function pointer array for update functions */
    void (*render_function[32])(); /** <@brief Function pointer array for render functions */
    uint32_t esc_counter; /** <@brief Used to know when the user has pressed ESC twice in quick succesion */
    bool game_ongoing; /** <@brief Used to terminate the game */
    bool uart_synced; /** <@brief Used to know if two UARTS are synced */
    bool has_partner; /** <@brief Used to know if the UART has a confirmed partner */
    uint32_t uart_last_sent /** <@brief Number of frames since last sent message */, uart_last_received /** <@brief Number of frames since last received message */;
	GameModeEnum gamemode; /** <@brief The current gamemode */
    Sprite_t *connecting_sprite; /** <@brief The Sprite shown when waiting for a partner*/

    bool normal_rendering; /** <@brief Used for the glitched screen effect on the switchboard */
    void (*switch_buffer_func[2])(); /** <@brief Index 0 has the 'glitched' function, index 1 the normla function */

} GameManager_t;

/**
 * @brief Get the game manager object
 * 
 * @return Pointer to the game manager singleton
 */
GameManager_t* get_game_manager();

/**
 * @brief Sets up a new level. For careful use by the main menu 
 * @warning Before calling these fuctions, set the gm->gamemode to the correct one
 */
void gm_start_level();
/**
 * @brief Sets up a new switchboard. For careful use by the main menu 
 * @warning Before calling these fuctions, set the gm->gamemode to the correct one
 */
void gm_start_switchboard();
/**
 * @brief Sets up a new arcade. For careful use by the main menu 
 * @warning Before calling these fuctions, set the gm->gamemode to the correct one
 */
void gm_start_arcade();
/**
 * @brief Sets up a new main menu. For careful use by the main menu 
 * @warning Before calling these fuctions, set the gm->gamemode to the correct one
 */
void gm_start_main_menu();

/**
 * @brief Used for exiting the game, even on catastrophic errors
 */
void exit_game();

/** 
 * @brief Starts (and plays) the game
 * @returns 0 on success, 1 otherwise
 */
uint8_t start_game(bool override_path, char *assets_path);
