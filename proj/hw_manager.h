#pragma once

#include <lcom/lcf.h>
#include "input_events.h"


/* SUBSCRIBE & UNSUBCRIBE INT */

uint8_t hw_manager_subscribe_int(uint32_t *timer0_mask, uint32_t *kbd_mask, uint32_t *mouse_mask);

void hw_manager_unsubscribe_int();

/* INT HANDLER STUFF */

void hw_manager_timer0_ih();

bool hw_manager_is_frame();

void hw_manager_kbd_ih();

void hw_manager_kbd(KbdInputEvents_t* kbd_ev);

void hw_manager_mouse_ih();

void hw_manager_mouse(MouseInputEvents_t* mouse_ev);

void reset_inputs(KbdInputEvents_t* kbd_eb, MouseInputEvents_t* mouse_ev);
