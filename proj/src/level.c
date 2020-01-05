#include "level.h"
#include "player.h"
#include "game_manager.h"
#include "hw_manager.h"

#define ARCADE_VERSUS_SPAWN_BUFFER 30
#define ARCADE_VERSUS_DEATH_BUFFER 30
#define ARCADE_VERSUS_BOTH_DEAD_TIME_DEFAULT 0xFFFFFFFF

Level_t* new_arcade_level(bool is_single_player) {
	Level_t* level = (Level_t*) calloc(1, sizeof(Level_t));

	// Background
	level->background = new_sprite(0, 0, 1, "background.bmp");
	if (level->background == NULL) {
		printf("new_arcade_level: Failed to create the background Sprite\n");
		free(level);
		return NULL;
	}

	// Player
	level->player = new_player(false, true, UNLOCKED_GRAVITY);
	if (level->player == NULL) {
		printf("new_arcade_level: Failed to create the Player\n");
		free_sprite(level->background);
		free(level);
		return NULL;
	}

	// Spikes
	level->spikes = new_arcade_spikes();
	if (level->spikes == NULL) {
		printf("new_arcade_level: Failed to create the Spikes\n");
		free_sprite(level->background);
		free_player(level->player);
		free(level);
		return NULL;
	}

	// Lasers
	level->lasers = new_arcade_lasers();
	if (level->lasers == NULL) {
		printf("new_arcade_level: Failed to create the Lasers\n");
		free_sprite(level->background);
		free_spikes(level->spikes);
		free_player(level->player);
		free(level);
		return NULL;
	}

	// Platforms
	level->platforms = new_arcade_platforms();
	if (level->platforms == NULL) {
		printf("new_arcade_level: Failed to create the Platforms\n");
		free_sprite(level->background);
		free_player(level->player);
		free_spikes(level->spikes);
		free_lasers(level->lasers);
		free(level);
		return NULL;
	}

	// Multiplayer
	if (!is_single_player) {
		level->score_1 = new_score(372, 40, 0, 3, COLOR_RED);
		level->score_2 = new_score(532, 40, 0, 3, COLOR_BLUE);
		level->player_two = new_player_two();
	}
	else {
		level->score_1 = new_score(820, 40, 0, 3, COLOR_NO_MULTIPLY);
		level->score_2 = new_score(820, 100, 0, 3, 0x8c51);
	}

	level->frames_since_start = 0;

	return level;
}

Level_t* prototype_level(bool is_single_player) {
	Level_t* level = (Level_t*) calloc(1, sizeof(Level_t));

	// Background
	level->background = new_sprite(0, 0, 1, "background.bmp");
	if (level->background == NULL) {
		printf("prototype_level: Failed to create the background Sprite\n");
		free(level);
		return NULL;
	}

	// Winning Screen
	level->win_screen = new_sprite(0, 0, 1, "win_screen.bmp");
	if (level->win_screen == NULL) {
		printf("prototype_level: Failed to create the winning screen Sprite\n");
		free_sprite(level->win_screen);
		free(level);
		return NULL;
	}

	// Player
	level->player = new_player(is_single_player, false, UNLOCKED_JUMP | UNLOCKED_SPEED);
	if (level->player == NULL) {
		printf("prototype_level: Failed to create the Player\n");
		free_sprite(level->background);
		free(level);
		return NULL;
	}

	// Platforms
	level->platforms = prototype_platforms();
	if (level->platforms == NULL) {
		printf("prototype_level: Failed to create the Platforms\n");
		free_sprite(level->background);
		free_player(level->player);
		free(level);
		return NULL;
	}

	// Lasers
	level->lasers = prototype_lasers();
	if (level->lasers == NULL) {
		printf("prototype_level: Failed to create the Lasers\n");
		free_sprite(level->background);
		free_player(level->player);
		free_platforms(level->platforms);
		free(level);

		return NULL;
	}

	// Spikes
	level->spikes = prototype_spikes();

	if (level->spikes == NULL) {
		printf("prototype_level: Failed to create the Spikes\n");
		free_sprite(level->background);
		free_player(level->player);
		free_platforms(level->platforms);
		free_lasers(level->lasers);
		free(level);
		
		return NULL;
	}

	//Powerups
	level->pu[0] = new_power_up("powerups/laser_icon.bmp", vec2d(48, 84), player_unlock_lasers);
	if (level->pu[0]== NULL) {
		printf("prototype_level: Failed to create laser powerup\n");
		free_sprite(level->background);
		free_player(level->player);
		free_platforms(level->platforms);
		free_lasers(level->lasers);
		free_spikes(level->spikes);

		return NULL;
	}

	level->pu[1] = new_power_up("powerups/anti_gravity_icon.bmp", vec2d(940, 680), player_unlock_gravity);

	if (level->pu[1] == NULL) {
		printf("prototype_level: Failed to create anti-gravity powerup\n");
		free_sprite(level->background);
		free_player(level->player);
		free_platforms(level->platforms);
		free_lasers(level->lasers);
		free_spikes(level->spikes);

		return NULL;
	}

	level->pu[2] = new_power_up("powerups/exit_icon.bmp", vec2d(920, 23), player_win);

	if (level->pu[2] == NULL) {
		printf("prototype_level: Failed to create exit\n");
		free_sprite(level->background);
		free_player(level->player);
		free_platforms(level->platforms);
		free_lasers(level->lasers);
		free_spikes(level->spikes);

		return NULL;
	}

	level->score_1 = NULL;
	level->score_2 = NULL;

	level->frames_since_start = 0;

	return level;
}

