#include "input_events.h"

/* KBD EVENTS */

KbdInputEvents_t* new_kbd_input_events() {
	KbdInputEvents_t* kbd_input_ev = (KbdInputEvents_t*) calloc(1, sizeof(KbdInputEvents_t));
	if (kbd_input_ev == NULL) {
		printf("new_kbd_input_events: Failed to allocate memory for KbdInputEvents object\n");
		return NULL;
	}
	return kbd_input_ev;
}

void free_kbd_input_events(KbdInputEvents_t* kbd_input_ev) {
  	if (kbd_input_ev == NULL) {
		printf("free_kbd_input_events: Cannot free a NULL pointer\n");
		return;
  	}
  	free(kbd_input_ev);
}

inline bool get_key(KbdInputEvents_t *kbd_ev, KeyboardMap_t map) {
	if (map > LARGEST_MAKECODE + EXTRA_MAKECODES) {
		printf("get_key: Invalid KeyboardMap value\n");
		return false;
	}
	return kbd_ev->key[map];
}

inline bool get_key_down(KbdInputEvents_t *kbd_ev, KeyboardMap_t map) {
	if (map > LARGEST_MAKECODE + EXTRA_MAKECODES) {
		printf("get_key: Invalid KeyboardMap value\n");
		return false;
	}
	return kbd_ev->key_down[map];
}

/* MOUSE EVENTS */

MouseInputEvents_t* new_mouse_input_events() {
	MouseInputEvents_t* mouse_input_ev = (MouseInputEvents_t*) calloc(1, sizeof(MouseInputEvents_t));
	if (mouse_input_ev == NULL) {
		printf("new_mouse_input_events: Failed to allocate memory for KbdInputEvents object\n");
		return NULL;
	}
	return mouse_input_ev;
}

void free_mouse_input_events(MouseInputEvents_t* mouse_input_ev) {
	if (mouse_input_ev == NULL) {
		printf("free_mouse_input_events: Cannot free a NULL pointer\n");
		return;
  	}
  	free(mouse_input_ev);
}


/* RESETTING NECESSARY INPUTS */

void reset_inputs(KbdInputEvents_t* kbd_ev, MouseInputEvents_t* mouse_ev) {
  
  	// Reset kbd key downs
  	memset(kbd_ev->key_down, 0, sizeof(kbd_ev->key_down));

	// Reset mouse
	mouse_ev->left_button_down = false;
	mouse_ev->right_button_down = false;
	mouse_ev->x_delta = 0;
	mouse_ev->y_delta = 0;
}
