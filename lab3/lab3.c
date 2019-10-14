#include <lcom/lab3.h>
#include <lcom/lcf.h>

#include <minix/sysutil.h>

#include <stdbool.h>
#include <stdint.h>

#include "i8042.h"
#include "keyboard.h"

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

		if (!kbc_error)
			kbc_print_scancode_wrapper();	
}

int (kbd_test_poll)() {

	uint8_t poll_st = 0;

	while (1)
	{
		if (util_sys_inb(STAT_REG, &poll_st)) // Read status (replace by a function?)
			return 1;
		// Check if there are errors in the status
		if (poll_st & ST_OUT_BUF) // Out buffer full (but it's fine)
		{
			if (poll_st & (ST_PAR_ERR | ST_TO_ERR)) // Invalid breakcode in out buffer (problem)
			{
				if (tickdelay(micros_to_ticks(KBC_WAIT)))
					return 1;		
				continue;
			}
		}
		else
		{
			if (tickdelay(micros_to_ticks(KBC_WAIT)))
				return 1;		
			continue;
		}		

		// It's all fine, time to read the scancode
		kbc_get_scancode();
		if (!kbc_error)
			kbc_print_scancode_wrapper();
		
		if (scancode == ESC_BREAKCODE)
			break;
	}

	kbc_reenable_default_int();

	if (kbd_print_no_sysinb(no_of_calls))
		return 1;

	return 0;
}

int (kbd_test_timed_scan)(uint8_t n) {
	/* To be completed by the students */
	printf("%s is not yet implemented!\n", __func__);

	return 1;
}
