#include "player.h"
#include "level.h"
#include "geometry.h"
#include "math_utils.h"
#include "sprite.h"
#include "ui_elements.h"
#include "game_manager.h"
#include "hw_manager.h"
#include "power_ups.h"

/** @defgroup player Player
 * @{
 */

/* PHYSICS STUFF */
#define BASE_GRAVITY 800.0f
#define FALLING_MULT 1.6f
#define MAX_VELOCITY 800.0f

/* PLAYER CONSTANTS */

#define IS_GROUNDED_MARGIN 8.0f

// These are (hopefully) measured in frames
#define PLAYER_GRAVITY_SWITCH_ANIMATION_TIME 30
#define PLAYER_IDLE_COUNTDOWN_OPEN 75
#define PLAYER_IDLE_COUNTDOWN_CLOSED 20
#define PLAYER_SPARK_COUNTDOWN 20
#define PLAYER_MINIMUM_WALK_COUNTDOWN 2
#define PLAYER_MAXIMUM_WALK_COUNTDOWN 10

#define PLAYER_RESPAWN_TIME 30 // Frames
#define PLAYER_BASE_SPEED 300.0f // Raw pixels
#define PLAYER_BASE_JUMP 575.0f

#define PLAYER_DEFAULT_SPEED_MULT 1.0f
#define PLAYER_MIN_SPEED_MULT 0.2f
#define PLAYER_MAX_SPEED_MULT 1.8f
#define PLAYER_SPEED_MULT_STEP 0.01f
#define PLAYER_DEFAULT_JUMP_MULT 1.0f
#define PLAYER_MIN_JUMP_MULT 0.4f
#define PLAYER_MAX_JUMP_MULT 1.6f
#define PLAYER_JUMP_MULT_STEP 0.01f

struct Player {
	Rect_t rect;
	Sprite_t *idle_sprite, *walk_sprite, *sparks_sprite;
	float speed_mult, jump_mult;
	float y_speed, gravity;
	float x_spawn, y_spawn;
	
	bool ui_controls, arcade_mode;
	uint8_t respawn_timer; /**< @brief If != 0, player is dead */

	bool heading_right, is_idle, grounded;

	int seconds_beginning;

	PlayerUnlockedPowers default_powers, current_powers;

	// Animation stuff
	uint8_t anim_idle_countdown, anim_walk_countdown, anim_spark_countdown;

	// Single Player UI ~ NULL ptr if multiplayer
	Slider_t *jump_slider, *speed_slider;
	Button_t **laser_buttons;

	Score_t* score; /**< @note Only used in Arcade mode  */
};

// Forward declaration in order to be used in new_player
static uint8_t player_walk_countdown_value(Player_t* player);
// Forward declaration in order to be used in update_player
static void player_send_info(Player_t* player, bool score_update);

/* UNLOCKING POWERS */

PlayerUnlockedPowers player_get_default_powers() {
	return get_game_manager()->level->player->default_powers;
}

static void player_unlock_powers(PlayerUnlockedPowers powers_to_give) {
	// Multiplayer
	if (get_game_manager()->gamemode & GM_UART) {
		hw_manager_uart_send_char(HEADER_PLAYER_UPDATE);
		hw_manager_uart_send_char((uint8_t) powers_to_give);
		hw_manager_uart_send_char(HEADER_TERMINATOR);
	}

	// Gets the player from anywhere
	get_game_manager()->level->player->current_powers |= powers_to_give;
}

void player_unlock_speed() {
	player_unlock_powers(UNLOCKED_SPEED);
	if (get_game_manager()->level->player->ui_controls) {
		slider_activate(get_game_manager()->level->player->speed_slider);
	}
}

void player_unlock_jump() {
	player_unlock_powers(UNLOCKED_JUMP);
	if (get_game_manager()->level->player->ui_controls) {
		slider_activate(get_game_manager()->level->player->jump_slider);
	}
}

