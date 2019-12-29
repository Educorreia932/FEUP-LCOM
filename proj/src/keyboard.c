#include "keyboard.h"

/** @defgroup kbd Keyboard
 *  @{
 */

static uint8_t scancode_bytes[2], st, valid_scancode=0;
uint8_t scancode, is_scancode_complete=1, scancode_no_bytes;
static int kbd_hook_id;

/**
 * @brief Subscribes keyboard interrupts 
 * Sends the bit number for the interrupt through bit_no and saves the hook id on kbd_hook_id to be used later for unsubscribing and other actions.
 * @return 0 on success, 1 otherwise
*/
int (kbd_subscribe_int)(uint32_t *bit_mask) {
	if (!bit_mask) // Check if pointer is NULL
		return 1;

	kbd_hook_id = KBD_IRQ;
	*bit_mask = BIT(KBD_IRQ);

	if (sys_irqsetpolicy(KBD_IRQ, IRQ_EXCLUSIVE | IRQ_REENABLE, &kbd_hook_id))
		return 1;

	return 0;
}

/** 
 * @brief Unsubscribes keyboard interrupts 
 * @returns 0 on success, 1 otherwise
 */
int(kbd_unsubscribe_int)() {
	return sys_irqrmpolicy(&kbd_hook_id);
}

/** @brief Reads a scancode from the kbc output buffer
 * @note Supports 2 byte scancodes, but it will wait for the second call to recognize the full scancode
 * @returns 0 upon success, 1 otherwise
 */
int kbc_get_scancode() {
	// Read status
	if (util_sys_inb(STAT_REG, &st)) {
		valid_scancode = 0;
		return 1;
	}

	// Check if output buffer is full
	if (!(st & ST_OUT_BUF))	{
		valid_scancode = 0;
		return 1;
	}

	// Read scancode
	if (util_sys_inb(OUT_BUF, &scancode)) { 
		valid_scancode = 0;
		return 1;
	}

	// If either one is set to 1, there's an error
	if (st & (ST_PAR_ERR | ST_TO_ERR | ST_MOUSE_DATA)) {
		valid_scancode = 0;
		return 1;
	}

	valid_scancode = 1;

	return 0;
}

void kbd_ih() {
	kbc_get_scancode();
}

/** 
 * @brief It's sole purpose is to parse both 1 & 2 byte scancodes 
 */
void analyse_scancode() {
	if (valid_scancode) { // Checks if the current scancode was invalid (error in the read operation)
		// Whenever is_scancode_complete is false,
		// a 2 byte scancode is being read
		if (is_scancode_complete) {
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

		else { // Reading the second byte of a scancode
			scancode_bytes[1] = scancode;
			is_scancode_complete = 1;
		}
	}
}

/** 
 * @returns 0 upon success, 1 otherwise
 */
int kbc_reenable_default_int() {
	if (kbc_send_cmd(IN_BUF_CMD, READ_CMD_BYTE))
		return 1;

	uint8_t cmd;

	if (kbc_read_outbf(OUT_BUF, &cmd, false))
		return 1;

	cmd |= CMD_BYTE_ENABLE_INT_KBD;

	if (kbc_send_cmd(IN_BUF_CMD, WRITE_CMD_BYTE))
		return 1;

	if (kbc_send_cmd(IN_BUF_ARGS, cmd))
		return 1;

	return 0;
}

/** @} end of Keyboard */
