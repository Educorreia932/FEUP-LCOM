#include "game_manager.h"
#include "hw_manager.h"
#include "mouse_cursor.h"

#define GM_ESC_COUNTDOWN_WINDOW 20 /** @note In frames */

static GameManager_t* gm;

char *assets_rel_path = "home/lcom/labs/proj/assets/";

// Add move verification here
GameManager_t* get_game_manager() {
	return gm;
}

// We prefer calling a void void function instead of, ye know, *seg fault*
static void gm_void_safety_function() {
	return;
}

/** 
 * @brief Erases everything until the queue is either empty or it finds a message terminator
 * This means an error will never "contaminate" the next messages
 */
static void gm_uart_erase_message() {
	while (hw_manager_uart_front() != HEADER_TERMINATOR
			&& !hw_manager_uart_is_empty()) {
		
		hw_manager_uart_pop();

	}

	if (!hw_manager_uart_is_empty())
		hw_manager_uart_pop();
}

// static void gm_uart_clear_everything() {
// 	while (!hw_manager_uart_is_empty())
// 		hw_manager_uart_pop();
// }

static void gm_update_campaign_single() {
	update_cursor();
	update_level(get_game_manager()->level);
}

static void gm_update_campaign_coop() {
	if (!hw_manager_uart_is_empty()) {
		bool keep_going = true;
		gm->uart_last_received = 0;
		while (!hw_manager_uart_is_empty() && keep_going) {
			if (!gm->uart_synced) {
				switch (hw_manager_uart_front()) {
					case HEADER_REQUEST_POWERS:
						if (hw_manager_uart_size() < HEADER_REQUEST_POWERS_SIZE) {
							keep_going = false;
							break;
						}
						hw_manager_uart_send_char(HEADER_RESPONSE_POWERS);
						hw_manager_uart_send_char((uint8_t) player_get_default_powers());
						hw_manager_uart_send_char(HEADER_TERMINATOR);
						gm->uart_synced = true;
						gm->has_partner = true;
						break;
				}
			}

			else {
				switch (hw_manager_uart_front()) {
					case HEADER_SPEED_MULT:
						if (hw_manager_uart_size() < HEADER_SPEED_MULT_SIZE) {
							keep_going = false;
							break;
						}
						hw_manager_uart_pop();
						player_set_speed(hw_manager_uart_front());
						break;
					case HEADER_JUMP_MULT:
						if (hw_manager_uart_size() < HEADER_JUMP_MULT_SIZE) {
							keep_going = false;
							break;
						}
						hw_manager_uart_pop();
						player_set_jump(hw_manager_uart_front());
						break;
					case HEADER_LASER:
						if (hw_manager_uart_size() < HEADER_LASER_SIZE) {
							keep_going = false;
							break;
						}
						hw_manager_uart_pop();
						lasers_set_link_id(gm->level->lasers, hw_manager_uart_front());
						break;
					case HEADER_GRAVITY:
						if (hw_manager_uart_size() < HEADER_GRAVITY_SIZE) {
							keep_going = false;
							break;
						}
						hw_manager_uart_pop();
						switch (hw_manager_uart_front()) {
							case UART_GRAVITY_NORMAL:
								player_set_gravity_normal();
								break;
							case UART_GRAVITY_REVERSED:
								player_set_gravity_reversed();
								break;
							default:
								printf("Coop campaign level: Unexpected gravity value\n");
								break;
						}
						break;
				}
			}
			if (keep_going)
				gm_uart_erase_message();
		}	
	}
	else {
		++gm->uart_last_received;
		if (gm->uart_synced && (gm->uart_last_received > UART_DC_TIME)) {
			if (gm->has_partner) {
				// Other player disconnected or some REALLY bad desync happened
				gm->uart_synced = false;
				gm->gamemode = GM_MAIN_MENU;
				gm_start_main_menu();
				return;
			}
		}
	}

	update_cursor();
	update_level(get_game_manager()->level);
}

static void gm_update_arcade() {
	update_cursor();
	update_arcade_level(get_game_manager()->level);
}

