#include "keyboard.h"

static uint8_t scancode_bytes[1], st;
uint8_t scancode, kbc_error;
int global_kbd_bit_no = KBD_IRQ;


int (kbd_subscribe_int)(uint8_t *bit_no) {
	if (!bit_no || *bit_no > 31)
		return 1;

	global_kbd_bit_no = *bit_no;

	if (sys_irqsetpolicy(KBD_IRQ, IRQ_EXCLUSIVE | IRQ_REENABLE, &global_kbd_bit_no))
		return 1;

	return 0;
}

// Unsubscribes the keyboard interrupts
int(kbd_unsubscribe_int)() {
	if (sys_irqrmpolicy(&global_kbd_bit_no))
		return 1;
	
	return 0;
}

// Sends a command to the kbc
int kbc_send_cmd(uint8_t port, uint8_t cmd)
{	
	// This section waits until it can write to input buffer
	// or it reaches a time out state
	uint8_t i = TIMEOUT_ATTEMPTS;
	while (i) // Tries for i attempts
	{
		if (util_sys_inb(STAT_REG, &st)) // Read status (replace by a function?)
			return 1;
		// Can only write if the ST_IN_BUF is set to 0
		if (st & ST_IN_BUF)
		{
			--i;
			if (tickdelay(micros_to_ticks(KBC_WAIT)))
				return 1;
			continue;
		}
		else
			break;		
	}
	if (i == 0) // Timed out
		return 1;

	if (sys_outb(port, cmd)) // Write the command
		return 1;

	return 0;
}

int kbc_read_outbf(uint8_t *content)
{
	// This section waits until it can write to input buffer
	// or it reaches a time out state
	uint8_t i = TIMEOUT_ATTEMPTS;
	while (i) // Tries for i attempts
	{
		if (util_sys_inb(STAT_REG, &st)) // Read status (replace by a function?)
			return 1;
		// Can only read if the ST_OUT_BUF is set to 1
		if (st & ST_OUT_BUF)
		{
			break;
		}
		else
		{
			--i;
			if (tickdelay(micros_to_ticks(KBC_WAIT)))
				return 1;
			continue;
		}		
	}
	if (i == 0) // Timed out
		return 1;

	if (util_sys_inb(OUT_BUF, content))
		return 1;
	
	return 0;

}

// Reads a scancode from the kbc output buffer
// Does not take into account timeouts because
// because it's also alled by the interrupt handler
void kbc_get_scancode()
{
	kbc_error = 1; // Assume there'll be an error

	if (util_sys_inb(OUT_BUF, &scancode)) // Read scancode
		return;
	
	scancode_bytes[0] = scancode;

	kbc_error = 0; // Everything went smoothly
}

// This function returns a value because it will only be used for testing,
// and not for use in the project or other stuff, therefore the
// performance hinderence is not so important
int kbc_print_scancode_wrapper()
{
	if (is_makecode(scancode)) /* Makecode */
	{
		if (kbd_print_scancode(true, sizeof(scancode), scancode_bytes))
			return 1;
	}
	else /* Breakcode */
	{
		if (kbd_print_scancode(false, sizeof(scancode), scancode_bytes))
			return 1;
	}
	return 0;
}

void (kbc_int_handler)() {
	if (util_sys_inb(STAT_REG, &st)) // Read Status (replace by a function?)
	{
		kbc_error = 1;
		return;
	}

	if (st & (ST_PAR_ERR | ST_TO_ERR))
	{
		kbc_error = 1;
		return;
	}

	kbc_get_scancode();

}

int kbc_reenable_default_int()
{
	if (kbc_send_cmd(IN_BUF_CMD, READ_CMD_BYTE))
		return 1;

	uint8_t cmd;
	if (kbc_read_outbf(&cmd))
		return 1;

	cmd |= CMD_BYTE_ENABLE_INT_KBD;

	if (kbc_send_cmd(IN_BUF_CMD, WRITE_CMD_BYTE))
		return 1;
	if (kbc_send_cmd(IN_BUF_ARGS, cmd))
		return 1;
	
	return 0;
}
