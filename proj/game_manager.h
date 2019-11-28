#pragma once

#include <lcom/lcf.h>

typedef struct GameManager GameManager_t;
GameManager_t* new_game_manager(const char* background_file_name);
GameManager_t* new_testing_game_manager(uint8_t player_number);
void free_game_manager(GameManager_t *gm);

uint8_t start_game(uint8_t player_number);
