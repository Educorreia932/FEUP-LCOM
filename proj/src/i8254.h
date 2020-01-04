#ifndef _LCOM_I8254_H_
#define _LCOM_I8254_H_

#include <lcom/lcf.h>

// Constants for programming the i8254 Timer. 

/** @file i8254.h */

/** @addtogroup timer 
 *  @{
 */

/** @brief clock frequency for timer in PC and AT */
#define TIMER_FREQ 1193182 
/** @brief Timer 0 IRQ line */
#define TIMER0_IRQ 0

/** @name I/O port addresses */
/** @{ */
#define TIMER_0    0x40 /**< @brief Timer 0 count register */
#define TIMER_1    0x41 /**< @brief Timer 1 count register */
#define TIMER_2    0x42 /**< @brief Timer 2 count register */
#define TIMER_CTRL 0x43 /**< @brief Control register */

#define SPEAKER_CTRL 0x61 /**< @brief Register for speaker control  */
/** @} */

/** @name Timer control 
 *  @{ 
 */
#define TIMER_SEL0   0x00              /**< @brief Control Word for Timer 0 */
#define TIMER_SEL1   BIT(6)            /**< @brief Control Word for Timer 1 */
#define TIMER_SEL2   BIT(7)            /**< @brief Control Word for Timer 2 */
#define TIMER_RB_CMD (BIT(7) | BIT(6)) /**< @brief Read Back Command */
#define TIMER_LSB     BIT(4)                  /**< @brief Initialize Counter LSB only */
#define TIMER_MSB     BIT(5)                  /**< @brief Initialize Counter MSB only */
#define TIMER_LSB_MSB (TIMER_LSB | TIMER_MSB) /**< @brief Initialize LSB first and MSB afterwards */
#define TIMER_SQR_WAVE (BIT(2) | BIT(1)) /**< @brief Mode 3: square wave generator */
#define TIMER_RATE_GEN BIT(2)            /**< @brief Mode 2: rate generator */
#define TIMER_BCD 0x01 /**< @brief Count in BCD */
#define TIMER_BIN 0x00 /**< @brief Count in binary */
/** @} */

/** @name Read-Back Command Format 
 * @{ 
 */
/** @brief Retrieve Timer Counting Value */
#define TIMER_RB_COUNT_  BIT(5) 
/** @brief Retrieve Timer Status */
#define TIMER_RB_STATUS_ BIT(4)
/** @brief Timer Selection */
#define TIMER_RB_SEL(n) BIT((n) + 1)
/** @} */

/** @name Status Related Macros 
 * Used to "filter" the relevant bits from the status byte
*/
///@{
/** @brief Outpu */
#define TIMER_STATUS_OUTPUT BIT(7)
/** @brief Null Count */
#define TIMER_STATUS_NULL_COUNT BIT(6)
/** @brief Counter Initialiaztion */
#define TIMER_STATUS_TYPE_OF_ACCESS (BIT(5) | BIT(4))
/** @brief Timer Programmed Mode */
#define TIMER_STATUS_PROGRAMMED_MODE (BIT(3) | BIT(2) | BIT(1))
/** @brief Indicates whether the counting mode is in BCD or note */
#define TIMER_STATUS_BCD BIT(0)
///@}

#endif /* _LCOM_I8254_H */

/** @} end of Timer */
