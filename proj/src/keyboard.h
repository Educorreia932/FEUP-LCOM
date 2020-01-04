#include <lcom/lcf.h>
#include <lcom/lab3.h>

#include "i8042.h"
#include "utils.h"

/** @file keyboard.h */

/** @defgroup kbd Keyboard
 * @brief Code used to interact with the Keyboard
 * @details Imported from Lab3
 * @{
 */

/** @brief Scancode that was read */
extern uint8_t scancode;
/** @brief Indicates if the scancode is already complete or not (some scancodes require two bytes) */
extern uint8_t is_scancode_complete;
/** @brief Number of bytes of a scancode (1 or 2) */
extern uint8_t scancode_no_bytes;

/**
 * @brief Subscribes keyboard interrupts 
 * Sends the bit number for the interrupt through bit_no and saves the hook id on kbd_hook_id to be used later for unsubscribing and other actions.
 * @param bit_mask Mask with Keyboard's IRQ line
 * @return 0 on success, 1 otherwise
*/
int (kbd_subscribe_int)(uint32_t *bit_mask);

/** 
 * @brief Unsubscribes keyboard interrupts 
 * @returns 0 on success, 1 otherwise
 */
int (kbd_unsubscribe_int)();

/** @brief Reads a scancode from the kbc output buffer
 * @note Supports 2 byte scancodes, but it will wait for the second call to recognize the full scancode
 * @returns 0 upon success, 1 otherwise
 */
int kbc_get_scancode();

/** 
 * @brief Handles Keyboard interrupts
 */
void kbd_ih();

/** 
 * @brief It's sole purpose is to parse both 1 & 2 byte scancodes 
 */
void analyse_scancode();

/** @brief Reenables the keyboard interrupts 
 * Avoids having to call the executable from a remote shell to restore the keyboard interrupts. 
 * This is only meant to be called when interrupts are disabled. Otherwise, the interrupt handler may "steal" our response and ruin everything
 * @returns 0 upon success, 1 otherwise
 */
int kbc_reenable_default_int();

/** @} end of Keyboard */
