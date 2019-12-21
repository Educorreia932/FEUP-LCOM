#pragma once

#include <lcom/lcf.h>

#include "level.h"
#include "mouse_cursor.h"
#include "switchboard.h"

extern char* assets_rel_path;

typedef enum GameMode {
    GM_UART = 1,
    GM_LEVEL = 2,
    GM_SWITCHBOARD = 4,
    GM_ARCADE = 8,
    // Specific combinations
    GM_LEVEL_UART = 3,
    GM_ARCADE_UART = 9,
    GM_SWITCHBOARD_UART = 5 // This one is dumb and works as a backup
} GameModeEnum;

// Fake singleton but kinda
typedef struct GameManager {
	Level_t *level;
	SwitchBoard_t *s_board;
    void (*update_function[16])();
    void (*render_function[16])();
	GameModeEnum gamemode;
} GameManager_t;

// Please only call when start_game has already been called
GameManager_t* get_game_manager();

uint8_t start_game(GameModeEnum gamemode);
