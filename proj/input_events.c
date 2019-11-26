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
