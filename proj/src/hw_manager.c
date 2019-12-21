#include "hw_manager.h"
#include "timer.h"
#include "keyboard.h"
#include "mouse.h"
#include "video.h"
#include "rtc.h"

// Frame rate
#define FRAME_PERIOD 1  // 60 fps

/* SUBSCRIBE & UNSUBCRIBE INT */

uint8_t hw_manager_subscribe_int (uint32_t *timer0_mask, uint32_t *kbd_mask, uint32_t *mouse_mask, uint32_t* rtc_mask) {
	uint8_t timer0_bit_no = TIMER0_IRQ;
	uint8_t kbd_bit_no = KBD_IRQ;
	uint8_t mouse_bit_no = MOUSE_IRQ;
	uint8_t rtc_bit_no = RTC_IRQ;

	// Set the masks to send outside this function
	*timer0_mask = BIT(timer0_bit_no);
	*kbd_mask = BIT(kbd_bit_no);
	*mouse_mask = BIT(mouse_bit_no);
	*rtc_mask = BIT(rtc_bit_no);

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

	/* ENABLE RTC */
	// Read C register 
	uint32_t reg_c = 0;

	rtc_read_register(RTC_REG_C, &reg_c);

	printf("%x\n", reg_c);

	if (rtc_subscribe_int(&rtc_bit_no))
		return 1;

	return 0;
}

void hw_manager_unsubscribe_int() {
	timer_unsubscribe_int();
	kbd_unsubscribe_int();
	mouse_unsubscribe_int();
	mouse_data_reporting_disable();

	uint32_t data;

	rtc_read_register(RTC_REG_B, &data);

	data &= ~RTC_AIE;

	rtc_write_register(RTC_DATA_REG, data);

	rtc_unsubscribe_int();
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

void hw_manager_kbd() {

  	analyse_scancode();
	  
  	if (is_scancode_complete)
		kbd_input_events_process_scancode();

}

inline void hw_manager_mouse_ih() {
	mouse_ih();
}

void hw_manager_mouse() {

	mouse_data_handler();

	if (is_mouse_packet_complete) {
		
		mouse_input_events_process_packet();

		is_mouse_packet_complete = false;    
	}

}

void hw_manager_rtc_ih() {
	rtc_ih();
}

void hw_manager_rtc_set_alarm(uint32_t period) {
	rtc_set_alarm(period);
}

/* VIDEO WRAPPERS */

inline void hw_manager_switch_double_buffer() {
	switch_double_buffer();
}

inline uint8_t hw_manager_enter_video_mode() {
	return vg_init(0x117) == NULL;
} 

inline void hw_manager_exit_video_mode() {
	vg_exit();
}
