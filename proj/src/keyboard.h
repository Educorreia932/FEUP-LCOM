#include <lcom/lcf.h>
#include <lcom/lab3.h>

#include "i8042.h"
#include "utils.h"

/** @defgroup kbd Keyboard
 *  @{
 */

extern uint8_t scancode, is_scancode_complete, scancode_no_bytes;

int (kbd_subscribe_int)(uint32_t *bit_mask);
int (kbd_unsubscribe_int)();


int kbc_get_scancode();
void kbd_ih();

void analyse_scancode();

/** @brief Reenables the keyboard interrupts 
 * Avoids having to call the executable from a remote shell to restore the keyboard interrupts. 
 * This is only meant to be called when interrupts are disabled. Otherwise, the interrupt handler may "steal" our response and ruin everything
 */
int kbc_reenable_default_int();

/** @} end of Keyboard */
