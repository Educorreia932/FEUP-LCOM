#include "game_manager.h"
#include "hw_manager.h"


static GameManager_t* gm;

char* assets_rel_path = "home/lcom/labs/proj/assets/";

// Add move verification here
GameManager_t* get_game_manager() {
	return gm;
}

// We prefer calling a void void function instead of, ye know, *seg fault*
void gm_void_safety_function() {
	return;
}

void gm_update_level() {
	update_cursor();
	update_level(get_game_manager()->level);
}

void gm_update_arcade() {
	update_cursor();
	update_arcade_level(get_game_manager()->level);
}

void gm_update_switchboard() {
	update_cursor();
	update_switchboard(get_game_manager()->s_board);
}

void gm_render_level() {
	render_level(gm->level);
	render_cursor();
}

void gm_render_arcade() {
	render_level(gm->level);
	render_cursor();
}

void gm_render_switchboard() {
	render_switchboard(gm->s_board);
	render_cursor();
}

uint8_t gm_start_level() {

	if (gm->s_board != NULL) {
		free_switchboard(gm->s_board);
		gm->s_board = NULL;
	}
	
	gm->level = prototype_level(gm->gamemode & GM_UART);
	
	if (gm->level == NULL) {
		printf("gm_start_level: Failed to create the Level object\n");
		return 1;
	}

	return 0;
}

uint8_t gm_start_switchboard() {
	
	if (gm->level != NULL) {
		free_level(gm->level);
		gm->level = NULL;
	}
	
	gm->s_board = new_switchboard();
	
	if (gm->s_board == NULL) {
		printf("gm_start_switchboard: Failed to create the Switchboard object\n");
		return 1;
	}

	return 0;

}

uint8_t gm_start_arcade() {

	if (gm->s_board != NULL) {
		free_switchboard(gm->s_board);
		gm->s_board = NULL;
	}

	gm->level = new_arcade_level();
	
	if (gm->level == NULL) {
		printf("gm_start_arcade: Failed to create the Switchboard object\n");
		return 1;
	}

	return 0;
}

/** 
 * @param player_number Number of the player that's playing 
 */
void initialize_game_manager(GameModeEnum gamemode) {
	
	// Avoid overwriting the old one
	if (gm != NULL) {
		printf("initialize_game_manager: GameManager already exists\n");
		return;
	}

	gm = (GameManager_t*) calloc(1, sizeof(GameManager_t));
	if (gm == NULL) {
		printf("new_testing_game_manager: Failed to allocate memory for GameManager object\n");
		exit(42); // This would pretty much be as fatal as a fatal error could be
	}

	// Save the gamemode
	gm->gamemode = gamemode;

	// Select the starting "gamemode" accordingly
	if (gm->gamemode & GM_LEVEL) {
		gm_start_level();
	}
	else if (gm->gamemode & GM_SWITCHBOARD) {
		gm_start_switchboard();
	}
	else if (gm->gamemode & GM_ARCADE) {
		gm_start_arcade();
	}
	else
	{
		printf("Invalid gamemode\n");
		exit(43); // Another pretty much fatal error
	}
	

	// Put all the unused ones to a non-seg fault fucntion
	for (uint8_t i = 0; i < 16; ++i) {
		gm->update_function[i] = &gm_void_safety_function;
		gm->render_function[i] = &gm_void_safety_function;
	}

	// Set update and render functions
	gm->update_function[GM_LEVEL] = &gm_update_level;
	gm->update_function[GM_LEVEL_UART] = &gm_update_level;
	gm->update_function[GM_SWITCHBOARD] = &gm_update_switchboard;
	gm->update_function[GM_SWITCHBOARD_UART] = &gm_update_switchboard;
	gm->update_function[GM_ARCADE] = &gm_update_arcade;
	gm->update_function[GM_ARCADE_UART] = &gm_update_arcade;
	
	gm->render_function[GM_LEVEL] = &gm_render_level;
	gm->render_function[GM_LEVEL_UART] = &gm_render_level;
	gm->render_function[GM_SWITCHBOARD] = &gm_render_switchboard;
	gm->render_function[GM_SWITCHBOARD_UART] = &gm_render_switchboard;
	gm->render_function[GM_ARCADE] = &gm_render_arcade;
	gm->render_function[GM_ARCADE_UART] = &gm_render_arcade;

}

void free_game_manager() {

	if (gm == NULL) {
        printf("free_game_manager: Cannot free a NULL pointer\n");
        return;
    }

	if (gm->level != NULL)
		free_level(gm->level);
	if (gm->s_board != NULL)
		free_switchboard(gm->s_board);
	
	free(gm);

}

/** 
 * @note Called once every frame
 */
void update() {
	gm->update_function[gm->gamemode]();
}

void render() {
	gm->render_function[gm->gamemode]();
}

/** 
 * @brief Starts the game
 * @returns 0 on success, 1 otherwise
 */
uint8_t start_game(GameModeEnum gamemode) {  

	printf("start_game: Started the game\n");

	// Initialize rand seed
	// time_t t;
	// srand((unsigned) time(&t));
	srand(time(NULL));

	// Enter video mode
	if (hw_manager_enter_video_mode())
		return 1;

	// If not initialized, guess what: Seg fault!
	initialize_kbd_input_events();
	initialize_mouse_input_events();
	initialize_cursor();

	initialize_game_manager(gamemode);

	if (gm == NULL) {
		printf("start_game: Failed to create GameManager object\n");
		return 1;
	}

	uint32_t kbd_bit_mask;
	uint32_t timer0_bit_mask;
	uint32_t mouse_bit_mask;
	uint32_t rtc_bit_mask;
	
	if (hw_manager_subscribe_int(&timer0_bit_mask, &kbd_bit_mask, &mouse_bit_mask, &rtc_bit_mask))
		printf("start_game: Failed to enable interrupts\n");

	int r, ipc_status;
	message msg;

	/* GAME LOOP */
  	/* aka interrupt loop */
	bool is_frame = false;

	printf("start_game: Entering game loop\n");

	// hw_manager_rtc_set_alarm(5);

	while (!get_key(KBD_ESC)) {
		if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
			printf("start_game: driver_receive failed with: %d", r);
			continue;
		}

		if (is_ipc_notify(ipc_status)) {
			switch (_ENDPOINT_P(msg.m_source)) {
				case HARDWARE: /* hardware interrupt notification */
					if (msg.m_notify.interrupts & mouse_bit_mask) {
						hw_manager_mouse_ih();
						hw_manager_mouse();
					}

					if (msg.m_notify.interrupts & kbd_bit_mask) {
						hw_manager_kbd_ih();
						hw_manager_kbd();
					}
		
					if (msg.m_notify.interrupts & timer0_bit_mask) {
						hw_manager_timer0_ih();
						is_frame = hw_manager_is_frame();
					}

					if (msg.m_notify.interrupts & rtc_bit_mask) {
						printf("RTC\n");
						hw_manager_rtc_ih();
					}

					break;
				default:
					break; /* no other notifications expected: do nothing */     
			}
		}

		// We only do the heavy stuff here, out of the "critical path"
		if (is_frame) {

			update();
			render();

			hw_manager_switch_double_buffer();

			//reset rtc
			reset_kbd_input_state();
			reset_mouse_input_state();

			is_frame = false;
		}
	}
	
	printf("start_game: Game loop ended\n");

	free_game_manager();
	free_kbd_input_events();
	free_mouse_input_events();
	free_cursor();
	hw_manager_unsubscribe_int();
	hw_manager_exit_video_mode();

	printf("Thanks for playing!\n");

	return 0;
}
