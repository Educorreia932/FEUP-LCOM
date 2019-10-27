// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>

#include <stdint.h>
#include <stdio.h>

// Any header files included below this line should have been created by you
#include "i8042.h"
#include "mouse.h"

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

	if (mouse_data_reporting_enable())
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

	if (mouse_unsubscribe_int(&mouse_bit_no))
		return 1;

  if (mouse_data_reporting_disable())
    return 1;

	return 0;
}

int (mouse_test_remote)(uint16_t period, uint8_t cnt) {
    /* To be completed */
    printf("%s(%u, %u): under construction\n", __func__, period, cnt);
    return 1;
}

int (mouse_test_async)(uint8_t idle_time) {
    /* To be completed */
    printf("%s(%u): under construction\n", __func__, idle_time);
    return 1;
}

int (mouse_test_gesture)(uint8_t x_len, uint8_t tolerance) {
    /* To be completed */
    printf("%s: under construction\n", __func__);
    return 1;
}