void player_unlock_lasers() {
	player_unlock_powers(UNLOCKED_LASERS);
	if (get_game_manager()->level->player->ui_controls) {
		Player_t *p = get_game_manager()->level->player;
		button_activate(p->laser_buttons[0]);
		button_activate(p->laser_buttons[1]);
		button_activate(p->laser_buttons[2]);
	}
}

void player_unlock_gravity() {
	player_unlock_powers(UNLOCKED_GRAVITY);
}

void player_win() {
	int seconds_beginning =	get_game_manager()->level->player->seconds_beginning;
	int seconds_end = hw_manager_rtc_read_date_in_seconds();
	int seconds_difference = seconds_end - seconds_beginning;
	//Score_t* time_took = new_score(800, 70, seconds_difference, 5);
	printf("It took you %u seconds\n", seconds_difference);
}

// PLAYER UPDATES

void player_set_speed(uint8_t speed) {
	// Gets the player from anywhere
	Player_t *p = get_game_manager()->level->player;
	if (p->current_powers & UNLOCKED_JUMP) {
		float mult = PLAYER_MIN_SPEED_MULT + (
			(PLAYER_MIN_SPEED_MULT - PLAYER_MAX_SPEED_MULT) /
			(0 - 255)
		) * speed;

		// Gets the player from anywhere
		p->speed_mult = mult;
	}
}

void player_set_jump(uint8_t jump) {
	// Gets the player from anywhere
	Player_t *p = get_game_manager()->level->player;
	if (p->current_powers & UNLOCKED_JUMP) {
		float mult = PLAYER_MAX_JUMP_MULT + (
			(PLAYER_MIN_JUMP_MULT - PLAYER_MAX_JUMP_MULT) /
			(255 - 0)
		) * jump;

		p->jump_mult = mult;
	}
}

void player_set_gravity_normal() {
	Player_t *p = get_game_manager()->level->player;
	if (p->current_powers & UNLOCKED_GRAVITY)
		p->gravity = BASE_GRAVITY;
}

void player_set_gravity_reversed() {
	Player_t *p = get_game_manager()->level->player;
	if (p->current_powers & UNLOCKED_GRAVITY)
		p->gravity = -BASE_GRAVITY;
}

static void player_switch_gravity() {
	// Gets the player from anywhere
	Player_t *p = get_game_manager()->level->player;
	if (p->current_powers & UNLOCKED_GRAVITY)
		p->gravity *= -1;
}

static void player_set_laser_0() {
	if (get_game_manager()->level->player->current_powers & UNLOCKED_LASERS)
		lasers_set_link_id(get_game_manager()->level->lasers, 0);
}

static void player_set_laser_1() {
	if (get_game_manager()->level->player->current_powers & UNLOCKED_LASERS)
		lasers_set_link_id(get_game_manager()->level->lasers, 1);
}

static void player_set_laser_2() {
	if (get_game_manager()->level->player->current_powers & UNLOCKED_LASERS)	lasers_set_link_id(get_game_manager()->level->lasers, 2);
}

