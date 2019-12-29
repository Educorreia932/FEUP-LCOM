#pragma once

#include "i8042.h"
#include "utils.h"

/** @defgroup mouse Mouse
 * @{
 */

extern bool mouse_ih_error, is_mouse_packet_complete;
extern struct packet mouse_parsed_packet;

/** 
 * @brief Susbcribes mouse interrupts 
 * @returns 0 on success, non-zero otherwise
 */
uint8_t (mouse_subscribe_int)(uint32_t *bit_mask);

/**
 * @brief Unsubcribes mouse interrupts
 * @returns 0 on success, non-zero otherwise
 */
uint8_t (mouse_unsubscribe_int)();

/** 
 * @brief Enables mouse interrupts 
 * @returns 0 on success, 1 otherwise
*/
uint8_t mouse_enable_int();

/** 
 * @brief Disables mouse interrupts 
 * @returns 0 on success, 1 otherwise
*/
uint8_t mouse_disable_int();

/** 
 * @brief Handles mouse interrutps.
 * Reads the status register and the output buffer (OB).
 * @note If there was some error, the byte read from the OB should be discarded.
 */
void (mouse_ih)();

/** 
 * @returns 0 on success, 1 otherwise
 */ 
uint8_t (mouse_data_handler)();

/** 
 * @brief Enables stream mode data reporting by sending the respective command to the mouse.
 * @returns 0 on success, 1 otherwise
*/
uint8_t mouse_data_reporting_enable();

/** 
 * @brief Disables mouse data reporting 
 * @returns 0 on success, 1 otherwise
*/
uint8_t mouse_data_reporting_disable();

/** 
 * @brief Sets mouse to stream mode 
 * @returns 0 on success, 1 otherwise
*/
uint8_t mouse_set_stream_mode();

/** 
 * @brief Sends cmd to mouse signaling we want to read a packet 
 * @returns 0 on success, 1 otherwise
*/
uint8_t mouse_read_data();

/** @} end of Mouse */
