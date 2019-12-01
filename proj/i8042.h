#ifndef _LCOM_I8042_H_
#define _LCOM_I8042_H_

#include <lcom/lcf.h>

// Constants for programming the i8042 Keyboard/Mouse.

/** @defgroup kbd Keyboard
 *  @{
 */

/** @brief Keyboard IRQ line */
#define KBD_IRQ 1 

/** @name Time outs */
///@{
#define TIMEOUT_ATTEMPTS 5 // At a rate of 10ms per attempt, it will timeout in 50ms
#define KBC_WAIT 10000 // Will wait for 10ms
///@}

/** @name I/O Port Addresses */
///@{
#define OUT_BUF 0x60
#define IN_BUF_CMD 0x64
#define IN_BUF_ARGS 0x60
#define STAT_REG 0x64
///@}

/** @name KBD Commands */
///@{
#define READ_CMD_BYTE 0x20
#define WRITE_CMD_BYTE 0x60
#define SELF_TEST 0xAA
#define CHECK_KBD_INTERFACE 0xAB
#define ENABLE_KBD_INTERFACE 0xAD
#define DISABLE_KBD_INTERFACE 0xAE
///@}

/** @name KBD Command Byte */
///@{
#define CMD_BYTE_DISABLE_MOUSE BIT(5)
#define CMD_BYTE_DISABLE_KBD_INTERFACE BIT(4)
#define CMD_BYTE_ENABLE_INT_MOUSE BIT(1)
#define CMD_BYTE_ENABLE_INT_KBD BIT(0)
///@}

/** @name Status Related Macros
 * Used to "filter" the relevant bits from the status byte 
 */
///@{

/* Input buffer full */
#define ST_OUT_BUF BIT(0)
#define ST_IN_BUF BIT(1)

#define ST_MOUSE_DATA BIT(5) /** @brief Is the output buffer filled with mouse data */

/** @name Error selection */
///@{
#define ST_PAR_ERR BIT(7) 
#define ST_TO_ERR BIT(6)
///@}
///@}

/** @name Scancode Related Macros */
///@{
#define MAKE_TO_BREAK 0x80 /** @brief Difference between the breakcode and the makecode */
#define SCANCODE_TWO_BYTES 0xE0
///@}

/** @} end of Keyboard */

/** @defgroup mouse Mouse
 * @{
 */

#define MOUSE_IRQ 12 /**< @brief Mouse IRQ line */

/** @name PS/2 Mouse Ports */
///@{
#define MOUSE_STAT_REG 0x64
#define MOUSE_CTRL_REG 0x64
#define MOUSE_IN_BUF 0x60
#define MOUSE_OUT_BUF 0x60
///@}

/** @name Mouse Commands */
///@{
#define KBC_WRITE_BYTE_TO_MOUSE 0xD4
#define MOUSE_CMD_RESET 0xFF
#define MOUSE_CMD_DISABLE_DATA_REPORT 0XF5
#define MOUSE_CMD_ENABLE_DATA_REPORT 0xF4
#define MOUSE_CMD_REMOTE_MODE 0xF0
#define MOUSE_CMD_READ_DATA 0xEB
#define MOUSE_CMD_STREAM_MODE 0xEA
#define MOUSE_CMD_SCALING_ACCELERATION 0xE7
#define MOUSE_CMD_SCALING_LINEAR 0xE6
#define MOUSE_CMD_STATUS_REQUEST 0xE9
///@}

/** @name Mouse Controller Acknowledgements */
///@{
#define MOUSE_CTRL_ACK 0xFA
#define MOUSE_CTRL_NACK 0xFE
#define MOUSE_CTRL_ERROR 0xFC
///@}

/** @name Mouse Packet Parsing */
///@{
#define MOUSE_PARSE_LB BIT(0)
#define MOUSE_PARSE_RB BIT(1)
#define MOUSE_PARSE_MB BIT(2)
#define MOUSE_PARSE_FIRST_PACKET_IDENTIFIER BIT(3)
#define MOUSE_PARSE_X_DELTA_MSB BIT(4)
#define MOUSE_PARSE_Y_DELTA_MSB BIT(5)
#define MOUSE_PARSE_X_OVERFLOW BIT(6)
#define MOUSE_PARSE_Y_OVERFLOW BIT(7)
#define EXTEND_SIGN 0xFF00
///@}

#endif /* _LCOM_I8042_H */

/** @} end of Mouse */