Player_t* new_player(bool ui_controls, bool arcade_mode, PlayerUnlockedPowers default_powers) { //TODO: Specify spawn
	Player_t* player = (Player_t*) calloc(1, sizeof(Player_t));
	
	if (player == NULL) {
		printf("new_player: Failed to allocate memory for the player object\n");
		return NULL;
	}

	// Idle animation
	player->idle_sprite = new_sprite(0, 0, 2, 
		"player/idle_0.bmp",
		"player/idle_1.bmp"
	);
	if (player->idle_sprite == NULL) {
		printf("new_player: Failed to create the idle animation's Sprite object\n");
		free(player);
		return NULL;
	}

	// Walking animation
	player->walk_sprite = new_sprite(0, 0, 5, 
		"player/walk_0.bmp",
		"player/walk_1.bmp",
		"player/walk_2.bmp",
		"player/walk_3.bmp",
		"player/walk_4.bmp"
	);
	if (player->idle_sprite == NULL) {
		printf("new_player: Failed to create the walk animation's Sprite object\n");
		free_sprite(player->idle_sprite);
		free(player);
		return NULL;
	}

	// The background sparks
	player->sparks_sprite = new_sprite(-8, -2, 4,
		"player/spark_0.bmp",
		"player/spark_1.bmp",
		"player/spark_2.bmp",
		"player/spark_3.bmp"
	);
	if (player->sparks_sprite == NULL) {
		printf("new_player: Failed to create the sparks' Sprite object\n");
		free_sprite(player->idle_sprite);
		free_sprite(player->walk_sprite);
		free(player);
		return NULL;
	}

	// Customizing player stats
	player->speed_mult = PLAYER_DEFAULT_SPEED_MULT;
	player->jump_mult = PLAYER_DEFAULT_JUMP_MULT;
	player->y_speed = 0.0f;
	player->gravity = BASE_GRAVITY;
	player->x_spawn = 60.0f;
	player->y_spawn = 704.0f;
	player->respawn_timer = 0;
	player->heading_right = true;
	player->is_idle = true;
	player->grounded = false;
	player->anim_idle_countdown = PLAYER_IDLE_COUNTDOWN_OPEN;
	player->anim_walk_countdown = player_walk_countdown_value(player);
	player->anim_spark_countdown = PLAYER_SPARK_COUNTDOWN;

	player->default_powers = default_powers;
	player->current_powers = default_powers;

	// Creating player hitbox
	player->rect = rect(
		player->x_spawn,
		player->y_spawn, 
		(float) sprite_get_width(player->idle_sprite), 
		(float) sprite_get_height(player->idle_sprite)
	);

	player->arcade_mode = arcade_mode;
	player->ui_controls = ui_controls;

	if (ui_controls) {
		// Creating the extra single player UI 
		player->jump_slider = new_slider("ui/small_vertical_slider.bmp", "ui/small_slider_handle.bmp", player_set_jump, vec2d(2, 20), 255, vec2d(4, 25), vec2d(4, 90));
		if (player->jump_slider == NULL) {
			printf("new_player: Failed to create jump slider\n");
			free_sprite(player->idle_sprite);
			free_sprite(player->walk_sprite);
			free_sprite(player->sparks_sprite);
			free(player);
			return NULL;
		}
		if (!(player->current_powers & UNLOCKED_JUMP))
			slider_deactivate(player->jump_slider);
		
		player->speed_slider = new_slider("ui/small_horizontal_slider.bmp", "ui/small_slider_handle.bmp", player_set_speed, vec2d(20, 2), 255, vec2d(25, 4), vec2d(90, 4));
		if (player->speed_slider == NULL) {
			printf("new_player: Failed to create speed slider\n");
			free_sprite(player->idle_sprite);
			free_sprite(player->walk_sprite);
			free_sprite(player->sparks_sprite);
			free_slider(player->jump_slider);
			free(player);
			return NULL;
		}
		if (!(player->current_powers & UNLOCKED_SPEED))
			slider_deactivate(player->speed_slider);
		player->laser_buttons = (Button_t**) calloc(3, sizeof(Button_t*));
		if (player->laser_buttons == NULL) {
			printf("new_player: Failed to allocate memory for the three buttons\n");
			free_sprite(player->idle_sprite);
			free_sprite(player->walk_sprite);
			free_sprite(player->sparks_sprite);
			free_slider(player->jump_slider);
			free_slider(player->speed_slider);
			free(player);
			return NULL;
		}

		player->laser_buttons[0] = NULL;
		player->laser_buttons[0] = new_button("ui/small_laser_button_red.bmp", player_set_laser_0, rect(130.0f, 4.0f, 16.0f, 16.0f));
		if (player->laser_buttons[0] == NULL) {
			free_sprite(player->idle_sprite);
			free_sprite(player->walk_sprite);
			free_sprite(player->sparks_sprite);
			free_slider(player->jump_slider);
			free_slider(player->speed_slider);
			free(player->laser_buttons);
			free(player);
			return NULL;
		}
		player->laser_buttons[1] = new_button("ui/small_laser_button_blue.bmp", player_set_laser_1, rect(160.0f, 4.0f, 16.0f, 16.0f));
				if (player->laser_buttons[0] == NULL) {
			printf("new_player: Failed to create red laser button\n");
			free_sprite(player->idle_sprite);
			free_sprite(player->walk_sprite);
			free_sprite(player->sparks_sprite);
			free_slider(player->jump_slider);
			free_slider(player->speed_slider);
			free_button(player->laser_buttons[0]);
			free(player->laser_buttons);
			free(player);
			return NULL;
		}
		player->laser_buttons[2] = new_button("ui/small_laser_button_pink.bmp", player_set_laser_2, rect(190.0f, 4.0f, 16.0f, 16.0f));
		if (player->laser_buttons[0] == NULL) {
			printf("new_player: Failed to create red laser button\n");
			free_sprite(player->idle_sprite);
			free_sprite(player->walk_sprite);
			free_sprite(player->sparks_sprite);
			free_slider(player->jump_slider);
			free_slider(player->speed_slider);
			free_button(player->laser_buttons[0]);
			free_button(player->laser_buttons[1]);
			free(player->laser_buttons);
			free(player);
			return NULL;
		}
		if (!(player->current_powers & UNLOCKED_LASERS)) {
			button_deactivate(player->laser_buttons[0]);
			button_deactivate(player->laser_buttons[1]);
			button_deactivate(player->laser_buttons[2]);
		}

	}

	// printf("new_testing_player: Finished making player\n");
	player->score = NULL;

	// Arcade
	if (player->arcade_mode)
		player->score = new_score(800, 75, 0, 3);
	
	player->seconds_beginning = hw_manager_rtc_read_date_in_seconds();

	return player;
}

