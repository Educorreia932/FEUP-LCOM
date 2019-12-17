#include "input_events.h"
#include "keyboard.h"
#include "mouse.h"

// 2 byte scancodes, but we only need to compare the second one
#define KBD_ARROW_LEFT_MAKECODE 0x4b
#define KBD_ARROW_RIGHT_MAKECODE 0x4d
#define KBD_ARROW_DOWN_MAKECODE 0x48
#define KBD_ARROW_UP_MAKECODE 0x50
#define KBD_DELETE_MAKECODE 0x53
#define KBD_SUPER_MAKECODE 0x5B


typedef struct KbdInputEvents {
    bool key_down[LARGEST_SCANCODE + EXTRA_MAKECODES + 1];
    bool key[LARGEST_SCANCODE + EXTRA_MAKECODES + 1];
} KbdInputEvents_t;

KbdInputEvents_t* kbd_ev = NULL;

/* KBD EVENTS */

void initialize_kbd_input_events() {
	if (kbd_ev == NULL) {
	kbd_ev = (KbdInputEvents_t*) calloc(1, sizeof(KbdInputEvents_t));
		if (kbd_ev == NULL) {
			printf("new_kbd_input_events: Failed to allocate memory for KbdInputEvents object\n");
			exit(1);
		}
	}
}

void free_kbd_input_events() {
  	if (kbd_ev == NULL) {
		printf("free_kbd_input_events: Cannot free a NULL pointer\n");
		return;
  	}
  	free(kbd_ev);
}

inline bool get_key(KeyboardMap_t map) {
	if (map > LARGEST_SCANCODE + EXTRA_MAKECODES) {
		printf("get_key: Invalid KeyboardMap value\n");
		return false;
	}
	return kbd_ev->key[map];
}

inline bool get_key_down(KeyboardMap_t map) {
	if (map > LARGEST_SCANCODE + EXTRA_MAKECODES) {
		printf("get_key: Invalid KeyboardMap value\n");
		return false;
	}
	return kbd_ev->key_down[map];
}

void kbd_input_events_process_scancode() {
	// This aux_scancode will already have the positions in the array
	// We only need to verify if it was a make or breack code later on
	uint8_t aux_scancode = scancode & (~MAKE_TO_BREAK);

	if (scancode_no_bytes == 2) {
		// Two bytes, it's the special case...
		switch (aux_scancode) {
			case KBD_ARROW_LEFT_MAKECODE:
				aux_scancode = KBD_ARROW_LEFT;
				break;
			case KBD_ARROW_RIGHT_MAKECODE:
				aux_scancode = KBD_ARROW_RIGHT;
				break;
			case KBD_ARROW_UP_MAKECODE:
				aux_scancode = KBD_ARROW_UP;
				break;
			case KBD_ARROW_DOWN_MAKECODE:
				aux_scancode = KBD_ARROW_DOWN;
				break;
			case KBD_DELETE_MAKECODE:
				aux_scancode = KBD_DELETE;
				break;
			case KBD_SUPER_MAKECODE:
				aux_scancode = KBD_SUPER;
				break;
		}
	}

	if (scancode & MAKE_TO_BREAK) {
		// It was a breakcode
		kbd_ev->key[aux_scancode] = false;
	}
	else {
		// It was a makecode
		if (!kbd_ev->key[aux_scancode])
			kbd_ev->key_down[aux_scancode] = true;
		kbd_ev->key[aux_scancode] = true;
	}
}

/* MOUSE EVENTS */

typedef struct MouseInputEvents {
    bool left_button_down; // Left button
    bool left_button;
    bool right_button_down; // Right button
    bool right_button;
    int32_t x_delta;
    int32_t y_delta; 
} MouseInputEvents_t;

MouseInputEvents_t* mouse_ev = NULL;

void initialize_mouse_input_events() {
	if (mouse_ev == NULL) {
		mouse_ev = (MouseInputEvents_t*) calloc(1, sizeof(MouseInputEvents_t));
		if (mouse_ev == NULL) {
			printf("new_mouse_input_events: Failed to allocate memory for KbdInputEvents object\n");
		}
	}
}

void free_mouse_input_events() {
	if (mouse_ev == NULL) {
		printf("free_mouse_input_events: Cannot free a NULL pointer\n");
		return;
  	}
  	free(mouse_ev);
}

void mouse_input_events_process_packet() {
	// Left Button
	if (mouse_parsed_packet.lb) {
		if (!mouse_ev->left_button)
			mouse_ev->left_button_down = true;
		mouse_ev->left_button = true;
	}

	else
		mouse_ev->left_button = false;
	
	// Right Button
	if (mouse_parsed_packet.rb) {
		if (!mouse_ev->right_button)
			mouse_ev->right_button_down = true;
	}			

	else
		mouse_ev->right_button = false;
	
	// Movement Deltas
	if (!mouse_parsed_packet.x_ov)
		mouse_ev->x_delta += (int32_t) mouse_parsed_packet.delta_x;
	
	if (!mouse_parsed_packet.y_ov)
		mouse_ev->y_delta +=  (int32_t) mouse_parsed_packet.delta_y;
}

inline bool mouse_get_lb_down() {
	return mouse_ev->left_button_down;
}

inline bool mouse_get_lb() {
	return mouse_ev->left_button;
}

inline bool mouse_get_rb_down() {
	return mouse_ev->right_button_down;
}

inline bool mouse_get_rb() {
	return mouse_ev->right_button;
}

inline int32_t mouse_get_x_delta() {
	return mouse_ev->x_delta;
}

inline int32_t mouse_get_y_delta() {
	return mouse_ev->y_delta;
}



/* RESETTING NECESSARY INPUTS */

void reset_kbd_input_state() {
  
  	// Reset kbd key downs
  	memset(kbd_ev->key_down, 0, sizeof(kbd_ev->key_down));

}

void reset_mouse_input_state() {
	
	// Reset mouse
	mouse_ev->left_button_down = false;
	mouse_ev->right_button_down = false;
	mouse_ev->x_delta = 0;
	mouse_ev->y_delta = 0;

}
