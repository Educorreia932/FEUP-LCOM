#pragma once

#include <lcom/lcf.h>
#include <lcom/timer.h>

#include "i8254.h"

/** @file timer.h */

/** @defgroup timer Timer
 * @brief Code used to interact with the Timer0
 * @details Imported from Lab2
 * @{
 */

/** @brief Timer 0 Couting Value */
extern unsigned long int global_timer0_counter;

/** 
 * @brief Subscribes and enables Timer 0 interrupts. 
 * @details Sends the bit number for the interrupt through bit_no and saves the hook id on timer0_hook_id to be used later for unsubscribing and other actions 
 * @returns 0 on sucess, 1 otherwise
 */
int timer0_subscribe_int(uint32_t *bit_mask);

/** 
 * @brief Unsubscribes Timer 0 interrupts.
 * @returns 0 upon success, 1 otherwise
 */
int timer0_unsubscribe_int();

/** 
 * @brief Handles Timer 0 interrupts
 * @details Increments the number of times that the timer sent an interrupt 
 */
void timer0_int_handler();

/** @} */
