#ifndef _LCOM_I8042_H_
#define _LCOM_I8042_H_

#include <lcom/lcf.h>

/** @file i8042.h */

/** @addtogroup kbd
 * 
 * @{
 */

#define KBD_IRQ 1 /**< @brief Keyboard IRQ line */

/** @name Timeouts */
///@{
#define TIMEOUT_ATTEMPTS 5 /**< @brief At a rate of 10ms per attempt, it will timeout in 50ms */
#define KBC_WAIT 10000 /**< @brief Will wait for 10ms */
///@}

/** @name I/O Port Addresses */
///@{
#define OUT_BUF 0x60 /**< @brief Output buffer port */
#define IN_BUF_CMD 0x64 /**< @brief Input buffer port */
#define IN_BUF_ARGS 0x60 /**< @brief Input buffer arguments */
#define STAT_REG 0x64 /**< @brief Status register*/
///@}

/** @name KBD Commands */
///@{
#define READ_CMD_BYTE 0x20 /**< @brief Read command byte */
#define WRITE_CMD_BYTE 0x60 /**< @brief Write command byte */
#define SELF_TEST 0xAA /**< @brief Check KBC (Self-test) */
#define CHECK_KBD_INTERFACE 0xAB /**< @brief Check keyboard interface */
#define ENABLE_KBD_INTERFACE 0xAD /**< @brief Enable keyboard interface */
#define DISABLE_KBD_INTERFACE 0xAE /**< @brief Disable keyboard interface */
///@}

/** @name KBD Command Byte */
///@{
#define CMD_BYTE_DISABLE_MOUSE BIT(5) /**< @brief Disable mouse */
#define CMD_BYTE_DISABLE_KBD_INTERFACE BIT(4) /**< @brief Disable keyboard interface */
#define CMD_BYTE_ENABLE_INT_MOUSE BIT(1) /**< @brief Enable mouse interrupts */
#define CMD_BYTE_ENABLE_INT_KBD BIT(0) /**< @brief Enable keyboard interrupts */
///@}

/** @name Status Related Macros
 * @brief Used to "filter" the relevant bits from the status byte 
 * @{ 
 */

 /** @brief Output buffer full */
#define ST_OUT_BUF BIT(0)
 /** @brief Input buffer full */
#define ST_IN_BUF BIT(1)
/** @brief Indicates wheter the output buffer is filled with mouse data or not*/
#define ST_MOUSE_DATA BIT(5) 
/** @brief Parity Error */
#define ST_PAR_ERR BIT(7) 
/** @brief Timeout Error */
#define ST_TO_ERR BIT(6)

/** @name Scancode Related Macros */
///@{
/** @brief Difference between the breakcode and the makecode */
#define MAKE_TO_BREAK 0x80 
/** @brief First byte of a scancode that consists of two bytes */
#define SCANCODE_TWO_BYTES 0xE0 
///@}

/** @} */

/** @addtogroup mouse
 * @{
 */

#define MOUSE_IRQ 12 /**< @brief Mouse IRQ line */

/** @name PS/2 Mouse Ports 
 * @{
*/
/** @brief Status Register */
#define MOUSE_STAT_REG 0x64
/** @brief Control Register */
#define MOUSE_CTRL_REG 0x64
/** @brief Input buffer port */
#define MOUSE_IN_BUF 0x60
/** @brief Output buffer port */
#define MOUSE_OUT_BUF 0x60
/** @} */

/** @name Mouse Commands */
///@{
/** @brief Write byte to mouse */
#define KBC_WRITE_BYTE_TO_MOUSE 0xD4
/** @brief Resets the mouse */
#define MOUSE_CMD_RESET 0xFF
/** @brief Disables mouse data reporting */
#define MOUSE_CMD_DISABLE_DATA_REPORT 0XF5
/** @brief Enables mouse data reporting */
#define MOUSE_CMD_ENABLE_DATA_REPORT 0xF4
/** @brief Sets the mouse in remote mode (data is only sent on a request) */
#define MOUSE_CMD_REMOTE_MODE 0xF0
/** @brief Sends a request to get a data packet */
#define MOUSE_CMD_READ_DATA 0xEB
/** @brief Sets the mouse in stream mode (data is sent on events) */
#define MOUSE_CMD_STREAM_MODE 0xEA
/** @brief Sets the mouse in accelaration mode */
#define MOUSE_CMD_SCALING_ACCELERATION 0xE7
/** @brief Sets the mouse in linear mode */
#define MOUSE_CMD_SCALING_LINEAR 0xE6
/** @brief Gets the mouse configuration */
#define MOUSE_CMD_STATUS_REQUEST 0xE9
///@}

/** @name Mouse Controller Acknowledgements */
///@{
/** @brief Everything was fine */
#define MOUSE_CTRL_ACK 0xFA
/** @brief One invalid byte */
#define MOUSE_CTRL_NACK 0xFE
/** @brief Second consecutive invalid byte */
#define MOUSE_CTRL_ERROR 0xFC
///@}

/** @name Mouse Packet Parsing */
///@{
/** @brief Left Button */
#define MOUSE_PARSE_LB BIT(0)
/** @brief Right Button */
#define MOUSE_PARSE_RB BIT(1)
/** @brief Middle Button */
#define MOUSE_PARSE_MB BIT(2)
/** @brief Always set as 1 in the first byte of a packet */
#define MOUSE_PARSE_FIRST_PACKET_IDENTIFIER BIT(3)
/** @brief Sign of the X Delta (1 if negative, 0 if positive) */
#define MOUSE_PARSE_X_DELTA_MSB BIT(4)
/** @brief Sign of the Y Delta (1 if negative, 0 if positive) */
#define MOUSE_PARSE_Y_DELTA_MSB BIT(5)
/** @brief Indicates if the X displacement overflowed */
#define MOUSE_PARSE_X_OVERFLOW BIT(6)
/** @brief Indicates if the Y displacement overflowed */
#define MOUSE_PARSE_Y_OVERFLOW BIT(7)
/** @brief Signal extension in case either X or Y's MSB is 1 */
#define EXTEND_SIGN 0xFF00
///@}

#endif /* _LCOM_I8042_H */

/** @} */
