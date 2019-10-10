#ifndef _LCOM_I8042_H_
#define _LCOM_I8042_H_

#include <lcom/lcf.h>

/** @defgroup i8042 i8042
 * @{
 *
 * Constants for programming the i8254 Keyboard. Needs to be completed.
 */

#define KBD_IRQ 1 /**< @brief Keyboard IRQ line */
#define WAIT_KBC 20000

/* I/O Port addresses */

#define OUT_BUF 0x60
#define IN_BUF 0x64
#define STAT_REG 0x64
#define KBC_CMD_REG 0x64

/* Status Related Macros */
/* Used to "filter" the relevant bits from the status byte */

/* Error selection: bits 7 and 6 */

#define PAR_ERR BIT(7)
#define TO_ERR BIT(6)

//#define OBF // ?

/* Scancode Related Macros */
#define ESC_BREAKCODE 0x81
#define MAKE_TO_BREAK 0x80 /**< @brief Difference between the breakcode and the makecode */

/**@}*/

#endif /* _LCOM_I8042_H */
