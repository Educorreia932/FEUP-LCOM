#include <lcom/lcf.h>
#include <lcom/lab3.h>

#include "i8042.h"
#include "utils.h"

extern uint8_t scancode, is_scancode_complete;

int (kbd_subscribe_int)(uint8_t *bit_no);
int (kbd_unsubscribe_int)();

int kbc_get_scancode();
void (kbc_ih)();
void analyse_scancode();
int kbc_print_scancode_wrapper();

int kbc_reenable_default_int();
