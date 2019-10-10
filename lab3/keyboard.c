#include "keyboard.h"

uint8_t scancode, st, error;
int global_kbd_bit_no = KBD_IRQ;

int (kbd_subscribe_int)(uint8_t *bit_no) {
	if (!bit_no || *bit_no > 31)
		return 1;

	global_kbd_bit_no = *bit_no;

	if (sys_irqsetpolicy(KBD_IRQ, IRQ_EXCLUSIVE | IRQ_REENABLE, &global_kbd_bit_no))
		return 1;

	return 0;
}

int(kbd_unsubscribe_int)() {
	if (sys_irqrmpolicy(&global_kbd_bit_no))
		return 1;
	
	return 0;
}

void (kbc_int_handler)() {
	util_sys_inb(OUT_BUF, &scancode);
	util_sys_inb(STAT_REG, &st);

	if (st & PAR_ERR || st & TO_ERR)
		error = 1;
}
