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

int rtc_int(uint16_t period) {
	// TODO: Read register C to see if there's any pending interrupt

	date_t initial_time = get_date();

	printf("Initial time: %u:%u:%u\n", initial_time.hour, initial_time.minutes, initial_time.seconds);

	rtc_set_alarm(period);
        
	// Only avoids making this operation on every notification
	uint32_t rtc_bit_mask;

	if (rtc_subscribe_int(&rtc_bit_mask)) 
    	return 1;

	int r, ipc_status;
	message msg;

	// Interrupt loop
	while (true) {		
		if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
			printf("driver_receive failed with: %d", r);
			continue;
		}

		if (is_ipc_notify(ipc_status)) {
			switch (_ENDPOINT_P(msg.m_source)) {
				case HARDWARE:
					if (msg.m_notify.interrupts & rtc_bit_mask) {
						rtc_ih();
						break;
					}
					
					break; /* no other notifications expected: do nothing */     
				default:
					break;
			}
		}
	
		else { /* received a standard message, not a notification */
			/* no standard messages expected: do nothing */
		}
	}

	uint32_t data;

	if (rtc_read_register(RTC_REG_B, &data))
		return 1;

	data &= ~RTC_AIE;

	if (rtc_write_register(RTC_DATA_REG, data))
		return 1;

	if (rtc_unsubscribe_int()) 
		return 1; 

	date_t final_time = get_date();

	printf("Final time: %u:%u:%u\n", final_time.hour, final_time.minutes, final_time.seconds);

	return 0;
}
