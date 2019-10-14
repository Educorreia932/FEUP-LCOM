#include <lcom/lcf.h>
#include <lcom/lab3.h>

#include "i8042.h"
#include "utils.h"

extern uint8_t scancode, kbc_error;
extern int global_kbd_bit_no;

int (kbd_subscribe_int)(uint8_t *bit_no);
int (kbd_unsubscribe_int)();
void (kbc_int_handler)();

int kbc_send_cmd(uint8_t port, uint8_t cmd);
int kbc_read_outbf(uint8_t *content);

void kbc_get_scancode();
int kbc_print_scancode_wrapper();

int kbc_reenable_default_int();