void free_level(Level_t *level) {
	if (level == NULL) {
        printf("free_level: Cannot free a NULL pointer\n");
        return;
    }

	free_sprite(level->background);
	free_platforms(level->platforms);
	free_player(level->player);
	free_lasers(level->lasers);
	free_spikes(level->spikes);

	for (uint8_t i = 0; i < MAX_POWERUPS; ++i) {
		if (level->pu[i] != NULL) {
			free_power_up(level->pu[i]);
		}
	}

	if (level->player_two != NULL)
		free_player_two(level->player_two);
	if (level->score_1 != NULL)
		free_score(level->score_1);
	if (level->score_2 != NULL)
		free_score(level->score_2);

	free(level);
}

// Actual level stuff

void update_level(Level_t* level) {
	++level->frames_since_start;
  	update_player(level->player, level->platforms, level->lasers, level->spikes, level->pu);
}

void update_arcade_level(Level_t* level) {
	++level->frames_since_start;
	arcade_update_laser_values(level->lasers, level->frames_since_start);
	arcade_move_lasers(level->lasers);
	if (arcade_spawn_next_laser(level->lasers)) {
		arcade_add_laser(level->lasers, arcade_generate_laser_height());
		arcade_lasers_set_correct_delay(level->lasers);
	}
	update_player(level->player, level->platforms, level->lasers, level->spikes, level->pu);
}

void update_arcade_versus(Level_t* level, uint8_t bytes[]) {
	++level->frames_since_start;
	arcade_move_lasers(level->lasers);
	if (level->laser_master) {
		if (arcade_spawn_next_laser(level->lasers)) {
			uint16_t height = arcade_generate_laser_height();
			arcade_add_laser(level->lasers, height);
			arcade_lasers_set_correct_delay(level->lasers);

			uint8_t height_msb, height_lsb;
			if (!(util_get_MSB(height, &height_msb) || util_get_LSB(height, &height_lsb))) {
				hw_manager_uart_send_char(HEADER_ARCADE_LASER);
				hw_manager_uart_send_char(height_msb);
				hw_manager_uart_send_char(height_lsb);
				hw_manager_uart_send_char(HEADER_TERMINATOR);
			}
		}
	}
	update_player(level->player, level->platforms, level->lasers, level->spikes, level->pu);
	if (bytes != NULL)
		update_player_two(level->player_two, bytes);

}

void reset_arcade_mode(Level_t* level) 
{
	level->frames_since_start = 0;

	arcade_reset_lasers(level->lasers);

}

void render_level(Level_t *level) {
	draw_sprite_floats(level->background, 0, 0, COLOR_NO_MULTIPLY, SPRITE_NORMAL);
	render_player_background(level->player);
	render_spikes(level->spikes);
	render_platforms(level->platforms);
	render_player_foreground(level->player);
	render_lasers(level->lasers);
	
	for (uint8_t i = 0; i < MAX_POWERUPS; ++i) {
		if (level->pu[i] != NULL)
			render_power_up(level->pu[i]);
	}

	render_player_ui(level->player);
}

void render_arcade_single(Level_t* level) {
	draw_sprite_floats(level->background, 0, 0, COLOR_NO_MULTIPLY, SPRITE_NORMAL);
	render_platforms(level->platforms);
	
	render_player_background(level->player);
	render_player_foreground(level->player);

	render_lasers(level->lasers);
	render_player_ui(level->player);

	render_score(level->score_1);
	render_score(level->score_2);
}

void render_arcade_versus(Level_t* level) {
	draw_sprite_floats(level->background, 0, 0, COLOR_NO_MULTIPLY, SPRITE_NORMAL);
	render_platforms(level->platforms);

	render_player_two_background(level->player_two);
	render_player_two_foreground(level->player_two);
	
	render_player_background(level->player);
	render_player_foreground(level->player);

	render_lasers(level->lasers);
	render_player_ui(level->player);

	render_score(level->score_1);
	render_score(level->score_2);
}

