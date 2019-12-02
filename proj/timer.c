#include <lcom/lcf.h>
#include <lcom/timer.h>

#include "timer.h"

/** @defgroup timer Timer
 *  @{
 */

static int timer0_hook_id;
unsigned long int global_timer0_counter = 0;

/** @brief Sets the corresponding timer bits on the control word */
int timer_set_control_word_timer(uint8_t timer, uint8_t *cmd) {
  if (!cmd) // Check if pointer is NULL
    return 1;

  switch (timer) {
    case 0:
      *cmd |= TIMER_SEL0;
      return 0;
    case 1:
      *cmd |= TIMER_SEL1;
      return 0;
    case 2:
      *cmd |= TIMER_SEL2;
      return 0;
    default:
      return 1;
  }
}

/** @brief Sets timer_port to the correct port of the timer
 * @returns 0 if timer is between 0 and 2, returns 1 otherwise */
int timer_get_port(uint8_t timer, uint8_t *timer_port) {
	if (!timer_port) // Check if pointer is NULL
		return 1;

  	switch (timer)  {
		case 0:
		*timer_port = TIMER_0;
		return 0;
		case 1:
		*timer_port = TIMER_1;
		return 0;
		case 2:
		*timer_port = TIMER_2;
		return 0;
		default:
		return 1;
	}
}

int (timer_set_frequency)(uint8_t timer, uint32_t freq) {
    // Freq lower than 19 would get a div larger than 16 bit -> Error
    // Freq higher than TIMER_FREQ (the clock) it is impossible -> Error
    if (freq < 19 || freq > TIMER_FREQ)
      return 1;

  // "Thus, in mode 3, the timer generates a square wave with a frequency given by the expression clock/div, where clock is the frequency of the Clock input and div is the value loaded initially in the timer."
  // TIMER_FREQ is clock on that equation
  // div is Time-Base
  uint16_t div = TIMER_FREQ / freq;

  uint8_t cmd; // Will initially have Status
  timer_get_conf(timer, &cmd); // Loads status to cmd

  // Saves the 4 least significant bits to preserve mode (3) and counting (BCD/Binary)
  cmd &= 0x0F;

  // Sets the correct timer on the control word
  if (timer_set_control_word_timer(timer, &cmd))
    return 1;
  
  // Sets LSB followed by MSB on the control word
  cmd |= TIMER_LSB_MSB;

  // Gets the timer port
  uint8_t timer_port;
  if (timer_get_port(timer, &timer_port))
    return 1;

  // Gets the LSB and MSB of the div
  uint8_t lsb, msb;
  if (util_get_LSB(div, &lsb))
    return 1;
  if (util_get_MSB(div, &msb))
    return 1;

  // Sends control word
  if (sys_outb(TIMER_CTRL, cmd))
    return 1;

  //Send the div to get the frequency right
  if (sys_outb(timer_port, lsb))
    return 1;
  if (sys_outb(timer_port, msb))
    return 1;

  return 0;
}

/** 
 * @brief Subscribes and enables Timer 0 interrupts. 
 * @details Sends the bit number for the interrupt through bit_no and saves the hook id on timer0_hook_id to be used later for unsubscribing and other actions 
 */
int (timer_subscribe_int)(uint8_t *bit_no) {

  if (!bit_no) // Check if pointer is NULL
    return 1;

  *bit_no = TIMER0_IRQ;
  timer0_hook_id = TIMER0_IRQ;
  global_timer0_counter = 0; // Reset time counter

  if (sys_irqsetpolicy(TIMER0_IRQ, IRQ_REENABLE, &timer0_hook_id) != OK)
    return 1;

  return 0;
}

/** 
 * @brief Unsubscribes Timer 0 interrupts.
 * @returns 0 upon success, 1 otherwise
 */
int (timer_unsubscribe_int)() {
	return sys_irqrmpolicy(&timer0_hook_id) != OK;
}

/** @brief Increments the number of times that the timer sent an interrupt */
void (timer_int_handler)() {
  ++global_timer0_counter;
}

/** 
 * @brief Reads the input timer configuration (status) via read-back command.
 * @param timer Timer whose configuration to read (Ranges from 0 to 2)
 * @param st Address of memory position to be filled with the timer config
 * @returns 0 upon success, 1 otherwise
 */
int (timer_get_conf)(uint8_t timer, uint8_t *st) {
	if (!st) // Checks if pointer is NULL
		return 1;

	uint8_t timer_port;
	// Need to set the RB_COUNT to 1, otherwise the status will not be correct
	uint8_t cmd = (uint8_t) (TIMER_RB_SEL(timer) |
		TIMER_RB_CMD | TIMER_RB_COUNT_);
	
	if (timer_get_port(timer, &timer_port))
		return 1;
	
	if (sys_outb(TIMER_CTRL, cmd))
		return 1;
	
	return(util_sys_inb(timer_port, st));
}
