// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>

#include <stdint.h>
#include <stdio.h>

// Any header files included below this line should have been created by you
#include "i8042.h"
#include "mouse.h"
#include "mouse_gesture.h"
#include "timer.h"

int main(int argc, char *argv[]) {
	// sets the language of LCF messages (can be either EN-US or PT-PT)
	lcf_set_language("EN-US");

	// enables to log function invocations that are being "wrapped" by LCF
	// [comment this out if you don't want/need/ it]
	lcf_trace_calls("/home/lcom/labs/lab4/trace.txt");

	// enables to save the output of printf function calls on a file
	// [comment this out if you don't want/need it]
	lcf_log_output("/home/lcom/labs/lab4/output.txt");

	// handles control over to LCF
	// [LCF handles command line arguments and invokes the right function]
	if (lcf_start(argc, argv))
	return 1;

	// LCF clean up tasks
	// [must be the last statement before return]
	lcf_cleanup();

	return 0;
}

int (mouse_test_packet)(uint32_t cnt) {
	uint8_t mouse_bit_no = MOUSE_IRQ;

	// Only avoids making this operation on every notification
	int mouse_bit_mask = BIT(mouse_bit_no);

	if (mouse_set_stream_mode())
		return 1;

	if (mouse_data_reporting_enable()) // Change to command
		return 1;

	if (mouse_subscribe_int(&mouse_bit_no))
		return 1;

	int r, ipc_status;
		message msg;

	// Interrupt loop
	while (cnt) {
		if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
			printf("driver_receive failed with: %d", r);
			continue;
		}

		if (is_ipc_notify(ipc_status)) {
			switch (_ENDPOINT_P(msg.m_source)) {
				case HARDWARE: /* hardware interrupt notification */
					if (msg.m_notify.interrupts & mouse_bit_mask) { /* subscribed interrupt */		
						mouse_ih();
													
						if (mouse_ih_error)
							return 1;
			
						if (mouse_data_handler())
							return 1;
			
						if (is_mouse_packet_complete) {
							cnt--;
							mouse_print_packet(&mouse_parsed_packet);
						}
					}

					break;
				default:
					break; /* no other notifications expected: do nothing */     
			}
		}

		else { /* received a standard message, not a notification */
			/* no standard messages expected: do nothing */
		}
	}

	if (mouse_unsubscribe_int())
		return 1;

	if (mouse_data_reporting_disable())
		return 1;

	return 0;
}

int (mouse_test_remote)(uint16_t period, uint8_t cnt) {
	// Set mouse to remote mode and disable data reporting
	if (mouse_set_remote_mode())
	return 1;
	if (mouse_data_reporting_disable())
	return 1;

	uint8_t status;
	uint8_t bytes_read;
	while (cnt) {
	
	// Send cmd to read a packet to the mouse
	if (mouse_read_data())
		return 1;

	bytes_read = 3;
	while (bytes_read) { // We will read 3 bytes (a complete packet)

		// Read status
		if (util_sys_inb(MOUSE_STAT_REG, &status))
		return 1;
		
		// Check if the output buffer is full with mouse data
		if (status & (ST_MOUSE_DATA | ST_OUT_BUF)) {

		// Pretty much same treatment as with the interrupts
		mouse_ih();
					
		if (mouse_ih_error)
			return 1;
		
		if (mouse_data_handler())
			return 1;

		--bytes_read;
		}
	}
	
	if (is_mouse_packet_complete) {
		cnt--;
		// printf("%x, %x, %x\n", mouse_parsed_packet.bytes[0], mouse_parsed_packet.bytes[1], mouse_parsed_packet.bytes[2]);
		mouse_print_packet(&mouse_parsed_packet);
	}

	// We have read out packet, so we wait for period miliseconds
	tickdelay(micros_to_ticks(period * 1000)); // *1000 to convert to microseconds
	}

	// Reenable stream mode
	if (mouse_set_stream_mode())
	return 1;
	
	// Disable data reporting
	if (mouse_data_reporting_disable())
	return 1;

	// Restore KBC byte to default state (returned by minix_get_dflt_kbc_byte())
	if (kbc_send_cmd(IN_BUF_CMD, WRITE_CMD_BYTE))
	return 1;
	if (kbc_send_cmd(IN_BUF_ARGS, minix_get_dflt_kbc_cmd_byte()))
	return 1;

	return 0;
}

