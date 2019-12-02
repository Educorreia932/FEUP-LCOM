#pragma once

#include <lcom/lcf.h>

typedef struct {
	uint16_t seconds, minutes, hour;
} date_t;

#define RTC_IRQ 8 /** @brief RTC IRQ line */
#define RTC_ADDR_REG 0x70 /** @brief RTC Address Port **/
#define RTC_DATA_REG 0x71 /** @brief RTC Data Port **/

/*! @name RTC Address Related */
///@{
#define RTC_REG_SECONDS 0
#define RTC_REG_A_SECONDS 1
#define RTC_REG_MINUTES 2
#define RTC_REG_A_MINUTES 3
#define RTC_REG_HOURS 4
#define RTC_REG_A_HOURS 5
#define RTC_REG_DAY 7
#define RTC_REG_MONTH 8
#define RTC_REG_YEAR 9
#define RTC_REG_A 10
#define RTC_REG_B 11
#define RTC_REG_C 12
#define RTC_REG_D 13
///@}

/*! @name Interrupt bits */
///@{
#define RTC_UIE BIT(4)
#define RTC_AIE BIT(5)
#define RTC_PIE BIT(6)
///@}

date_t get_date(); 

int rtc_enable_alarm_int();

int rtc_int(uint16_t period);

void rtc_ih();

void rtc_set_alarm(uint32_t period);

int rtc_subscribe_int();

int rtc_unsubscribe_int();

int rtc_read_register(uint32_t addr, uint32_t* data);
int rtc_write_register(uint32_t addr, uint32_t data);
