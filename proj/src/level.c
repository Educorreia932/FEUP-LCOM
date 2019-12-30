#include "level.h"

Level_t* new_level() {
	Level_t *level = (Level_t*) malloc(sizeof(Level_t));
	if (level == NULL) {
		printf("new_level: Failed to allocate memory for Level object\n");
		return NULL;
	}

	level->background = new_sprite(0, 0, 1, "background.bmp");
	if (level->background == NULL) {
		printf("new_level: Failed to load background sprite\n");
		return NULL;
	}

	level->platforms = new_platforms();
	if (level->platforms == NULL) {
			printf("new_level: Failed to allocate memory for Platforms object\n");
		return NULL;
	}

	level->player = new_player(false, false);
	if (level->player == NULL) {
		printf("new_level: Failed to allocate memory for Player object\n");
		return NULL;
	}

	return level;
}

Level_t* new_arcade_level() {
	Level_t* level = (Level_t*) calloc(1, sizeof(Level_t));

	// Background
	level->background = new_sprite(0, 0, 1, "background.bmp");
	if (level->background == NULL) {
		printf("new_arcade_level: Failed to create the background Sprite\n");
		free(level);
		return NULL;
	}

	// Player
	level->player = new_player(false, true);
	if (level->player == NULL) {
		printf("new_arcade_level: Failed to create the Player\n");
		free_sprite(level->background);
		free(level);
		return NULL;
	}

	// Platforms
	level->platforms = new_arcade_platforms();
	if (level->platforms == NULL) {
		printf("new_arcade_level: Failed to create the Platforms\n");
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
		free_player(level->player);
		free_platforms(level->platforms);
		free(level);
		return NULL;
	}

	// Spikes
	level->spikes = new_arcade_spikes();
	if (level->spikes == NULL) {
		printf("new_arcade_level: Failed to create the Spikes\n");
		free_sprite(level->background);
		free_player(level->player);
		free_platforms(level->platforms);
		free_lasers(level->lasers);
		free(level);
		return NULL;
	}

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

	// Player
	level->player = new_player(is_single_player, false);
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
	level->pu[0] = new_power_up("powerups/laser_icon.bmp", rect(48, 84, 40, 40), placeholder);
	if (level->pu[0]== NULL) {
		printf("prototype_level: Failed to create laser powerup\n");
		free_sprite(level->background);
		free_player(level->player);
		free_platforms(level->platforms);
		free_lasers(level->lasers);
		free_spikes(level->spikes);

		return NULL;
	}

	level->pu[1] = new_power_up("powerups/anti_gravity_icon.bmp", rect(940, 704, 40, 40), placeholder);

	if (level->pu[1] == NULL) {
		printf("prototype_level: Failed to create anti-gravity powerup\n");
		free_sprite(level->background);
		free_player(level->player);
		free_platforms(level->platforms);
		free_lasers(level->lasers);
		free_spikes(level->spikes);

		return NULL;
	}

	level->pu[2] = new_power_up("powerups/exit_icon.bmp", rect(940, 24, 80, 80), placeholder);

	if (level->pu[2] == NULL) {
		printf("prototype_level: Failed to create exit\n");
		free_sprite(level->background);
		free_player(level->player);
		free_platforms(level->platforms);
		free_lasers(level->lasers);
		free_spikes(level->spikes);

		return NULL;
	}

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
	free(level);
}

// Actual level stuff

void update_level(Level_t* level) {
  	update_player(level->player, level->platforms, level->lasers, level->spikes, level->pu);
	animator_player(level->player);
}

void update_arcade_level(Level_t* level) {
	arcade_move_lasers(level->lasers);
	arcade_add_laser(level->lasers);
	update_player(level->player, level->platforms, level->lasers, level->spikes, level->pu);
	animator_player(level->player);
}

void render_level(Level_t *level) {
	draw_sprite_floats(level->background, 0, 0, COLOR_NO_MULTIPLY, SPRITE_NORMAL);
	render_player_background(level->player);
	render_spikes(level->spikes);
	render_platforms(level->platforms);
	render_player_foreground(level->player);
	render_lasers(level->lasers);
	render_player_ui(level->player);

	for (uint8_t i = 0; i < 3; ++i) {
		if (level->pu[i] != NULL)
			render_power_up(level->pu[i]);
	}

}