void free_player(Player_t* player) {
	if (player == NULL) {
		printf("free_player: Cannot free a NULL pointer\n");
		return;
  	}
	
	free_sprite(player->idle_sprite);
	free_sprite(player->walk_sprite);
	free_sprite(player->sparks_sprite);

	if (player->ui_controls) {
		free_slider(player->jump_slider);
		free_slider(player->speed_slider);

		free_button(player->laser_buttons[0]);
		free_button(player->laser_buttons[1]);
		free_button(player->laser_buttons[2]);

		free(player->laser_buttons);
	}

	free(player);
}

inline static bool player_is_grav_inverted(Player_t *player) {
	return player->gravity < 0 ? true : false;
}

// TODO: This is_grounded is very sketchy, improve it later down the road
static bool player_is_grounded(Player_t* player, Platforms_t* plat) {
	Rect_t r;
	if (player->gravity > 0) {
		r = rect(
					player->rect.x + 2,
					player->rect.y + player->rect.h,
					player->rect.w - 2,
					IS_GROUNDED_MARGIN
				);
	}
	else {
		r = rect(
			player->rect.x + 2,
			player->rect.y - IS_GROUNDED_MARGIN,
			player->rect.w - 2,
			IS_GROUNDED_MARGIN
		);
	}
	return does_collide_platforms(plat, &r);
}

static inline void player_respawn(Player_t *player) {
	player->rect.x = player->x_spawn;
	player->rect.y = player->y_spawn;
	player->y_speed = 0;
	player->gravity = BASE_GRAVITY;
	player->heading_right = true;
	// player->speed_mult = PLAYER_DEFAULT_SPEED_MULT;
	// player->jump_mult = PLAYER_DEFAULT_JUMP_MULT;
	player->respawn_timer = 0;

	player->current_powers = player->default_powers;

	if (player->ui_controls) {
		if (!(player->default_powers & UNLOCKED_SPEED))
			slider_deactivate(player->speed_slider);
		if (!(player->default_powers & UNLOCKED_JUMP))
			slider_deactivate(player->jump_slider);
		if (!(player->default_powers & UNLOCKED_LASERS)) {
			button_deactivate(player->laser_buttons[0]);
			button_deactivate(player->laser_buttons[1]);
			button_deactivate(player->laser_buttons[2]);
		}
	}

	PowerUp_t **power_ups = get_game_manager()->level->pu; 
	for (uint8_t i = 0; i < MAX_POWERUPS; ++i) {
		if (power_ups[i] != NULL)
			respawn_powerup(power_ups[i]);
	}

	player->anim_idle_countdown = PLAYER_IDLE_COUNTDOWN_OPEN;
	player->anim_walk_countdown = player_walk_countdown_value(player);
	player->anim_spark_countdown = PLAYER_SPARK_COUNTDOWN;

	set_animation_state(player->idle_sprite, 0);
	set_animation_state(player->walk_sprite, 0);
	set_animation_state(player->sparks_sprite, 0);
	
	if (get_game_manager()->gamemode & GM_UART) {
		hw_manager_uart_send_char(HEADER_PLAYER_RESPAWN);
		hw_manager_uart_send_char(HEADER_TERMINATOR);
	}
}

