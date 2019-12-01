#include <lcom/lcf.h>
#include <lcom/lab3.h>

#include "i8042.h"
#include "utils.h"

/** @defgroup kbd Keyboard
 *  @{
 */

extern uint8_t scancode, is_scancode_complete, scancode_no_bytes;

int (kbd_subscribe_int)(uint8_t *bit_no);

int (kbd_unsubscribe_int)();

/** @brief Reads a scancode from the kbc output buffer
 * Supports 2 byte scancodes, but it will wait for the second call to recognize the full scancode
 */
int kbc_get_scancode();
void kbd_ih();

/** @brief It's sole purpose is to parse both 1 & 2 byte scancodes */
void analyse_scancode();

/** @brief Reenables the keyboard interrupts 
 * Avoids having to call the executable from a remote shell to restore the keyboard interrupts. 
 * This is only meant to be called when interrupts are disabled. Otherwise, the interrupt handler may "steal" our response and ruin everything
 */
int kbc_reenable_default_int();

/** @} end of Keyboard */
