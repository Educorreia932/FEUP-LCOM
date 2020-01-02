#pragma once

#include <lcom/lcf.h>
#include "input_events.h"
#include "platforms.h"
#include "lasers.h"
#include "spikes.h"
#include "power_ups.h"

typedef struct Player Player_t;

typedef enum PlayerUnlockedPowers {
    UNLOCKED_NOTHING = 0,
    UNLOCKED_SPEED = 1,
    UNLOCKED_JUMP = 2,
    UNLOCKED_LASERS = 4,
    UNLOCKED_GRAVITY = 8,
    UNLOCKED_ALL = 15
} PlayerUnlockedPowers;

PlayerUnlockedPowers player_get_default_powers();
// void player_unlock_speed();
// void player_unlock_jump();
void player_unlock_lasers();
void player_unlock_gravity();

void player_win();

void player_set_speed(uint8_t speed);
void player_set_jump(uint8_t jump);
void player_set_gravity_normal();
void player_set_gravity_reversed();


Player_t* new_player(bool ui_controls, bool arcade_mode, PlayerUnlockedPowers default_powers);

void free_player(Player_t* player);

void update_player(Player_t* player, Platforms_t* plat, Lasers_t* lasers, Spikes_t* spikes, PowerUp_t* pu[], int* laser_pos);

void animator_player(Player_t* player);
void render_player_background(Player_t* player);
void render_player_foreground(Player_t* player);
void render_player_ui(Player_t *player);

typedef struct PlayerTwo PlayerTwo_t;

PlayerTwo_t* new_player_two();
void free_player_two(PlayerTwo_t* player_two);

void render_player_two_background(PlayerTwo_t* player_two);
void render_player_two_foreground(PlayerTwo_t* player_two);
void render_player_two_ui(PlayerTwo_t* player_two);

void update_player_two(PlayerTwo_t* player_two, uint8_t bytes[], int* laser_pos);
