#include "hw_manager.h"
#include "timer.h"
#include "keyboard.h"
#include "mouse.h"

// Frame rate
#define FRAME_PERIOD 2  // 30 fps

// 1 byte scancodes
#define KBD_ESC_MAKECODE 0x01
#define KBD_ESC_BREAKCODE 0x81
#define KBD_Z_MAKECODE 0x2c
#define KBD_Z_BREAKCODE 0xac
#define KBD_X_MAKECODE 0x2d
#define KBD_X_BREAKCODE 0xad

// 2 byte scancodes, but we only need to compare the second one
#define KBD_ARROW_LEFT_MAKECODE 0x4b
#define KBD_ARROW_LEFT_BREAKCODE 0xcb
#define KBD_ARROW_RIGHT_MAKECODE 0x4d
#define KBD_ARROW_RIGHT_BREAKCODE 0xcd

/* SUBSCRIBE & UNSUBCRIBE INT */

uint8_t hw_manager_subscribe_int (uint32_t *timer0_mask, uint32_t *kbd_mask, uint32_t *mouse_mask) {
	uint8_t timer0_bit_no = TIMER0_IRQ;
	uint8_t kbd_bit_no = KBD_IRQ;
	uint8_t mouse_bit_no = MOUSE_IRQ;

	// Set the masks to send outside this function
	*timer0_mask = BIT(timer0_bit_no);
	*kbd_mask = BIT(kbd_bit_no);
	*mouse_mask = BIT(mouse_bit_no);

	/* ENABLE MOUSE STUFF */
	if (mouse_subscribe_int(&mouse_bit_no))
		return 1;

	if (mouse_disable_int())
		return 1;

	// Set mouse to streaming mode with data reporting enabled
	if (mouse_set_stream_mode())
		return 1;
	if (mouse_data_reporting_enable())
	 	return 1;

	if (mouse_enable_int())
		return 1;

	/* ENABLE TIMER 0 */
	if (timer_subscribe_int(&timer0_bit_no))
		return 1;

	/* ENABLE KEYBOARD */
	if (kbd_subscribe_int(&kbd_bit_no))
		return 1;

	return 0;
}

void hw_manager_unsubscribe_int() {
	timer_unsubscribe_int();
	kbd_unsubscribe_int();
	mouse_unsubscribe_int();
	mouse_data_reporting_disable();
}

/* INT HANDLER STUFF */

inline void hw_manager_timer0_ih() {
	timer_int_handler();
}

bool hw_manager_is_frame() {
	if (global_timer0_counter >= FRAME_PERIOD) {
		global_timer0_counter = 0;
		return true;
	}

	return false;
}

inline void hw_manager_kbd_ih() {
	kbd_ih();
}

void hw_manager_kbd(KbdInputEvents_t* kbd_ev) {

  analyse_scancode();
  if (is_scancode_complete) {
    if (scancode_no_bytes == 1) {
      // 1 byte scancodes
      switch (scancode) {
        case KBD_ESC_MAKECODE:
          if (!kbd_ev->key_esc)
            kbd_ev->key_esc_down = true;
          kbd_ev->key_esc = true;
          break;
        case KBD_ESC_BREAKCODE:
          kbd_ev->key_esc = false;
          break;
        case KBD_Z_MAKECODE:
          if (!kbd_ev->key_z)
            kbd_ev->key_z_down = true;
          kbd_ev->key_z = true;
          break;
        case KBD_Z_BREAKCODE:
          kbd_ev->key_z = false;
          break;
        case KBD_X_MAKECODE:
          if (!kbd_ev->key_x)
            kbd_ev->key_x_down = true;
          kbd_ev->key_x = true;
          break;
        case KBD_X_BREAKCODE:
          kbd_ev->key_x = false;
        // No need for a default
      }
    }
    else {
      // 2 byte scancodes
      switch (scancode) {
        case KBD_ARROW_LEFT_MAKECODE:
          if (!kbd_ev->left_arrow)
            kbd_ev->left_arrow_down = true;
          kbd_ev->left_arrow = true;
          break;
        case KBD_ARROW_LEFT_BREAKCODE:
          kbd_ev->left_arrow = false;
          break;
        case KBD_ARROW_RIGHT_MAKECODE:
          if (!kbd_ev->right_arrow)
            kbd_ev->right_arrow_down = true;
          kbd_ev->right_arrow = true;          
          break;
        case KBD_ARROW_RIGHT_BREAKCODE:
          kbd_ev->right_arrow = false;
          break;
        // No need for a default
      }
    }
  }
}

inline void hw_manager_mouse_ih() {
	mouse_ih();
}

void hw_manager_mouse(MouseInputEvents_t* mouse_ev) {
	mouse_data_handler();

	if (is_mouse_packet_complete) {
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

		is_mouse_packet_complete = false;    
	}

}

/* RESETTING NECESSARY INPUTS */

void reset_inputs(KbdInputEvents_t* kbd_ev, MouseInputEvents_t* mouse_ev) {
  
  // Reset kbd
  kbd_ev->key_esc_down = false;
  kbd_ev->key_z_down = false;
  kbd_ev->key_x_down = false;
  kbd_ev->left_arrow_down = false;
  kbd_ev->right_arrow_down = false;

	// Reset mouse
	mouse_ev->left_button_down = false;
	mouse_ev->right_button_down = false;
	mouse_ev->x_delta = 0;
	mouse_ev->y_delta = 0;
}
