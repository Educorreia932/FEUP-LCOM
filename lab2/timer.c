#include <lcom/lcf.h>
#include <lcom/timer.h>

#include <stdint.h>

#include "i8254.h"

static int timer0_hook_id;
unsigned int global_counter;

// Sets the corresponding timer bits on the control word
int timer_set_control_word_timer(uint8_t timer, uint8_t *cmd)
{
  switch (timer)
  {
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

// Sets timer_port to the correct port of the timer
// Returns 0 if timer is between 0 and 2, returns 1 otherwise
int timer_get_port(uint8_t timer, uint8_t *timer_port)
{
  switch (timer)
  {
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

// bit_no = bit a 1 do hook_id 
int (timer_subscribe_int)(uint8_t *bit_no) {

  if (sys_irqsetpolicy(TIMER0_IRQ, IRQ_REENABLE, &timer0_hook_id) != OK)
    return 1;

  *bit_no = 0;
  int aux = timer0_hook_id;
  // aka while (aux != 0)
  while (aux)
  {
    ++bit_no;
    aux >>= 1;
  }

  // bit_no must be in [0, 31]
  if (*bit_no > 31)
    return 1;

  // Should not be necessary, but here it is anyway
  // if (sys_irqenable(bit_no) != OK)
  //   return 1;

  global_counter = 0;

  return 0;
}

int (timer_unsubscribe_int)() {
  if (sys_irqrmpolicy(&timer0_hook_id) != OK)
    return 1;
  return 0;
}

void (timer_int_handler)() {
  ++global_counter;
}


int (timer_get_conf)(uint8_t timer, uint8_t *st) {
  uint8_t timer_port;
  // Need to set the RB_COUNT to 1, otherwise the status will not be correct
  uint8_t cmd = (uint8_t) (0x00 | TIMER_RB_SEL(timer) |
    TIMER_RB_CMD | TIMER_RB_COUNT_);
  
  if (timer_get_port(timer, &timer_port))
    return 1;
  
  if (sys_outb(TIMER_CTRL, cmd))
    return 1;
  
  return(util_sys_inb(timer_port, st));
}



int (timer_display_conf)(uint8_t timer, uint8_t st,
                        enum timer_status_field field) {
  
  // Check if timer is within range
  if (timer > 2)
    return 1;

  union timer_status_field_val field_val;
  if (field == 0) // aka "all"
  {
    field_val.byte = st;

  } else if (field == 1) // aka counting "mode" (0 - 5)
  {
    st = (st & TIMER_STATUS_TYPE_OF_ACCESS) >> 4;
    switch (st)
    {
      case 0:
        field_val.in_mode = INVAL_val; break;
      case 1:
        field_val.in_mode = LSB_only; break;
      case 2:
        field_val.in_mode = MSB_only; break;
      case 3:
        field_val.in_mode = MSB_after_LSB; break;
      default:
        return 1;
    }

  } else if (field == 2) // aka "init"ialization mode (LSB and MSB related)
  {
    // >> 1 Because the programmed mode is in binary representraion,
    // but on the bits 1, 2 and 3 (if the TIMER_STATUS_PROGRAMMED_MODE is unchanged)
    field_val.count_mode = (st & TIMER_STATUS_PROGRAMMED_MODE) >> 1;
    // If mode = {2, 3} it would read {6, 7}, so this fixes that
    if (field_val.count_mode & BIT(1))
    {
      field_val.count_mode &= ~BIT(2); // Sets BIT 2 to 0
    }

  } else if (field == 3) // aka counting "base"
  {
    if (st & TIMER_STATUS_BCD)
      field_val.bcd = true;
    else
      field_val.bcd = false;
  } else
      return 1;

  return timer_print_config(timer, field, field_val);
}
