#include "level.h"

Level_t* new_level(const char* background_file_name) {
  Level_t *level = (Level_t*) malloc(sizeof(Level_t));
  if (level == NULL) {
    printf("new_level: Failed to allocate memory for Level object\n");
    return NULL;
  }

  level->background = new_sprite(0, 0, 1, background_file_name);
  if (level->background == NULL) {
    printf("new_level: Failed to load background sprite\n");
    return NULL;
  }

  level->platforms = new_platforms();
  if (level->platforms == NULL) {
        printf("new_level: Failed to allocate memory for Platforms object\n");
    return NULL;
  }

  level->player = new_player();
  if (level->player == NULL) {
    printf("new_level: Failed to allocate memory for Player object\n");
    return NULL;
  }

  return level;
}

Level_t* new_testing_level() {
	Level_t *level = (Level_t*) malloc(sizeof(Level_t));

	if (level == NULL) {
		printf("new_testing_level: Failed to allocate memory for Level object\n");
		return NULL;
	}
	
	printf("new_testing_level: Background\n");
	level->background = new_sprite(0, 0, 1, "/home/lcom/labs/proj/assets/background.bmp");
	if (level->background == NULL) {
		printf("new_testing_level: Failed to load background sprite\n");
		return NULL;
	}

	printf("Creating Player\n");
	level->player = new_testing_player();
	if (level->player == NULL) {
		printf("new_testing_level: Failed to create new testing player\n");
		return NULL;
	}

	printf("new_testing_level: Creating platforms\n");
	level->platforms = new_testing_platforms();
	if (level->platforms == NULL) {
		printf("new_testing_level: Failed to create new testing platforms\n");
		return NULL;
	}

	printf("new_testing_level: Creating lasers\n");
	
	level->lasers = new_testing_lasers();

	if (level->lasers == NULL) {
		printf("new_testing_level: Failed to create new testing lasers\n");
    	return NULL;
	} 


  printf("new_testing_level: Created full Level\n");
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
	free(level);
}

// Actual level stuff

void update_level(Level_t* level, KbdInputEvents_t* kbd_ev, MouseInputEvents_t* mouse_ev){
  	player_movement(level->player, level->platforms, level->lasers, kbd_ev, mouse_ev);
}

void render_level(Level_t *level) {
	draw_sprite_floats(level->background, 0, 0, COLOR_NO_MULTIPLY, false);
	render_platforms(level->platforms);
	render_player(level->player);
	render_lasers(level->lasers);
}
