#include <lcom/lab3.h>
#include <lcom/lcf.h>

#include <minix/sysutil.h>

#include <stdbool.h>
#include <stdint.h>

#include "i8042.h"
#include "keyboard.h"

extern uint8_t scancode, st, error;	// Defined in keyboard
extern uint32_t no_of_calls;

int main(int argc, char *argv[]) {
	// sets the language of LCF messages (can be either EN-US or PT-PT)
	lcf_set_language("EN-US");

	// enables to log function invocations that are being "wrapped" by LCF
	// [comment this out if you don't want/need it]
	lcf_trace_calls("/home/lcom/labs/lab3/trace.txt");

	// enables to save the output of printf function calls on a file
	// [comment this out if you don't want/need it]
	lcf_log_output("/home/lcom/labs/lab3/output.txt");

	// handles control over to LCF
	// [LCF handles command line arguments and invokes the right function]
	if (lcf_start(argc, argv))
		return 1;

	// LCF clean up tasks
	// [must be the last statement before return]
	lcf_cleanup();

	return 0;
}

int(kbd_test_scan)() {
	uint8_t bit_no = KBD_IRQ;
	
	// Only avoids making this operation on every notification
	int kbd_bit_mask = BIT(bit_no);

  	if (kbd_subscribe_int(&bit_no))
	  	return 1;

	int r, ipc_status;
  	message msg;
	uint8_t bytes[1];
	
	// Interrupt loop
	while (scancode != ESC_BREAKCODE) { 
		if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
			printf("driver_receive failed with: %d", r);
			continue;
		}

		if (is_ipc_notify(ipc_status)) {
			switch (_ENDPOINT_P(msg.m_source)) {
				case HARDWARE: /* hardware interrupt notification */
					if (msg.m_notify.interrupts & kbd_bit_mask) { /* subscribed interrupt */
						kbc_ih();
						
						uint8_t size = sizeof(scancode);		

						//Implement in function
						if (util_get_LSB(scancode, bytes)) //Only one byte
							return 1;

						if (sizeof(scancode) > 1)
							if (util_get_MSB(scancode, bytes + 1)) //Two bytes
								return 1; 

						if (is_makecode(scancode)) /* Makecode */
							kbd_print_scancode(true, size, bytes);
						
						else /* Breakcode */
							kbd_print_scancode(false, size, bytes);
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

	if (kbd_unsubscribe_int(&bit_no))
		return 1;
	
	if (kbd_print_no_sysinb(no_of_calls))
		return 1;

	return 0;
}

void (kbc_ih)() {
		kbc_int_handler();
}

int (kbd_test_poll)() {
	/* To be completed by the students */
	printf("%s is not yet implemented!\n", __func__);

	return 1;
}

int (kbd_test_timed_scan)(uint8_t n) {
	/* To be completed by the students */
	printf("%s is not yet implemented!\n", __func__);

	return 1;
}