static void gm_update_arcade_versus() {
	uint8_t bytes[6];

	if (!hw_manager_uart_is_empty()) {
		gm->uart_last_received = 0;
		bool keep_going = true;

		while (!hw_manager_uart_is_empty() && keep_going) {
			if (!gm->uart_synced) {
				switch (hw_manager_uart_front()) {
					case HEADER_AVAILABLE_ARCADE:
						if (hw_manager_uart_size() < HEADER_AVAILABLE_ARCADE_SIZE) {
							keep_going = false;
							break;
						}
						gm->uart_synced = true;
						gm->has_partner = true;
						hw_manager_uart_send_char(HEADER_ARCADE_READY);
						hw_manager_uart_send_char(HEADER_TERMINATOR);
						break;
					case HEADER_ARCADE_READY:
						if (hw_manager_uart_size() < HEADER_ARCADE_READY_SIZE) {
							keep_going = false;
							break;
						}
						gm->uart_synced = true;
						gm->has_partner = true;
						break;
				}
			}

			else {
				switch (hw_manager_uart_front()) {
					case HEADER_PLAYER_TWO_UPDATE:
						if (hw_manager_uart_size() < HEADER_PLAYER_TWO_UPDATE_SIZE) {
							keep_going = false;
							break;
						}
						hw_manager_uart_pop();
						
						for (size_t i = 0; i < 6; i++)
							bytes[i] = hw_manager_uart_pop();

						break;
				}
			}
			
			if (keep_going)
				gm_uart_erase_message();
		}
	}
	
	else {
		++gm->uart_last_received;
		
		if (gm->uart_synced && (gm->uart_last_received > UART_DC_TIME)) {
			if (gm->has_partner) {
				// Other player disconnected or some REALLY bad desync happened
				gm->uart_synced = false;
				gm->gamemode = GM_MAIN_MENU;
				gm_start_main_menu();
				return;
			}
		}
	}

	update_cursor();

	if (gm->uart_synced)
		update_arcade_versus(get_game_manager()->level, bytes);
}

static void gm_update_switchboard() {
	if (!hw_manager_uart_is_empty()) {
		gm->uart_last_received = 0;
		bool keep_going = true;
		while (!hw_manager_uart_is_empty() && keep_going) {
			if (!gm->uart_synced) {
				switch (hw_manager_uart_front()) {
					case HEADER_AVAILABLE_LEVEL:
						if (hw_manager_uart_size() < HEADER_AVAILABLE_LEVEL_SIZE) {
							keep_going = false;
							break;
						}
						hw_manager_uart_send_char(HEADER_REQUEST_POWERS);
						hw_manager_uart_send_char(HEADER_TERMINATOR);
						break;
					case HEADER_RESPONSE_POWERS:
						if (hw_manager_uart_size() < HEADER_RESPONSE_POWERS_SIZE) {
							keep_going = false;
							break;
						}
						hw_manager_uart_pop();
						switchboard_set_default_powers(gm->s_board, hw_manager_uart_front());
						gm->uart_synced = true;
						gm->has_partner = true;
						break;
				}
			}

			else {
				switch (hw_manager_uart_front()) {
					case HEADER_PLAYER_RESPAWN:
						if (hw_manager_uart_size() < HEADER_PLAYER_RESPAWN_SIZE) {
							keep_going = false;
							break;
						}
						switchboard_player_respawn(gm->s_board);
						break;
					case HEADER_PLAYER_UPDATE:
						if (hw_manager_uart_size() < HEADER_PLAYER_UPDATE_SIZE) {
							keep_going = false;
							break;
						}
						hw_manager_uart_pop();
						switchboard_unlock_powers(gm->s_board, hw_manager_uart_front());
						break;
					case HEADER_RESPONSE_POWERS:
						if (hw_manager_uart_size() < HEADER_RESPONSE_POWERS_SIZE) {
							keep_going = false;
							break;
						}
						hw_manager_uart_pop();
						switchboard_set_default_powers(gm->s_board, hw_manager_uart_front());
						break;
				}
			}
			if (keep_going)
				gm_uart_erase_message();
		}
	}

	else {
		++gm->uart_last_received;
		if (gm->uart_synced && (gm->uart_last_received > UART_DC_TIME)) {
			if (gm->has_partner) {
				// Other player disconnected or some REALLY bad desync happened
				gm->uart_synced = false;
				gm->gamemode = GM_MAIN_MENU;
				gm_start_main_menu();
				return;
			}
		}
	}

	update_cursor();

	if (gm->uart_synced)
		update_switchboard(get_game_manager()->s_board);
}

static void gm_update_main_menu() {
	update_cursor();
	update_main_menu(gm->main_menu);
	gm_uart_erase_message();
}

static void gm_render_campaign_single() {
	render_level(gm->level);
	render_cursor();
}

static void gm_render_campaign_coop() {
	if (gm->uart_synced)
		render_level(gm->level);
	else
		draw_sprite_floats(gm->connecting_sprite, 0, 0, COLOR_NO_MULTIPLY, SPRITE_NORMAL);
	render_cursor();
}

static void gm_render_arcade() {
	render_level(gm->level);

	render_cursor();
}