static inline void player_death_cycle(Player_t *player) {
	if (player->respawn_timer != 1)
		--player->respawn_timer;
	
	else {
		if (player->arcade_mode)
			reset_score(player->score);

		player_respawn(player);
	}
}

static inline void player_start_death(Player_t *player) {
	set_animation_state(player->idle_sprite, 1);
	player->current_powers = player->default_powers;
	player->respawn_timer = PLAYER_RESPAWN_TIME + 1;
}

void update_player(Player_t* player, Platforms_t* plat, Lasers_t* lasers, Spikes_t* spikes, PowerUp_t* pu[]) {
	Rect_t previous_rect = player->rect;
	bool score_update = false, died = false;

	// Horizontal Movement
	float h_delta = 0;

	if (player->respawn_timer == 0) {
		if (player->ui_controls) {
			update_slider(player->jump_slider);
			update_slider(player->speed_slider);

			update_button(player->laser_buttons[0]);
			update_button(player->laser_buttons[1]);
			update_button(player->laser_buttons[2]);

			if (get_key_down(KBD_X))
				player_switch_gravity();
		}

		if (player->arcade_mode) {
			if (get_key_down(KBD_X))
				player_switch_gravity();
		}

		int8_t direction = 0;

		if (get_key(KBD_ARROW_RIGHT) || get_key(KBD_D)) {
			++direction;
			player->heading_right = true;
		}
		
		if (get_key(KBD_ARROW_LEFT) || get_key(KBD_A)) {
			--direction;
			player->heading_right = false;
		}

		h_delta = direction * DELTATIME * PLAYER_BASE_SPEED * player->speed_mult;
	}

	if (h_delta != 0) {
		player->is_idle = false;

		player->rect.x += h_delta / 2;
		
		if (does_collide_platforms(plat, &player->rect)) {
			player->rect.x -= h_delta / 4;

			if (does_collide_platforms(plat, &player->rect))
				player->rect = previous_rect;
		}

		else {	
			previous_rect = player->rect;
			player->rect.x += h_delta / 2;
			if (does_collide_platforms(plat, &player->rect))
				player->rect = previous_rect;
		}
	}

	else
		player->is_idle = true;

	// Vertical Movement
	previous_rect = player->rect;

	if (player->y_speed * player->gravity > 0)
		player->y_speed += DELTATIME * player->gravity;
	
	else
		player->y_speed += DELTATIME * FALLING_MULT * player->gravity;

	// Jump button
	player->grounded = false;
	if (player->respawn_timer == 0)
		// Player is alive
		player->grounded = player_is_grounded(player, plat);
		if (get_key_down(KBD_Z) || get_key_down(KBD_SPACEBAR))
			if (player->grounded)
				player->y_speed = -PLAYER_BASE_JUMP * player->jump_mult * fsign(player->gravity);

	// Kinda like terminal velocity
	player->y_speed = fclampf(player->y_speed, -MAX_VELOCITY, MAX_VELOCITY);

	player->rect.y += player->y_speed * DELTATIME
		+ 0.5f * powf(DELTATIME, 2) * player->gravity;
	
	if (does_collide_platforms(plat, &player->rect)) {
		player->rect = previous_rect;
		player->y_speed /= 3;
	}

	for (uint8_t i = 0; i < MAX_POWERUPS; ++i) {
		if (pu[i] != NULL)
			update_power_up(pu[i], &(player->rect));
	}

	if (player->respawn_timer == 0) {
		if (player_is_dead(lasers, &player->rect) || player_touches_spike(spikes, &player->rect))  {
			died = true;
			player_start_death(player);
		}
	}

	else
		player_death_cycle(player);
	
	animator_player(player);

	if (player->arcade_mode) {
		if (arcade_player_passes_lasers(lasers, &player->rect)) {
			update_score(player->score);
			score_update = true;
		}

		if (get_game_manager()->gamemode & GM_UART)
			player_send_info(player, score_update);
	}
}

