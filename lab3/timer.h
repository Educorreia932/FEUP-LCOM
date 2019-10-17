#include <lcom/lcf.h>
#include <lcom/timer.h>

#include <stdint.h>

#include "i8254.h"

extern unsigned long int global_timer0_counter;

int timer0_subscribe_int(uint8_t *bit_no);
int timer0_unsubscribe_int();
void timer0_int_handler();
