#pragma once

#include <lcom/lcf.h>
#include <lcom/timer.h>

#include "i8254.h"

extern unsigned long int global_timer0_counter;

int timer0_subscribe_int(uint32_t *bit_mask);
int timer0_unsubscribe_int();
void timer0_int_handler();
