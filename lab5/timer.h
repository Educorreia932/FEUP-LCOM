#pragma once

#include <lcom/lcf.h>
#include <lcom/timer.h>

#include <stdint.h>

#include "i8254.h"

extern unsigned long int global_timer0_counter;

// LCF takes care of these ones
// int timer_subscribe_int(uint8_t *bit_no);
// int timer_unsubscribe_int();
// void timer_int_handler();
