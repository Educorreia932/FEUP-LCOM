#pragma once

#include <lcom/lcf.h>

/* INPUT EVENT STUFF */

typedef struct KbdInputEvents {
    bool right_arrow_down; // true only the frame the user pressed the key
    bool right_arrow;
    bool left_arrow_down;
    bool left_arrow;
    bool key_z_down;
    bool key_z;
    bool key_x_down;
    bool key_x;
    bool key_c_down;
    bool key_c;
    bool key_esc_down;
    bool key_esc;
} KbdInputEvents_t;

typedef struct MouseInputEvents {
    bool left_button_down; // Left button
    bool left_button;
    bool right_button_down; // Right button
    bool right_button;
    int32_t x_delta;
    int32_t y_delta; 
} MouseInputEvents_t;

// Sets all bools to false
KbdInputEvents_t* new_kbd_input_events();
void free_kbd_input_events(KbdInputEvents_t* kbd_input_ev);

// Sets all bools to false
MouseInputEvents_t* new_mouse_input_events();
void free_mouse_input_events(MouseInputEvents_t* mouse_input_ev);