int (mouse_test_async)(uint8_t idle_time) {
	uint8_t mouse_bit_no = MOUSE_IRQ;
	uint8_t timer0_bit_no = TIMER0_IRQ;

	// Subscribe to timer0
	if (timer_subscribe_int(&timer0_bit_no))
	return 1;

	// Set mouse to streaming mode with data reporting enabled
	if (mouse_set_stream_mode())
	return 1;
	if (mouse_data_reporting_enable())
	 	return 1;
	
	// Subscribe to mouse interrupts
	if (mouse_subscribe_int(&mouse_bit_no)) {
		timer_unsubscribe_int(); // If it fails, we need to unsubscribe from timer0
	return 1;
	}

	// Only avoids making this operation on every notification
	int mouse_bit_mask = BIT(mouse_bit_no);
	int timer0_bit_mask = BIT(timer0_bit_no);
	uint8_t idle_countdown = idle_time;

	int r, ipc_status;
	message msg;

	// Interrupt loop
	while (idle_countdown) {
			if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
				printf("driver_receive failed with: %d", r);
				continue;
			}

			if (is_ipc_notify(ipc_status)) {
				switch (_ENDPOINT_P(msg.m_source)) {
					case HARDWARE: /* hardware interrupt notification */

			/* -------- Timer0 Interrupts -------- */
			if (msg.m_notify.interrupts & timer0_bit_mask) {
				timer_int_handler();
				if (global_timer0_counter == 60) {
				global_timer0_counter = 0;
				--idle_countdown;
				}
			}

			/* -------- Mouse Interrupts -------- */
						if (msg.m_notify.interrupts & mouse_bit_mask) { /* subscribed interrupt */
				
				mouse_ih();
														
							if (mouse_ih_error)
				return 1;
				
				if (mouse_data_handler())
				return 1;
				
				if (is_mouse_packet_complete) {
				idle_countdown = idle_time;
				global_timer0_counter = 0;
				// printf("%x, %x, %x\n", mouse_parsed_packet.bytes[0], mouse_parsed_packet.bytes[1], mouse_parsed_packet.bytes[2]);
				mouse_print_packet(&mouse_parsed_packet);
				}
						}

						break;
					default:
						break; /* no other notifications expected: do nothing */     
				}
			}

			else { /* received a standard message, not a notification */
				/* no standard messages expected: do nothing */
			}
	}

	// Unsubscribe from timer0 interrupts
	if (timer_unsubscribe_int())
	return 1;

	// Unsubscribe from mouse interrupts
	if (mouse_unsubscribe_int())
		return 1;

	// Mouse is already on streaming mode, only need to disable data reporting
	if (mouse_data_reporting_disable())
	return 1;

	return 0;
}

int (mouse_test_gesture)(uint8_t x_len, uint8_t tolerance) {
	uint8_t mouse_bit_no = MOUSE_IRQ;

	// Only avoids making this operation on every notification
	int mouse_bit_mask = BIT(mouse_bit_no);

	if (mouse_set_stream_mode())
		return 1;

	if (mouse_data_reporting_enable())
	 	return 1;
	
	if (mouse_subscribe_int(&mouse_bit_no))
		return 1;

	int r, ipc_status;
		message msg;
	
	bool notFoundGesture = true;

	// Interrupt loop
	while (notFoundGesture) {
		if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
			printf("driver_receive failed with: %d", r);
			continue;
		}

		if (is_ipc_notify(ipc_status)) {
			switch (_ENDPOINT_P(msg.m_source)) {
				case HARDWARE: /* hardware interrupt notification */
					if (msg.m_notify.interrupts & mouse_bit_mask) { /* subscribed interrupt */     
						mouse_ih();
																						
						if (mouse_ih_error)
							return 1;

						if (mouse_data_handler())
							return 1;
						}

						break;
				default:
					break; /* no other notifications expected: do nothing */     
			}
		}

		else { /* received a standard message, not a notification */
				/* no standard messages expected: do nothing */
		}

		if (is_mouse_packet_complete) { // Time for state machine stuff :D
			
			notFoundGesture = !gesture_sm(x_len, tolerance);

			is_mouse_packet_complete = false;

			mouse_print_packet(&mouse_parsed_packet);
		}
	}

	if (mouse_unsubscribe_int())
		return 1;

	if (mouse_data_reporting_disable())
		return 1;

	return 0;
}
