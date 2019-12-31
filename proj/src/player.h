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

// void player_unlock_speed();
// void player_unlock_jump();
void player_unlock_lasers();
void player_unlock_gravity();

void player_win();

void player_set_speed(uint8_t speed);
void player_set_jump(uint8_t jump);
// Grav direction should be -1 or 1
void player_set_gravity(int8_t grav_direction);

Player_t* new_player(bool ui_controls, bool arcade_mode, PlayerUnlockedPowers default_powers);

void free_player(Player_t* player);

void update_player(Player_t* player, Platforms_t* plat, Lasers_t* lasers, Spikes_t* spikes, PowerUp_t* pu[]);

void animator_player(Player_t* player);
void render_player_background(Player_t* player);
void render_player_foreground(Player_t* player);
void render_player_ui(Player_t *player);

typedef struct PlayerTwo PlayerTwo_t;

PlayerTwo_t* new_player_two();

void render_player_player_two(PlayerTwo_t* player_two);

void update_player_two(Player_t* player_two, uint16_t x, uint16_t y, bool is_dead);

void send_info();
