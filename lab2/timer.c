#include <lcom/lcf.h>
#include <lcom/timer.h>

#include <stdint.h>

#include "i8254.h"

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
  /* To be implemented by the students */
  printf("%s is not yet implemented!\n", __func__);

  return 1;
}

int (timer_subscribe_int)(uint8_t *bit_no) {
    /* To be implemented by the students */
  printf("%s is not yet implemented!\n", __func__);

  return 1;
}

int (timer_unsubscribe_int)() {
  /* To be implemented by the students */
  printf("%s is not yet implemented!\n", __func__);

  return 1;
}

void (timer_int_handler)() {
  /* To be implemented by the students */
  printf("%s is not yet implemented!\n", __func__);
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
