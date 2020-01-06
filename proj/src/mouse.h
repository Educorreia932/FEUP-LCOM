#pragma once

#include "i8042.h"
#include "utils.h"

/** @file */

/** @defgroup mouse Mouse
 * @brief Code used to interact with the Mouse 
 * @{
 */

/** @brief Indicates if an error occured in the mouse interttupt handler */
extern bool mouse_ih_error;

/** @brief Indicates if the mouse packet is complete for parsing */
extern bool is_mouse_packet_complete;

/** @brief Structure that contains the parsed content of a mouse packet */
extern struct packet mouse_parsed_packet;

/** 
 * @brief Susbcribes mouse interrupts 
 * @details Sends the bit number for the interrupt through bit_no and saves the hook id on mouse_hook_id to be used later for unsubscribing and other actions 
 * @param bit_mask Mask with Mouse's IRQ line
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
 * @details Reads the status register and the output buffer (OB).
 * @note If there was some error, the byte read from the OB should be discarded.
 */
void (mouse_ih)();

/**
 * @brief Handles the data (mouse packets) received
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
 * @brief Sends command to mouse signaling we want to read a packet 
 * @returns 0 on success, 1 otherwise
*/
uint8_t mouse_read_data();

/** @} end of Mouse */