static void gm_render_arcade_versus() {
	if (gm->uart_synced) {
		render_arcade_versus(gm->level);
	}
	else
		draw_sprite_floats(gm->connecting_sprite, 0, 0, COLOR_NO_MULTIPLY, SPRITE_NORMAL);

	render_cursor();
}

static void gm_render_switchboard() {
	if (gm->uart_synced)
		render_switchboard(gm->s_board);

	else 
		draw_sprite_floats(gm->connecting_sprite, 0, 0, COLOR_NO_MULTIPLY, SPRITE_NORMAL);
	
	render_cursor();
}

static void gm_render_main_menu() {
	render_main_menu(gm->main_menu);
	render_cursor();
}

void gm_start_level() {

	if (gm->s_board != NULL) {
		free_switchboard(gm->s_board);
		gm->s_board = NULL;
	}

	if (gm->main_menu != NULL) {
		free_main_menu(gm->main_menu);
		gm->main_menu = NULL;
	}

	gm->uart_synced = false;
	gm->has_partner = false;

	gm->level = prototype_level(!(gm->gamemode & GM_UART));

	if (gm->level == NULL) {
		printf("gm_start_level: Failed to create the Switchboard object\n");
		exit_game();
		exit(42);
	}

	if (gm->gamemode & GM_UART) {
		hw_manager_uart_send_char(HEADER_AVAILABLE_LEVEL);
		hw_manager_uart_send_char(HEADER_TERMINATOR);
	}
}

void gm_start_switchboard() {
	if (gm->level != NULL) {
		free_level(gm->level);
		gm->level = NULL;
	}

	if (gm->main_menu != NULL) {
		free_main_menu(gm->main_menu);
		gm->main_menu = NULL;
	}
	
	gm->uart_synced = false;
	gm->has_partner = false;
	gm->s_board = new_switchboard();
	
	if (gm->s_board == NULL) {
		printf("gm_start_switchboard: Failed to create the Switchboard object\n");
		exit_game();
		exit(42);
	}

	if (gm->gamemode & GM_UART) {
		hw_manager_uart_send_char(HEADER_REQUEST_POWERS);
		hw_manager_uart_send_char(HEADER_TERMINATOR);
	}
}

void gm_start_arcade() {
	srand(12);
	printf("%u\n", rand());

	if (gm->s_board != NULL) {
		free_switchboard(gm->s_board);
		gm->s_board = NULL;
	}

	if (gm->main_menu != NULL) {
		free_main_menu(gm->main_menu);
		gm->main_menu = NULL;
	}

	gm->uart_synced = false;
	gm->has_partner = false;
	gm->level = new_arcade_level(!(gm->gamemode & GM_UART));
	
	if (gm->level == NULL) {
		printf("gm_start_arcade: Failed to create the Switchboard object\n");
		exit_game();
		exit(42);
	}

	if (gm->gamemode & GM_UART) {
		hw_manager_uart_send_char(HEADER_AVAILABLE_ARCADE);
		hw_manager_uart_send_char(HEADER_TERMINATOR);
	}
}

void gm_start_main_menu() {
	if (gm->level != NULL) {
		free_level(gm->level);
		gm->level = NULL;
	}

	if (gm->s_board != NULL) {
		free_switchboard(gm->s_board);
		gm->s_board = NULL;
	}

	gm->uart_synced = false;
	gm->has_partner = false;
	gm->main_menu = new_main_menu();

	if (gm->main_menu == NULL) {
		printf("gm_start_main_menu: Faild to create the MainMenu object\n");
		exit_game();
		exit(42);
	}

}

/** 
 * @param gamemode Indicates the gamemode to start the game on
 */
