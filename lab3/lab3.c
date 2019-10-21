#include <lcom/lab3.h>
#include <lcom/lcf.h>

#include <minix/sysutil.h>

#include <stdbool.h>
#include <stdint.h>

#include "i8042.h"
#include "keyboard.h"
#include "utils.h"
// #include "i8254.h" Already in "timer.h"
#include "timer.h"

// // The counter is declared here too, because it won't compile
// // with it only declared on timer.h, which is included too
// extern unsigned long int global_timer0_counter;

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
						analyse_scancode();
						if (is_scancode_complete)
							if (kbc_print_scancode_wrapper())
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
	}

	if (kbd_unsubscribe_int(&bit_no))
		return 1;
	
	if (kbd_print_no_sysinb(no_of_calls))
		return 1;

	return 0;
}

void (kbc_ih)() {
		kbc_get_scancode();
}

int (kbd_test_poll)() {
	while (scancode != ESC_BREAKCODE)
	{
		// ALl the verification is already done inside kbc_get_scancode()
		// So all we need to check is if it returns a non-zero
		if (kbc_get_scancode())
		{ // There was an error in the read operation
			if (tickdelay(micros_to_ticks(KBC_WAIT)))
				return 1;		
			continue;
		}
		
		// There was no error in the read operation (if there was, the cycle repeats until it is valid, thanks to the continue)
		analyse_scancode();

		if (is_scancode_complete)
			if (kbc_print_scancode_wrapper())
				return 1;
		
	}

	// Avoids the trouble of reenabling the
	// keyboard interrupts from a remote shell
	if (kbc_reenable_default_int())
		return 1;

	if (kbd_print_no_sysinb(no_of_calls))
		return 1;

	return 0;
}

int (kbd_test_timed_scan)(uint8_t n) {
	uint8_t idle_countdown = n;
	// These next vars their values from the function
	uint8_t kbd_bit_no, timer0_bit_no;
	
	// Subscribe to both interrupts
	if (timer_subscribe_int(&timer0_bit_no))
		return 1;

	if (kbd_subscribe_int(&kbd_bit_no))
	{ // If this has an error, the timer would still be subscribed, so we need to unsubscribe it
		timer_unsubscribe_int(); // Don't care if it ends in an error, we did all we could do
		return 1;
	}

	// Only avoids making this operation on every notification
	int kbd_bit_mask = BIT(kbd_bit_no);
	int timer0_bit_mask = BIT(timer0_bit_no);

	int r, ipc_status;
  message msg;
	
	// Interrupt loop
	while ((scancode != ESC_BREAKCODE) && idle_countdown) { 
		if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
			printf("driver_receive failed with: %d", r);
			continue;
		}

		if (is_ipc_notify(ipc_status)) {
			switch (_ENDPOINT_P(msg.m_source)) {
				case HARDWARE: /* hardware interrupt notification */
					if (msg.m_notify.interrupts & timer0_bit_mask)
					{
						timer_int_handler();
						if (global_timer0_counter == 60)
						{
							global_timer0_counter = 0;
							--idle_countdown;
						}
					}
					if (msg.m_notify.interrupts & kbd_bit_mask) { /* subscribed interrupt */
						kbc_ih();
						analyse_scancode();
						idle_countdown = n; // Reset the idle countdown
						global_timer0_counter = 0; // Make sure we are counting <idle> complete seconds
						if (is_scancode_complete)
						{
							if (kbc_print_scancode_wrapper())
								return 1;
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

	if (kbd_unsubscribe_int())
		return 1;
	
	if (timer_unsubscribe_int())
		return 1;

	// if (kbd_print_no_sysinb(no_of_calls))
	// 	return 1;

	return 0;
}
