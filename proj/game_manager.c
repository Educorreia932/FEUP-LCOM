#include "game_manager.h"
#include "hw_manager.h"
#include "level.h"

struct GameManager {
    Level_t *level;
    MouseInputEvents_t *mouse_input_events;
    KbdInputEvents_t *kbd_input_events;
};

GameManager_t * new_game_manager(const char* background_file_name) {
  GameManager_t *gm = (GameManager_t*) malloc(sizeof(GameManager_t));
  if (gm == NULL) {
    printf("new_game_manager: Failed to allocate memory for GameManager object\n");
    return NULL;
  }

  gm->level = new_level(background_file_name);
  if (gm->level == NULL) {
    printf("new_level_manager: Failed to allocate memory for the Level object\n");
    return NULL;
  }

  gm->kbd_input_events = new_kbd_input_events();
  if (gm->kbd_input_events == NULL) {
    printf("new_level_manager: Failed to allocate memory for the Keyboard Input Events object\n");
    return NULL;
  }

  gm->mouse_input_events = new_mouse_input_events();
  if (gm->mouse_input_events == NULL) {
    printf("new_level_manager: Failed to allocate memory for the Mouse Input Events object\n");
    return NULL;
  }

  return gm;
}

GameManager_t * new_testing_game_manager() {
  GameManager_t *gm = (GameManager_t*) malloc(sizeof(GameManager_t));
  if (gm == NULL) {
    printf("new_testing_game_manager: Failed to allocate memory for GameManager object\n");
    return NULL;
  }

  gm->level = new_testing_level();
  if (gm->level == NULL) {
    printf("new_testing_game_manager: Failed to allocate memory for the Level object\n");
    return NULL;
  }

  gm->kbd_input_events = new_kbd_input_events();
  if (gm->kbd_input_events == NULL) {
    printf("new_testing_game_manager: Failed to allocate memory for the Keyboard Input Events object\n");
    return NULL;
  }

  gm->mouse_input_events = new_mouse_input_events();
  if (gm->mouse_input_events == NULL) {
    printf("new_testing_game_manager: Failed to allocate memory for the Mouse Input Events object\n");
    return NULL;
  }

  return gm;
}

void free_game_manager(GameManager_t *gm) {
  free_level(gm->level);
  free_kbd_input_events(gm->kbd_input_events);
  free_mouse_input_events(gm->mouse_input_events);
  free(gm);
}

// Update is executed once every frame
void update(GameManager_t* gm) {
  
}

void render(GameManager_t *gm) {
  render_level(gm->level);
}

uint8_t start_game() {
  
  printf("start_game: Creating GameManager object\n");
  GameManager_t *gm = new_testing_game_manager();
  if (gm == NULL) {
    printf("start_game: Failed to create GameManager object\n");
    return 1;
  }
  printf("start_game: Created Game Manager\n");

  uint32_t kbd_bit_mask;
  uint32_t timer0_bit_mask;
  uint32_t mouse_bit_mask;

  if (hw_manager_subscribe_int(&timer0_bit_mask, &kbd_bit_mask, &mouse_bit_mask)) {
    printf("start_game: Failed to enable interrupts\n");
  }
  printf("start_game: Subscribed to all interrupts\n");

	int r, ipc_status;
		message msg;

	/* GAME LOOP */
  /* aka interrupt loop */
  bool mouse_interrupt=false, kbd_interrupt=false, timer0_interrupt=false;
  printf("start_game: Entering game loop\n");
	while (!gm->kbd_input_events->key_esc_down) {
		if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
			printf("start_game: driver_receive failed with: %d", r);
			continue;
		}

		if (is_ipc_notify(ipc_status)) {
			switch (_ENDPOINT_P(msg.m_source)) {
				case HARDWARE: /* hardware interrupt notification */
					if (msg.m_notify.interrupts & mouse_bit_mask) {
						hw_manager_mouse_ih();
            mouse_interrupt = true;
          }
					
          if (msg.m_notify.interrupts & kbd_bit_mask) {
            hw_manager_kbd_ih();
            kbd_interrupt = true;
          }
          
          if (msg.m_notify.interrupts & timer0_bit_mask) {
            hw_manager_timer0_ih();
            timer0_interrupt = true;
          }

					break;
				default:
					break; /* no other notifications expected: do nothing */     
			}
		}

    // This is our place for "out of the critical path stuff"
    if (kbd_interrupt) {
      hw_manager_kbd(gm->kbd_input_events);
      kbd_interrupt = false;
    }

    if (mouse_interrupt) {
      hw_manager_mouse(gm->mouse_input_events);
      mouse_interrupt = false;
    }

    if (timer0_interrupt) {
      if (hw_manager_is_frame()) {
        update(gm);
        render(gm);
        reset_inputs(gm->kbd_input_events, gm->mouse_input_events);
      }
      timer0_interrupt = false;
    }    
	}
  
  printf("start_game: Game loop ended\n");

  free_game_manager(gm);
  hw_manager_unsubscribe_int();
  printf("start_game: Unsubscribed to all interrupts\n");

  return 0;
}
