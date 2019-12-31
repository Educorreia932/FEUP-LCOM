#include "hw_manager.h"
#include "timer.h"
#include "keyboard.h"
#include "mouse.h"
#include "video.h"
#include "rtc.h"
#include "uart.h"

// Frame rate
#define FRAME_PERIOD 1  // 60 fps

/* SUBSCRIBE & UNSUBCRIBE INT */

uint8_t hw_manager_subscribe_int (uint32_t *timer0_mask, uint32_t *kbd_mask, uint32_t *mouse_mask, uint32_t *rtc_mask, uint32_t *uart_mask) {

	/* ENABLE MOUSE STUFF */
	if (mouse_subscribe_int(mouse_mask))
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
	if (timer0_subscribe_int(timer0_mask))
		return 1;

	/* ENABLE KEYBOARD */
	if (kbd_subscribe_int(kbd_mask)) 
		return 1;

	/* ENABLE RTC */
	// Read C register 
	uint32_t reg_c = 0;

	rtc_read_register(RTC_REG_C, &reg_c);

	printf("%x\n", reg_c);

	if (rtc_subscribe_int(rtc_mask))
		return 1;

	/* ENABLE UART */
	if (uart_set_conf())
		return 1;
	
	uart_initialize_sw_queues();

	if (uart_subscribe_int(uart_mask))
		return 1;

	return 0;
}

void hw_manager_unsubscribe_int() {
	timer0_unsubscribe_int();
	kbd_unsubscribe_int();
	
	// MOUSE
	mouse_unsubscribe_int();
	mouse_data_reporting_disable();

	uint32_t data;

	// RTC
	rtc_read_register(RTC_REG_B, &data);

	data &= ~RTC_AIE;

	rtc_write_register(RTC_DATA_REG, data);

	rtc_unsubscribe_int();

	// UART
	uart_free_sw_queues();
	uart_unsubscribe_int();

}

/* INT HANDLER STUFF */

inline void hw_manager_timer0_ih() {
	timer0_int_handler();
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

inline void hw_manager_uart_ih() {
	uart_ih();
}

inline void hw_manager_uart_send_char(uint8_t to_send) {
	uart_send_char(to_send);
}

inline uint8_t hw_manager_uart_front() {
	return uart_receiver_q_front();
}

inline void hw_manager_uart_pop() {
	uart_receiver_q_pop();
} 

inline bool hw_manager_uart_is_empty() {
	return uart_receiver_q_empty();
}
