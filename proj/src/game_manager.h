#pragma once

#include <lcom/lcf.h>

#include "level.h"
#include "mouse_cursor.h"
#include "switchboard.h"

extern const char* assets_rel_path;

enum PlayerNumber {
    PLAYER_1 = 1,
    PLAYER_2 = 2,
    SINGLEPLAYER = 4
};

// Fake singleton but kinda
typedef struct GameManager {
	Level_t *level;
	SwitchBoard_t *s_board;
	enum PlayerNumber player_number;
} GameManager_t;

// Please only call when start_game has already been called
GameManager_t* get_game_manager();

uint8_t start_game(enum PlayerNumber);
