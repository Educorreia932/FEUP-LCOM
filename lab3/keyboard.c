#include "keyboard.h"

static uint8_t scancode_no_bytes, scancode_bytes[2], st, valid_scancode=0;
uint8_t scancode, is_scancode_complete=1;
static int kbd_hook_id;

// Sends the bit number for the interrupt through bit_no
// and saves the hook id on kbd_hook_id to be used
// later for unsubscribing and other actions
int (kbd_subscribe_int)(uint8_t *bit_no) {

	if (!bit_no) // Check if pointer is NULL
		return 1;

	kbd_hook_id = KBD_IRQ;
	*bit_no = KBD_IRQ;

	if (sys_irqsetpolicy(KBD_IRQ, IRQ_EXCLUSIVE | IRQ_REENABLE, &kbd_hook_id))
		return 1;

	return 0;
}

// Unsubscribes the keyboard interrupts
int(kbd_unsubscribe_int)() {
	if (sys_irqrmpolicy(&kbd_hook_id))
		return 1;
		
	return 0;
}

// Sends a command to the kbc
// Used for reenabling the interrupts
int kbc_send_cmd(uint8_t port, uint8_t cmd)
{	
	// This section waits until it can write to input buffer
	// or it reaches a time out state
	uint8_t i = TIMEOUT_ATTEMPTS;
	while (i) // Tries for i attempts
	{
		if (util_sys_inb(STAT_REG, &st)) // Read Status
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

// Used for reenabling the interrupts
// Reads something from port (used mainly to read the responses from commands)
int kbc_read_outbf(uint8_t port, uint8_t *content)
{
	// This section waits until there is something
	// to read from the output buffer
	// or it reaches a time out state
	uint8_t i = TIMEOUT_ATTEMPTS;
	while (i) // Tries for i attempts
	{
		if (util_sys_inb(STAT_REG, &st)) // Read Status
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
// Supports 2 byte scancodes, but it will wait
// for the second call to recognize the full scancode
int kbc_get_scancode()
{

	if (util_sys_inb(STAT_REG, &st))
	{
		valid_scancode = 0;
		return 1;
	}	

	// Check if output buffer is full
	if (!(st & ST_OUT_BUF))
	{
		valid_scancode = 0;
		return 1;
	}	

	if (util_sys_inb(OUT_BUF, &scancode)) // Read scancode
	{
		valid_scancode = 0;
		return 1;
	}	

	// If either one is set to 1, there's an error
	if (st & (ST_PAR_ERR | ST_TO_ERR | ST_MOUSE_DATA))
	{
		valid_scancode = 0;
		return 1;
	}	

	valid_scancode = 1;
	
	return 0;
}


// It's sole purpose is to parse both 1 & 2 byte scancodes
void analyse_scancode() {
	if (valid_scancode) // Checks if the current scancode was invalid (error in the read operation)
	{
		// Whenever is_scancode_complete is false,
		// a 2 byte scancode is being read
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
	}
}

// Unimportant function for the project as it is only used for lab3
// It is stored here only to avoid more global variables
// (using static variables isntead)
int kbc_print_scancode_wrapper()
{
	// In 2 byte scancodes, the difference between make and break
	// will already be stored in scancode
	if (scancode < MAKE_TO_BREAK) // Checks if it's a make code
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

// Reenables the keyboard interrupts
// Avoids having to call the executable from a remote shell
// to restore the keyboard interrupts
// IMPORTANT:
// This is only meant to be called when interrupts are disabled
// Otherwise, the ih may "steal" our response and ruin everything
int kbc_reenable_default_int()
{
	if (kbc_send_cmd(IN_BUF_CMD, READ_CMD_BYTE))
		return 1;

	uint8_t cmd;
	if (kbc_read_outbf(OUT_BUF, &cmd))
		return 1;

	cmd |= CMD_BYTE_ENABLE_INT_KBD;
	
	if (kbc_send_cmd(IN_BUF_CMD, WRITE_CMD_BYTE))
		return 1;

	if (kbc_send_cmd(IN_BUF_ARGS, cmd))
		return 1;
	
	return 0;
}