/*
-----------------------------------
|     Rendering & Animations      |
-----------------------------------
*/

static uint8_t player_walk_countdown_value(Player_t* player) {
	return (uint8_t) (PLAYER_MAXIMUM_WALK_COUNTDOWN
		+ ( // Ponto da reta [0, 1] a desenhar
			(player->speed_mult - PLAYER_MIN_SPEED_MULT)
			/ (PLAYER_MAX_SPEED_MULT - PLAYER_MIN_SPEED_MULT)
		)
		// Declive
		* (PLAYER_MINIMUM_WALK_COUNTDOWN - PLAYER_MAXIMUM_WALK_COUNTDOWN)
	);
}

// For now, both run independently
void animator_player(Player_t* player) {
	// If player is alive
	if (player->respawn_timer == 0) {

		// Treat Sparks
		if (player->anim_spark_countdown == 0) {
			uint8_t state = get_animation_state(player->sparks_sprite);
			set_animation_state(player->sparks_sprite, (state+1) % 4);
			player->anim_spark_countdown = PLAYER_SPARK_COUNTDOWN;
		}
		else
			--player->anim_spark_countdown;

		// Foreground
		if (player->anim_idle_countdown == 0) {
			uint8_t state = get_animation_state(player->idle_sprite);
			if (state == 0)  // These are apparently the other way around, but they mean
			// "Time to wait for the next animation", so when currently open
			// we set the waiting time for it to be closed
				player->anim_idle_countdown = PLAYER_IDLE_COUNTDOWN_CLOSED;
			else
				player->anim_idle_countdown = PLAYER_IDLE_COUNTDOWN_OPEN;
			set_animation_state(player->idle_sprite, (state+1) % 2);
		}
		else
			--player->anim_idle_countdown;

		if (player->anim_walk_countdown == 0) {
			uint8_t state = get_animation_state(player->walk_sprite);
			set_animation_state(player->walk_sprite, (state+1) % 5);
			player->anim_walk_countdown = player_walk_countdown_value(player);
		}
		else
			--player->anim_walk_countdown;
		
	}

}

void render_player_background(Player_t* player) {
	if (player->respawn_timer == 0 && player->gravity != BASE_GRAVITY) {
		// Player is alive
		draw_sprite(player->sparks_sprite, &player->rect, COLOR_NO_MULTIPLY, SPRITE_Y_AXIS * !player->heading_right);
	}
}

void render_player_foreground(Player_t* player) {
	SpriteReverse sr = SPRITE_Y_AXIS * player->heading_right
		+ SPRITE_X_AXIS * player_is_grav_inverted(player);

	if (player->respawn_timer == 0)
		// Player is alive
		if (player->grounded && player->is_idle)
			draw_sprite(player->idle_sprite, &player->rect, COLOR_NO_MULTIPLY, sr);
		else
			draw_sprite(player->walk_sprite, &player->rect, COLOR_NO_MULTIPLY, sr);
	else {
		// Player is dead
		draw_sprite(player->idle_sprite, &player->rect, COLOR_RED, sr);
	}
}

void render_player_ui(Player_t *player) {
	if (player->ui_controls) {
		render_slider(player->speed_slider);
		render_slider(player->jump_slider);

		render_button(player->laser_buttons[0]);
		render_button(player->laser_buttons[1]);
		render_button(player->laser_buttons[2]);
	}

	if (player->score != NULL) // Arcade mode 
		render_score(player->score);
}

