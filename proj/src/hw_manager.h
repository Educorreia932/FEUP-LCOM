#pragma once

#include <lcom/lcf.h>
#include "input_events.h"

#define UART_DC_TIME (5 * 60) // 5 seconds
#define UART_CONNECTION_WARNING_TIME (UART_DC_TIME / 2) // Half the UART_DC_TIME

/* SUBSCRIBE & UNSUBCRIBE INT */

uint8_t hw_manager_subscribe_int(uint32_t *timer0_mask, uint32_t *kbd_mask, uint32_t *mouse_mask, uint32_t *rtc_mask, uint32_t *uart_mask);

void hw_manager_unsubscribe_int();

/* INT HANDLER STUFF */

void hw_manager_timer0_ih();

bool hw_manager_is_frame();

void hw_manager_kbd_ih();

void hw_manager_kbd();

void hw_manager_mouse_ih();

void hw_manager_mouse();

void hw_manager_rtc_ih();

int hw_manager_rtc_read_date_in_seconds();

void hw_manager_rtc_set_alarm(uint32_t period);

/* VIDEO WRAPPERS */

void hw_manager_switch_double_buffer();
void hw_manager_glitched_switch_double_buffer();

uint8_t hw_manager_enter_video_mode();

void hw_manager_exit_video_mode();

void hw_manager_uart_ih();
void hw_manager_uart_send_char(uint8_t to_send);
uint8_t hw_manager_uart_front();
uint8_t hw_manager_uart_pop();
bool hw_manager_uart_is_empty();
uint8_t hw_manager_uart_size();
