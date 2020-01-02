#pragma once

#include <lcom/lcf.h>

#include "level.h"
#include "switchboard.h"
#include "main_menu.h"

extern char* assets_rel_path;

#define UART_GRAVITY_NORMAL 0
#define UART_GRAVITY_REVERSED 1

// number of bytes of each kind of message
#define HEADER_SYNCED_SIZE 2
#define HEADER_AVAILABLE_LEVEL_SIZE 2
#define HEADER_AVAILABLE_ARCADE_SIZE 2

// Campaign
#define HEADER_SPEED_MULT_SIZE 3
#define HEADER_JUMP_MULT_SIZE 3
#define HEADER_LASER_SIZE 3
#define HEADER_GRAVITY_SIZE 3 
#define HEADER_PLAYER_RESPAWN_SIZE 2 
#define HEADER_PLAYER_UPDATE_SIZE 3
#define HEADER_REQUEST_POWERS_SIZE 2
#define HEADER_RESPONSE_POWERS_SIZE 3

// Arcade
#define HEADER_PLAYER_TWO_UPDATE_SIZE 8
#define HEADER_ARCADE_READY_SIZE 2

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

// Fake singleton but kinda
typedef struct GameManager {
	Level_t *level;
	SwitchBoard_t *s_board;
    MainMenu_t *main_menu;
    void (*update_function[32])();
    void (*render_function[32])();
    uint32_t esc_counter; // Used to go back
    bool game_ongoing; // Used to exit the game
    bool uart_synced; // Used to know if the two players are connected or not
    bool has_partner;
    uint32_t uart_last_sent, uart_last_received;
	GameModeEnum gamemode;
    Sprite_t *connecting_sprite;
} GameManager_t;

// Please only call when start_game has already been called
GameManager_t* get_game_manager();

void gm_start_level();
void gm_start_switchboard();
void gm_start_arcade();
void gm_start_main_menu();

/**
 * @brief Used for exiting the game, even on catastrophic errors
 */
void exit_game();

/** 
 * @brief Starts the game
 * @param gamemode Indicates the gamemode to start the game on (usually the Menu)
 * @returns 0 on success, 1 otherwise
 */
uint8_t start_game(GameModeEnum gamemode);
