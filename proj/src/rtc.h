#pragma once

#include <lcom/lcf.h>

/** @file rtc.h */

/** @defgroup rtc RTC
 * @brief Code used to interact with the Real Time Clock 
 * @{ 
 */

/** 
 * @brief Stores information regarding time, namely, current seconds, minutes and hour
 */
typedef struct {
	uint16_t seconds, minutes, hour;
} date_t;

/** @brief Update time in microseconds */
#define RTC_WAIT_UIP 244 

/** @brief RTC IRQ line */
#define RTC_IRQ 8 
/** @brief RTC Address Port **/
#define RTC_ADDR_REG 0x70 
/** @brief RTC Data Port **/
#define RTC_DATA_REG 0x71 

/*! @name RTC Address Related */
///@{
/** @brief Current seconds */
#define RTC_REG_SECONDS 0
/** @brief Seconds alarm */
#define RTC_REG_A_SECONDS 1
/** @brief Current minutes */
#define RTC_REG_MINUTES 2
/** @brief Minutes alarm */
#define RTC_REG_A_MINUTES 3
/** @brief Current hours */
#define RTC_REG_HOURS 4
/** @brief Hours alarm */
#define RTC_REG_A_HOURS 5
/** @brief Current day */
#define RTC_REG_DAY 7
/** @brief Current month */
#define RTC_REG_MONTH 8
/** @brief Current year */
#define RTC_REG_YEAR 9
/** @brief Register A */
#define RTC_REG_A 10
/** @brief REgister B */
#define RTC_REG_B 11
/** @brief Register C */
#define RTC_REG_C 12
/** @brief Register D */
#define RTC_REG_D 13
///@}

/*! @name Interrupt bits */
///@{
/** @brief Enable Update Interrutps */
#define RTC_UIE BIT(4)
/** @brief Enable Alarm Interrupts */
#define RTC_AIE BIT(5)
/** @brief Enable Periodic Interrupts */
#define RTC_PIE BIT(6)
///@}

/**
 * @brief Reads the current date from the RTC
 *  
 * @returns A Date object containing hour, minute and second
 */
date_t get_date(); 

/**
 * @brief Enables RTC alarm interrupts
 * 
 * @returns 0 on success, 1 otherwise
 */
int rtc_enable_alarm_int();

/**
 * @brief Handles RTC interrupts
 */
void rtc_ih();

/** 
 * @brief Sets an RTC alarm
 * 
 * @param period Period in seconds in which the alarm must be set counting from the moment it's called */
void rtc_set_alarm(uint32_t period);

/** 
 * @brief Subscribes and enables RTC interrupts. 
 * @details Sends the bit number for the interrupt through bit_no and saves the hook id on rtc_hook_id to be used later for unsubscribing and other actions 
 * @param bit_mask Mask with RTC's IRQ line
 * @returns 0 on sucess, 1 otherwise
 */
int rtc_subscribe_int(uint32_t *bit_mask);

/** 
 * @brief Unsubscribes RTC interrupts.
 * @returns 0 upon success, 1 otherwise
 */
int rtc_unsubscribe_int();

/**
 * @brief Reads a RTC register from memory
 * @param addr The port that is to be read
 * @param data Address of the 32-bit variable to be update with the value read
 */
int rtc_read_register(uint32_t addr, uint32_t* data);

/**
 * @brief Writes a RTC register in memory
 * @param addr The port that is to be written
 * @param data Address of the 32-bit variable to be written
 */
int rtc_write_register(uint32_t addr, uint32_t data);

/** @} */
