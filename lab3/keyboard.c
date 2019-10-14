#include "keyboard.h"

static uint8_t scancode_no_bytes, scancode_bytes[2], st;
uint8_t scancode, kbc_error, is_scancode_complete=1;
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
// Was only used for reenabling the interrupts
// Has status as a parameter so we may save util_sys_inb() calls
int kbc_send_cmd(uint8_t port, uint8_t cmd, uint8_t *status)
{	
	// This section waits until it can write to input buffer
	// or it reaches a time out state
	uint8_t i = TIMEOUT_ATTEMPTS;
	while (i) // Tries for i attempts
	{
		// Can only write if the ST_IN_BUF is set to 0
		if (st & ST_IN_BUF)
		{
			--i;
			if (tickdelay(micros_to_ticks(KBC_WAIT)))
				return 1;
			if (util_sys_inb(STAT_REG, status)) // Read status (replace by a function?)
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

// Was used only for reenabling the interrupts
// Reads something from port (used mainly to read the responses from commands)
// Has status as a parameter so we may save util_sys_inb() calls
int kbc_read_outbf(uint8_t port, uint8_t *content, uint8_t *status)
{
	// This section waits until there is something
	// to read from the output buffer
	// or it reaches a time out state
	uint8_t i = TIMEOUT_ATTEMPTS;
	while (i) // Tries for i attempts
	{
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
			if (util_sys_inb(port, status)) // Read status (replace by a function?)
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
// Supports 2 byte scancodes, but it will wait
// for the second call to recognize the full scancode
int kbc_get_scancode(uint8_t isPoll)
{
	if (util_sys_inb(STAT_REG, &st))
		return 1;

	// Only make this check if in polling mode
	// because sometimes the interrupts can be faster
	// than the status updating
	if (isPoll && !(st & ST_OUT_BUF))
		return 1;

	// If either one is set to 1, there's an error
	if (st & (ST_PAR_ERR | ST_TO_ERR | ST_MOUSE_DATA))
		return 1;

	if (util_sys_inb(OUT_BUF, &scancode)) // Read scancode
		return 1;
	
	if (is_scancode_complete)
	{
		scancode_bytes[0] = scancode;
		if (scancode == SCANCODE_TWO_BYTES)
		{ // Will have to wait for the second byte
			is_scancode_complete = 0;
			scancode_no_bytes = 2;
		}
		else
		{ // Regular 1 byte scancodes
			is_scancode_complete = 1;
			scancode_no_bytes = 1;
		}
	}
	else // Reading the second byte of a scancode
	{
		scancode_bytes[1] = scancode;
		is_scancode_complete = 1;
	}
	
	return 0;
}

// Unimportant function for the project as it is only used for lab3
// It is stored here only to avoid more global variables
int kbc_print_scancode_wrapper()
{
	// In 2 byte scancodes, the difference between make and break
	// will already be stored in scancode
	if (is_makecode(scancode)) /* Makecode */
	{
		if (kbd_print_scancode(true, scancode_no_bytes, scancode_bytes))
			return 1;
	}
	else /* Breakcode */
	{
		if (kbd_print_scancode(false, scancode_no_bytes, scancode_bytes))
			return 1;
	}
	return 0;
}

// Wasn't requested, but reenables the keyboard interrupts
// Avoids having to call the executable from a remote shell
// to restore the keyboard interrupts
int kbc_reenable_default_int()
{
	if (util_sys_inb(STAT_REG, &st))
  	return 1;
	if (kbc_send_cmd(IN_BUF_CMD, READ_CMD_BYTE, &st))
		return 1;

	uint8_t cmd;
	if (util_sys_inb(STAT_REG, &st))
  	return 1;
	if (kbc_read_outbf(OUT_BUF, &cmd, &st))
		return 1;

	cmd |= CMD_BYTE_ENABLE_INT_KBD;
	
	if (util_sys_inb(STAT_REG, &st))
  	return 1;
	if (kbc_send_cmd(IN_BUF_CMD, WRITE_CMD_BYTE, &st))
		return 1;

	if (util_sys_inb(STAT_REG, &st))
  	return 1;
	if (kbc_send_cmd(IN_BUF_ARGS, cmd, &st))
		return 1;
	
	return 0;
}
