#include "rtc.h"
#include "utils.h"

static int rtc_hook_id;

int bcd_to_dec (uint16_t bcd) {
	unsigned short MS, LS;
	MS = (bcd >> 4);
	LS = (bcd & 0x0f);

	if (!MS) 
		return LS;
	
	else if (MS < 8) 
		return 10 * MS + LS;
	
	else if (MS == 8) 
		return 12 + LS;
	
	else 
		return 22 + LS;
}

uint16_t dec_to_bcd (uint16_t dec) {
	return ((dec / 10) << 4) + (dec % 10);
}

int rtc_read_register(uint32_t addr, uint32_t* data) {
	if(sys_outb(RTC_ADDR_REG, addr)) 
    	return 1;

  	if(sys_inb(RTC_DATA_REG, data))
    	return 1;
  
  	return 0;
}

int rtc_write_register(uint32_t addr, uint32_t data) {
	if (sys_outb(RTC_ADDR_REG, addr))
		return 1;

	if (sys_outb(RTC_DATA_REG, data))
		return 1;

  	return 0;
}

date_t get_date() {
	date_t t;
	uint32_t data;

	rtc_read_register(RTC_REG_A, &data);

	if (data & RTC_WAIT_UIP)
		tickdelay(micros_to_ticks(RTC_WAIT_UIP));

	rtc_read_register(RTC_REG_SECONDS, &data);
	t.seconds = bcd_to_dec(data);

	rtc_read_register(RTC_REG_MINUTES, &data);
	t.minutes = bcd_to_dec(data);

	rtc_read_register(RTC_REG_HOURS, &data);
	t.hour = bcd_to_dec(data);

	return t;
}

int (rtc_subscribe_int)(uint32_t *bit_mask) {
	if (!bit_mask) // Check if pointer is NULL
		return 1;

	rtc_hook_id = RTC_IRQ;
	*bit_mask = BIT(RTC_IRQ);

	if (sys_irqsetpolicy(RTC_IRQ, IRQ_REENABLE, &rtc_hook_id))
		return 1;

	return 0;
}

/** 
 * @brief Unsubscribes the RTC interrupts
 */
int (rtc_unsubscribe_int)() {
	return sys_irqrmpolicy(&rtc_hook_id);
}

void rtc_ih() {
	int cause;
	uint32_t regA;

	sys_outb(RTC_ADDR_REG, RTC_REG_C);
	cause = sys_inb(RTC_DATA_REG, &regA);
	
	// Alarm interrupts
	if (cause & RTC_AIE)
		printf("alarm");

	// Update interrupts
	if (cause & RTC_UIE)
		printf("update");

	// Periodic interrupts
	if (cause & RTC_PIE)
		printf("periodic");
}

int rtc_enable_alarm_int() {
	uint32_t data;
	
	if (rtc_read_register(RTC_REG_B, &data))
		return 1;

	data |= RTC_AIE;

	if (rtc_write_register(RTC_REG_B, data))
		return 1;

	return 0;
}

int rtc_disable_alarm_int() {
	uint32_t data;

	if (rtc_read_register(RTC_REG_B, &data))
		return 1;

	data &= ~RTC_AIE;

	if (rtc_write_register(RTC_DATA_REG, data))
		return 1;;

	return 0;
}

void rtc_set_alarm(uint32_t period) {
	date_t t = get_date();

	t.seconds += period;
	t.minutes += t.seconds / 60;
	t.seconds = t.seconds % 60;
	t.hour += t.minutes / 60;
	t.minutes = t.minutes % 60;
	t.hour = t.hour % 24;

	rtc_write_register(RTC_REG_A_SECONDS, dec_to_bcd(t.seconds));
	rtc_write_register(RTC_REG_A_MINUTES, dec_to_bcd(t.minutes)); 
	rtc_write_register(RTC_REG_A_HOURS, dec_to_bcd(t.hour));

	rtc_enable_alarm_int();

	printf("Alarm set to: %x:%x:%x\n", dec_to_bcd(t.hour), dec_to_bcd(t.minutes), dec_to_bcd(t.seconds));
}