#define PLAYER_TWO_X_MSB 0
#define PLAYER_TWO_X_LSB 1
#define PLAYER_TWO_Y_MSB 2
#define PLAYER_TWO_Y_LSB 3
#define PLAYER_TWO_ADDITIONAL_1 4
#define PLAYER_TWO_ADDITIONAL_2 5

#define PLAYER_TWO_ANIMATION_MASK (BIT(0) | BIT(1) | BIT(2)) // Player and Spark
#define PLAYER_TWO_IS_DEAD BIT(4)
#define PLAYER_TWO_IS_IDLE BIT(5)
#define PLAYER_TWO_HEADING_RIGHT BIT(6)
#define PLAYER_TWO_ANTI_GRAVITY BIT(7)

#define PLAYER_TWO_INCREASE_SCORE BIT(4)

/** 
 * @brief Representation of the second player while playing Arcade mode in versus
 * @note Only used in Arcade mode 
 */

struct PlayerTwo {
	Vec2d_t pos;
	Sprite_t* idle_sprite;
	Sprite_t* walk_sprite;
	Sprite_t* sparks_sprite;
	
	bool heading_right, is_idle, is_dead, anti_gravity;

	Score_t* score;
};

PlayerTwo_t* new_player_two() { 
	PlayerTwo_t* player_two = (PlayerTwo_t*) calloc(1, sizeof(PlayerTwo_t));
	
	if (player_two == NULL) {
		printf("new_player: Failed to allocate memory for the player two object\n");
		return NULL;
	}

	// Idle animation
	player_two->idle_sprite = new_sprite(0, 0, 2, 
		"player/idle_0.bmp",
		"player/idle_1.bmp"
	);
	if (player_two->idle_sprite == NULL) {
		printf("new_player: Failed to create the idle animation's Sprite object\n");
		free(player_two);
		return NULL;
	}

	// Walking animation
	player_two->walk_sprite = new_sprite(0, 0, 5, 
		"player/walk_0.bmp",
		"player/walk_1.bmp",
		"player/walk_2.bmp",
		"player/walk_3.bmp",
		"player/walk_4.bmp"
	);
	if (player_two->idle_sprite == NULL) {
		printf("new_player: Failed to create the walk animation's Sprite object\n");
		free_sprite(player_two->idle_sprite);
		free(player_two);
		return NULL;
	}

	// The background sparks
	player_two->sparks_sprite = new_sprite(-8, -2, 4,
		"player/spark_0.bmp",
		"player/spark_1.bmp",
		"player/spark_2.bmp",
		"player/spark_3.bmp"
	);
	if (player_two->sparks_sprite == NULL) {
		printf("new_player_two: Failed to create the sparks' Sprite object\n");
		free_sprite(player_two->idle_sprite);
		free_sprite(player_two->walk_sprite);
		free(player_two);
		return NULL;
	}

	player_two->pos = vec2d(60.0f, 704.0f);

	player_two->score = new_score(800, 140, 0, 3);

	if (player_two->score == NULL) {
        printf("new_player_two: Failed to create the score\n");
		free_sprite(player_two->idle_sprite);
		free_sprite(player_two->walk_sprite);
        free(player_two);
        return NULL;
    }

	return player_two;
}

void free_player_two(PlayerTwo_t* player_two) {

	free_sprite(player_two->idle_sprite);
	free_sprite(player_two->walk_sprite);
	free_sprite(player_two->sparks_sprite);

	free_score(player_two->score);

	free(player_two);

}