static void initialize_game_manager() {
	// Avoid overwriting the old one
	if (gm != NULL) {
		printf("initialize_game_manager: GameManager already exists\n");
		return;
	}

	gm = (GameManager_t*) calloc(1, sizeof(GameManager_t));
	if (gm == NULL) {
		printf("initialize_game_manager: Failed to allocate memory for GameManager object\n");
		exit(42); // This would pretty much be as fatal as a fatal error could be
	}

	gm->connecting_sprite = new_sprite(0, 0, 1, "main_menu/awaiting_connection_screen.bmp");
	if (gm->connecting_sprite == NULL) {
		printf("initialize_game_manager: Failed to create the awaiting connection Sprite\n");
		free(gm);
		gm = NULL;
		exit(42);
	}

	gm->esc_counter = GM_ESC_COUNTDOWN_WINDOW + 1;
	gm->game_ongoing = true;
	gm->uart_synced = false;
	gm->has_partner = false;

	// Save the gamemode
	gm->gamemode = GM_MAIN_MENU;
	
	// Put all the unused ones to a non-seg fault fucntion
	for (uint8_t i = 0; i < 31; ++i) {
		gm->update_function[i] = &gm_void_safety_function;
		gm->render_function[i] = &gm_void_safety_function;
	}

	// Set update and render functions
	gm->update_function[GM_LEVEL] = &gm_update_campaign_single;
	gm->update_function[GM_LEVEL_UART] = &gm_update_campaign_coop;
	gm->update_function[GM_SWITCHBOARD] = &gm_update_switchboard;
	gm->update_function[GM_SWITCHBOARD_UART] = &gm_update_switchboard;
	gm->update_function[GM_ARCADE] = &gm_update_arcade;
	gm->update_function[GM_ARCADE_UART] = &gm_update_arcade_versus;
	gm->update_function[GM_MAIN_MENU] = &gm_update_main_menu;
	gm->update_function[GM_MAIN_MENU | GM_UART] = &gm_update_main_menu;

	gm->render_function[GM_LEVEL] = &gm_render_campaign_single;
	gm->render_function[GM_LEVEL_UART] = &gm_render_campaign_coop;
	gm->render_function[GM_SWITCHBOARD] = &gm_render_switchboard;
	gm->render_function[GM_SWITCHBOARD_UART] = &gm_render_switchboard;
	gm->render_function[GM_ARCADE] = &gm_render_arcade;
	gm->render_function[GM_ARCADE_UART] = &gm_render_arcade_versus;
	gm->render_function[GM_MAIN_MENU] = &gm_render_main_menu;
	gm->render_function[GM_MAIN_MENU | GM_UART] = &gm_render_main_menu;
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
	if (gm->main_menu != NULL)
		free_main_menu(gm->main_menu);
	if (gm->connecting_sprite != NULL)
		free_sprite(gm->connecting_sprite);
	
	free(gm);

}

/**
 * @note Called once every frame
 */
void update() {
	// Takes care of exiting a gamemode and exiting the game
	if (get_key_down(KBD_ESC)) {
		if (gm->esc_counter <= GM_ESC_COUNTDOWN_WINDOW) {
			// Go back a Menu
			if (gm->gamemode & GM_MAIN_MENU)
				gm->game_ongoing = false;
			else {
				gm->gamemode = GM_MAIN_MENU;
				gm_start_main_menu();
			}
		}
		else
			gm->esc_counter = 0;
	}
	else
		++gm->esc_counter;
	
	if (gm->gamemode & GM_UART) {
		// Used to make sure the UART is still synced
		++gm->uart_last_sent;
		if (gm->uart_last_sent > UART_CONNECTION_WARNING_TIME) {
			hw_manager_uart_send_char(HEADER_SYNCED);
			hw_manager_uart_send_char(HEADER_TERMINATOR);
		}
	}
	
	gm->update_function[gm->gamemode]();

}

void render() {
	gm->render_function[gm->gamemode]();
}

// Used for shutdowns (including panicky ones)
void exit_game() {
	free_game_manager();
	free_kbd_input_events();
	free_mouse_input_events();
	free_cursor();
	hw_manager_unsubscribe_int();
	hw_manager_exit_video_mode();
}

uint8_t start_game(bool override_path, char *assets_path) {  
	printf("start_game: Started the game\n");

	if (override_path) {
		free(assets_rel_path);
		assets_rel_path = (char*) malloc(sizeof(assets_path) * strlen(assets_path));
		strncpy(assets_rel_path, assets_path, 255);
	}

	// Enter video mode
	if (hw_manager_enter_video_mode())
		return 1;

	// If not initialized, guess what: Seg fault!
	initialize_kbd_input_events();
	initialize_mouse_input_events();
	initialize_cursor();

	initialize_game_manager();

	if (gm == NULL) {
		printf("start_game: Failed to create GameManager object\n");
		return 1;
	}

	uint32_t kbd_bit_mask;
	uint32_t timer0_bit_mask;
	uint32_t mouse_bit_mask;
	uint32_t rtc_bit_mask;
	uint32_t uart_bit_mask;
	
	if (hw_manager_subscribe_int(&timer0_bit_mask, &kbd_bit_mask, &mouse_bit_mask, &rtc_bit_mask, &uart_bit_mask)) {
		printf("start_game: Failed to enable interrupts\n");
		exit_game();
		exit(42);
	}

	gm_start_main_menu();

	int r, ipc_status;
	message msg;

	/* GAME LOOP */
  	/* aka interrupt loop */
	bool is_frame = false;

	printf("start_game: Entering game loop\n");

	while (gm->game_ongoing) {
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
						hw_manager_rtc_ih();
					}

					if (msg.m_notify.interrupts & uart_bit_mask) {
						hw_manager_uart_ih();
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

	exit_game();

	printf("Thanks for playing!\n");

	return 0;
}
