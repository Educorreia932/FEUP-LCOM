#include "game_manager.h"
#include "hw_manager.h"

static GameManager_t* gm;

GameManager_t* get_game_manager() {
	return gm;
}

/** 
 * @param player_number Number of the player that's playing 
 */
GameManager_t * new_testing_game_manager(enum PlayerNumber player_number) {
	GameManager_t *game_manager = (GameManager_t*) malloc(sizeof(GameManager_t));
	
	if (game_manager == NULL) {
		printf("new_testing_game_manager: Failed to allocate memory for GameManager object\n");
		return NULL;
	}

	game_manager->kbd_ev = new_kbd_input_events();
	
	if (game_manager->kbd_ev == NULL) {
		printf("new_testing_game_manager: Failed to create the Keyboard Input Events object\n");
		return NULL;
	}

	game_manager->mouse_ev = new_mouse_input_events();

	if (game_manager->mouse_ev == NULL) {
		printf("new_testing_game_manager: Failed to create the Mouse Input Events object\n");
		return NULL;
	}

	game_manager->cursor = new_cursor(game_manager->mouse_ev, "/home/lcom/labs/proj/assets/wrench_cursor.bmp");
	
	if (game_manager->cursor == NULL) {
		printf("new_testing_game_manager: Failed to create the Cursor object\n");
		return NULL;
	}

	game_manager->player_number = player_number;

	if (game_manager->player_number == SINGLEPLAYER) {
		game_manager->s_board = NULL;
		game_manager->level = prototype_level(true);
		
		if (game_manager->level == NULL) {
			printf("new_testing_game_manager: Failed to create the Level object\n");
			return NULL;
		}
	}
	else if (game_manager->player_number == PLAYER_1) {
		game_manager->s_board = NULL;
		game_manager->level = prototype_level(false);
		
		if (game_manager->level == NULL) {
			printf("new_testing_game_manager: Failed to create the Level object\n");
			return NULL;
		}
	}
	else if (game_manager->player_number == PLAYER_2) {
		game_manager->level = NULL;
		game_manager->s_board = new_switchboard(game_manager->cursor);
		
		if (game_manager->s_board == NULL) {
			printf("new_testing_game_manager: Failed to create the Switchboard object\n");
			return NULL;
		}
	}

	return game_manager;
}

void free_game_manager(GameManager_t *game_manager) {
	if (game_manager == NULL) {
        printf("free_game_manager: Cannot free a NULL pointer\n");
        return;
    }

	if (game_manager->level != NULL)
	
		free_level(game_manager->level);
	if (game_manager->s_board != NULL)
		free_switchboard(game_manager->s_board);
	
	free_kbd_input_events(game_manager->kbd_ev);
	free_mouse_input_events(game_manager->mouse_ev);
	free(game_manager);
}

/** 
 * @note Update is executed once every frame
 */
void update() {
	update_cursor(gm->cursor);
	if (gm->player_number & SINGLEPLAYER)
  		update_level(gm->level, gm->kbd_ev, gm->mouse_ev);
	else if (gm->player_number & PLAYER_1)
		update_level(gm->level, gm->kbd_ev, gm->mouse_ev);
	else if (gm->player_number & PLAYER_2)
		update_switchboard(gm->s_board);	
}

void render() {
	if (gm->player_number & SINGLEPLAYER)
  		render_level(gm->level);
	else if (gm->player_number & PLAYER_1)
		render_level(gm->level);
	else if (gm->player_number & PLAYER_2)
		render_switchboard(gm->s_board);	

	render_cursor(gm->cursor);
}

/** 
 * @brief Starts the game
 * @param player_number Number of the player that's playing (1 to control Watt, 2 to control the switchboard)
 * @returns 0 on success, 1 otherwise
 */
uint8_t start_game(enum PlayerNumber player_number) {  
	printf("start_game: Creating GameManager object\n");

	gm = new_testing_game_manager(player_number);

	if (gm == NULL) {
		printf("start_game: Failed to create GameManager object\n");
		return 1;
	}
	
	printf("start_game: Created Game Manager\n");

	uint32_t kbd_bit_mask;
	uint32_t timer0_bit_mask;
	uint32_t mouse_bit_mask;
	uint32_t rtc_bit_mask;
	
	if (hw_manager_subscribe_int(&timer0_bit_mask, &kbd_bit_mask, &mouse_bit_mask, &rtc_bit_mask))
		printf("start_game: Failed to enable interrupts\n");
  
  	printf("start_game: Subscribed to all interrupts\n");

	int r, ipc_status;
	message msg;

	/* GAME LOOP */
  	/* aka interrupt loop */
	bool is_frame = false;

	printf("start_game: Entering game loop\n");

	hw_manager_rtc_set_alarm(5);

	while (!get_key(gm->kbd_ev, KBD_ESC)) {
		if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
			printf("start_game: driver_receive failed with: %d", r);
			continue;
		}

		if (is_ipc_notify(ipc_status)) {
			switch (_ENDPOINT_P(msg.m_source)) {
				case HARDWARE: /* hardware interrupt notification */
					if (msg.m_notify.interrupts & mouse_bit_mask) {
						hw_manager_mouse_ih();
						hw_manager_mouse(gm->mouse_ev);
					}

					if (msg.m_notify.interrupts & kbd_bit_mask) {
						hw_manager_kbd_ih();
						hw_manager_kbd(gm->kbd_ev);
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
			reset_inputs(gm->kbd_ev, gm->mouse_ev);

			is_frame = false;
		}
	}
	
	printf("start_game: Game loop ended\n");

	free_game_manager(gm);
	hw_manager_unsubscribe_int();
	printf("start_game: Unsubscribed to all interrupts\n");

	return 0;
}
