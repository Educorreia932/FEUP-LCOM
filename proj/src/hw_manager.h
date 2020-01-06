#pragma once

#include <lcom/lcf.h>
#include "input_events.h"

/** @file */

/** @defgroup hw_man Hardware Manager */
/** @addtogroup hw_man
  *
  * @brief Acts as an interface between most low level hardware modules and functions and the high level game
  *
  * @{
  */

/**
 * @brief The number of frames that must pass until the game assumes the other player disconnected
 * 
 */
#define UART_DC_TIME (3 * 60)
/**
 * @brief The maximum number of frames without sending information until we send a message to the other player informing we are still connected
 * 
 */
#define UART_CONNECTION_WARNING_TIME (UART_DC_TIME / 2) // Half the UART_DC_TIME

/* SUBSCRIBE & UNSUBCRIBE INT */

/**
 * @brief Subscribes to all needed interrupts
 * @details Also initializes and sets the correct configuration of the required devices\n
 * The arguments are meant to have the needed masks for the MINIX notifications
 * 
 * @param timer0_mask Mask identifying the timer 0 notifications
 * @param kbd_mask Mask identifying the keyboard notifications
 * @param mouse_mask Mask identifying the mouse notifications
 * @param rtc_mask Mask identifying the real time clock notifications
 * @param uart_mask Mask identifying the serial port notifications
 * @return uint8_t 0 on success\n
 * 1 otherwise
 */
uint8_t hw_manager_subscribe_int(uint32_t *timer0_mask, uint32_t *kbd_mask, uint32_t *mouse_mask, uint32_t *rtc_mask, uint32_t *uart_mask);

/**
 * @brief Unsubscribes all the interrupts and resets the configuration of the needed devices
 * 
 */
void hw_manager_unsubscribe_int();

/* INT HANDLER STUFF */

/**
 * @brief Wrapper for timer0 interrupt handler (ih)
 * 
 */
void hw_manager_timer0_ih();
/**
 * @brief Returns wether there should currently be a frame or not
 * 
 * @return True when a frame should occur
 * False otherwise
 */
bool hw_manager_is_frame();

/**
 * @brief Wrapper for the keyboard interrupt handler (ih)
 * 
 */
void hw_manager_kbd_ih();
/**
 * @brief Processes the data read by the keyboard ih
 * 
 */
void hw_manager_kbd();

/**
 * @brief Wrapper for the mouse interrupt handler (ih)
 * 
 */
void hw_manager_mouse_ih();
/**
 * @brief Processes the data read by the mouse ih
 * 
 */
void hw_manager_mouse();

/**
 * @brief Wrapper for the real time clock interrupt handler (ih)
 * 
 */
void hw_manager_rtc_ih();

/* RTC WRAPPERS */

/**
 * @brief Returns the current date measured in seconds
 * 
 * @return int The current date measured in seconds
 */
int hw_manager_rtc_read_date_in_seconds();

/**
 * @brief Sets an alarm on the real time clock for a 'period' ammount of seconds
 * 
 * @param period The number of seconds until the alarm should go off
 */
void hw_manager_rtc_set_alarm(uint32_t period);

/* VIDEO WRAPPERS */

/**
 * @brief Sends the content currently present on the buffer to the actual frame buffer
 * @details Yes, this is what makes double buffering work
 * 
 */
void hw_manager_switch_double_buffer();
/**
 * @brief Same as hw_manager_switch_double_buffer(), but applies a warped, glitchy effect
 * @details Used for the switchboard "failure penalty" 
 */
void hw_manager_glitched_switch_double_buffer();

/**
 * @brief Makes MINIX enter video mode with the mode selected for our project
 * @details Currently enters video mode 0x117, boasting 1024x768 resolution, with RBG values 565. Our project's rendering functions have been highly optimised for this mode 
 * 
 * @return uint8_t 0 on success\n
 * 1 otherwise
 */
uint8_t hw_manager_enter_video_mode();

/**
 * @brief Exits video mode and reenters text mode
 * 
 */
void hw_manager_exit_video_mode();

/* UART WRAPPERS */

/**
 * @brief Wrapper for serial port interrupt handler (ih)
 * 
 */
void hw_manager_uart_ih();
/**
 * @brief Sends a byte through the serial port
 * 
 * @param to_send 
 */
void hw_manager_uart_send_char(uint8_t to_send);

/**
 * @brief Clear all data from the receiver Queue
 * 
 */
void hw_manager_uart_clear();
/**
 * @brief Returns the byte at the front of the data receiver Queue
 * 
 * @return uint8_t The value at the front of the receiver Queue on success\n
 * 0xFF otherwise
 */
uint8_t hw_manager_uart_front();
/**
 * @brief Returns and pops the byte at the front of the data receiver Queue
 * 
 * @return uint8_t uint8_t The value at the front of the receiver Queue on success\n
 * 0xFF otherwise
 */
uint8_t hw_manager_uart_pop();
/**
 * @brief Returns true when there is no information in the receiver Queue
 * 
 * @return True when the receiver Queue is empty\n
 * False otherwise
 */
bool hw_manager_uart_is_empty();
/**
 * @brief Returns the current size of the receiver Queue
 * 
 * @return uint8_t The current size of the receiver Queue
 */
uint8_t hw_manager_uart_size();

/** @} */
