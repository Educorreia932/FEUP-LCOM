#ifndef _LCOM_I8042_H_
#define _LCOM_I8042_H_

#include <lcom/lcf.h>

/** @defgroup i8042 i8042
 * @{
 *
 * Constants for programming the i8042 Keyboard/Mouse.
 */


/* ---------------- Keyboard Macros ---------------- */

#define KBD_IRQ 1 /**< @brief Keyboard IRQ line */

/* Time outs */
#define TIMEOUT_ATTEMPTS 5 // At a rate of 10ms per attempt, it will timeout in 50ms
#define KBC_WAIT 10000 // Will wait for 10ms

/* I/O Port addresses */

#define OUT_BUF 0x60
#define IN_BUF_CMD 0x64
#define IN_BUF_ARGS 0x60
#define STAT_REG 0x64

/* KBD Commands */

#define READ_CMD_BYTE 0x20
#define WRITE_CMD_BYTE 0x60
#define SELF_TEST 0xAA
#define CHECK_KBD_INTERFACE 0xAB
#define ENABLE_KBD_INTERFACE 0xAD
#define DISABLE_KBD_INTERFACE 0xAE

/* KBD Command Byte */

#define CMD_BYTE_DISABLE_MOUSE BIT(5)
#define CMD_BYTE_DISABLE_KBD_INTERFACE BIT(4)
#define CMD_BYTE_ENABLE_INT_MOUSE BIT(1)
#define CMD_BYTE_ENABLE_INT_KBD BIT(0)

/* Status Related Macros */
/* Used to "filter" the relevant bits from the status byte */

/* Input buffer full */

#define ST_OUT_BUF BIT(0)
#define ST_IN_BUF BIT(1)

/* Is the output buffer filled with mouse data */

#define ST_MOUSE_DATA BIT(5)

/* Error selection: bits 7 and 6 */

#define ST_PAR_ERR BIT(7) 
#define ST_TO_ERR BIT(6)

/* Scancode Related Macros */
#define ESC_BREAKCODE 0x81
#define MAKE_TO_BREAK 0x80 /**< @brief Difference between the breakcode and the makecode */

#define SCANCODE_TWO_BYTES 0xE0

/* ---------------- Mouse Macros ---------------- */

#define MOUSE_IRQ 12 /**< @brief Mouse IRQ line */

#define CTRL_REG 0x64
#define IN_BUF 0x60

#define EXTEND_SIGN 0xFF00

/* Mouse Commands */

#define RESET 0xFF
#define DISABLE_DATA_REPORT 0XF5
#define ENABLE_DATA_REPORT 0xF4
#define REMOTE_MODE 0xF0
#define STREAM_MODE 0xEA
#define SCALING_ACCELERATION 0xE7
#define SCALING_LINEAR 0xE6

/**@}*/

#endif /* _LCOM_I8042_H */