void update_player_two(PlayerTwo_t* player_two, uint8_t bytes[]) {
	player_two->pos = vec2d((float) ((bytes[PLAYER_TWO_X_MSB] << 8) | bytes[PLAYER_TWO_X_LSB]), (float) ((bytes[PLAYER_TWO_Y_MSB] << 8) | bytes[PLAYER_TWO_Y_LSB]));

	player_two->is_dead = bytes[PLAYER_TWO_ADDITIONAL_1] & PLAYER_TWO_IS_DEAD;
	player_two->is_idle = bytes[PLAYER_TWO_ADDITIONAL_1] & PLAYER_TWO_IS_IDLE;
	player_two->heading_right = bytes[PLAYER_TWO_ADDITIONAL_1] & PLAYER_TWO_HEADING_RIGHT;
	player_two->anti_gravity = bytes[PLAYER_TWO_ADDITIONAL_1] & PLAYER_TWO_ANTI_GRAVITY;

	if (player_two->is_idle)
		set_animation_state(player_two->idle_sprite, bytes[PLAYER_TWO_ADDITIONAL_1] & PLAYER_TWO_ANIMATION_MASK);
	else
		set_animation_state(player_two->walk_sprite, bytes[PLAYER_TWO_ADDITIONAL_1] & PLAYER_TWO_ANIMATION_MASK);

	set_animation_state(player_two->sparks_sprite, bytes[PLAYER_TWO_ADDITIONAL_2] & PLAYER_TWO_ANIMATION_MASK);

	if (bytes[PLAYER_TWO_ADDITIONAL_2] & PLAYER_TWO_INCREASE_SCORE)
		update_score(player_two->score);

	if (player_two->is_dead)
		reset_score(player_two->score);

}

void render_player_two_background(PlayerTwo_t* player_two) {
	if (!player_two->is_dead && player_two->anti_gravity) {
		// Player is alive
		draw_sprite_vec2d(player_two->sparks_sprite, player_two->pos, COLOR_BLUE, SPRITE_NORMAL);
	}
}

void render_player_two_foreground(PlayerTwo_t* player_two) {
	SpriteReverse sr = SPRITE_Y_AXIS * player_two->heading_right
		+ SPRITE_X_AXIS * player_two->anti_gravity;

	if (!player_two->is_dead)
		if (player_two->is_idle)
			draw_sprite_vec2d(player_two->idle_sprite, player_two->pos, COLOR_BLUE, sr);
		
		else
			draw_sprite_vec2d(player_two->walk_sprite, player_two->pos, COLOR_BLUE, sr);

	else 
		// Player is dead
		draw_sprite_vec2d(player_two->idle_sprite, player_two->pos, COLOR_PINK, sr);
}

void render_player_two_ui(PlayerTwo_t* player_two) {
	render_score(player_two->score);
}


static void player_send_info(Player_t* player, bool score_update) {
	int16_t x = (int16_t) player->rect.x;
	int16_t y = (int16_t) player->rect.y;

	uint8_t x_lsb, x_msb, y_lsb, y_msb, additional_byte_1 = 0, additional_byte_2 = 0;
	
	if (util_get_LSB(x, &x_lsb) || util_get_MSB(x, &x_msb) || util_get_LSB(y, &y_lsb) || util_get_MSB(y, &y_msb)) 
		return; 

	if (player->is_idle && player->grounded) {
		additional_byte_1 = get_animation_state(player->idle_sprite) & PLAYER_TWO_ANIMATION_MASK;
		additional_byte_1 |= PLAYER_TWO_IS_IDLE;
	}
	else
		additional_byte_1 = get_animation_state(player->walk_sprite) & PLAYER_TWO_ANIMATION_MASK;

	if (player->respawn_timer != 0)
		additional_byte_1 |= PLAYER_TWO_IS_DEAD;
	additional_byte_1 |= (player->heading_right << 6);
	if (player_is_grav_inverted(player))
		additional_byte_1 |= PLAYER_TWO_ANTI_GRAVITY;

	additional_byte_2 = get_animation_state(player->sparks_sprite) & PLAYER_TWO_ANIMATION_MASK;
	additional_byte_2 |= (score_update << 4);

	hw_manager_uart_send_char(HEADER_PLAYER_TWO_UPDATE);

	hw_manager_uart_send_char(x_msb);
	hw_manager_uart_send_char(x_lsb);
	hw_manager_uart_send_char(y_msb);
	hw_manager_uart_send_char(y_lsb);
	hw_manager_uart_send_char(additional_byte_1);
	hw_manager_uart_send_char(additional_byte_2);

	hw_manager_uart_send_char(HEADER_TERMINATOR);
}

/** @} end of Player */
