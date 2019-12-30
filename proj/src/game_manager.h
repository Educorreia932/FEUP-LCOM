#pragma once

#include <lcom/lcf.h>

#include "level.h"
#include "switchboard.h"
#include "main_menu.h"

extern char* assets_rel_path;

#define UART_TERMINATOR 0xFF

enum UART_EVENT_HEADER {

    HEADER_SYNCED = 's',

    HEADER_LASER = 'L',
    HEADER_SPEED_MULT = 'S',
    HEADER_JUMP_MULT = 'J',
    HEADER_PLAYER_DEATH = 'D',
    HEADER_PLAYER_ALIVE = 'A',
    HEADER_POWERS_UNLOCKED = 'P',
    HEADER_PLAYER_UPDATE = 'U'
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
    uint32_t esc_counter;
    bool game_ongoing;
	GameModeEnum gamemode;
} GameManager_t;

// Please only call when start_game has already been called
GameManager_t* get_game_manager();

void gm_start_level();
void gm_start_switchboard();
void gm_start_arcade();
void gm_start_main_menu();

uint8_t start_game(GameModeEnum gamemode);
