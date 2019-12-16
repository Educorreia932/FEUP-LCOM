#include "uart.h"

static int ser_hook_id;

uint8_t (uart_subscribe_int)(uint8_t *bit_no) {
    if (!bit_no) // Check if pointer is NULL
        return 1;

    ser_hook_id = SER_1_IRQ;
    *bit_no = SER_1_IRQ;	

    return sys_irqsetpolicy(SER_1_IRQ, IRQ_EXCLUSIVE | IRQ_REENABLE, &ser_hook_id);
}

uint8_t (uart_unsubscribe_int)() {
    return sys_irqrmpolicy(&ser_hook_id);
}

int uart_test_conf(uint16_t base_addr) {
    uint8_t lcr, ier;

    if (util_sys_inb(base_addr + LCR, &lcr))
        return 1;
    
    if (util_sys_inb(base_addr + IER, &ier))
        return 1;

    printf("LCR: %x\n", lcr);

    uint8_t no_bits_per_char = lcr & NO_BITS_PER_CHAR;

    printf("Number of bits per char:\n");

    switch (no_bits_per_char) {
        case 0: // 0b00
            printf("\t5 bits per char\n");
            break;
        case 1: // 0b01
            printf("\t6 bits per char\n");
        case 2: // 0b10
            printf("\t7 bits per char\n");
        case 3: // 0b11
            printf("\t8 bits per char\n");
    }

    uint8_t stop_bit = (lcr & STOP_BIT) >> 2;

    printf("Number of stop bits:\n");

    switch (stop_bit) {
        case 0:
            printf("\t1 stop bit\n");
            break;
        case 1:
            printf("\t2 stop bits\n");
            break;
    }

    uint8_t parity = (lcr & PARITY_CONTROL) >> 3;

    printf("Parity control:\n");

    switch (parity) {
        case 1: // 0b001
            printf("\tOdd parity\n");
            break;
        case 3: // 0b011
            printf("\tEven parity\n");
            break;
        case 5: // 0b101
            printf("\tParity bit is 1\n");
            break;
        case 7: // 0b111
            printf("\tParity bit is 0\n");
            break;
        default: // 0bxx0
            printf("\tNo parity\n");
            break;
    }

    uint8_t dlab = (lcr & DLAB) >> 7;

    printf("Divisor Latch Access\n");

    switch (dlab) {
        case 0:
            printf("\tRBR (read)\n");
            break;
        case 1:
            printf("\tTHR (write)\n");
            break;
    }

    // TODO: IER

    return 0;
}

int uart_test_set(uint16_t base_addr, uint8_t bits, uint8_t stop, int8_t parity, uint8_t rate) {
    uint8_t lcr;

    if (util_sys_inb(base_addr + LCR, &lcr))
            return 1;

    // Parity

    switch (parity) {
        case -1: // None
            parity = 0;
            break;
        case 0: // Odd parity
            parity = 1;
            break;
        case 1: // Even parity
            parity = 3;
            break;
    }

    // Rate

    uint8_t dll, dlm;

    rate = 115200 / rate;

    dll = rate % 256;
	dlm = rate / 256;

	if (util_sys_inb(base_addr + LCR, &lcr) != OK)
		return 1;

	lcr |= DLAB;

	if (sys_outb(base_addr + LCR, lcr))
		return 1;

	if (sys_outb(base_addr + DLL, dll))
		return 1;

	if (sys_outb(base_addr + DLM, dlm))
		return 1;

    // Set

    lcr &= 0xC0; // We don't to change bits 6 and 7

    lcr |= bits | stop << 2 | parity << 3;

    if (sys_outb(base_addr + LCR, lcr))
        return 1;

    return 0;
}

int uart_test_poll(uint16_t base_addr, uint8_t tx, uint8_t bits, uint8_t stop, int8_t parity, uint8_t rate, int stringc, char* strings[]) {
    uint8_t delay = 100000 / bits;

    if (!tx) {
        if (receive_data(base_addr, delay))
            return 1;
    }
        
    else
        if (send_data(base_addr, stringc, strings, delay))
            return 1;

    return 0;
}

int receive_data(uint16_t base_addr, uint8_t delay) {
    uint8_t lsr;
    uint8_t character; 

    while (true) {
        if (util_sys_inb(base_addr + LSR, &lsr))
            return 1;

        while(!(lsr & RECEIVER_DATA)) {
            tickdelay(micros_to_ticks(delay));
            
            if (util_sys_inb(base_addr + LSR, &lsr))
                return 1;
        }

        if (util_sys_inb(base_addr + RBR, &character))
            return 1;

        if (character == 1)
            return 1;

        if (character == '.')
            return 0;
    }
}

int send_data(uint8_t base_addr, int stringc, char *strings[], uint8_t delay) {
    uint8_t lsr;
    
    for (int i = 0; i < stringc; i++) {
        if (util_sys_inb(base_addr + LSR, &lsr))
		    return 1;

        if (lsr & OVERRUN_ERROR || lsr & PARITY_ERROR || lsr & FRAMING_ERROR)
            return 1;

        while (!(lsr & THR_EMPTY)) {
            tickdelay(micros_to_ticks(delay));

            if (util_sys_inb(base_addr + LSR, &lsr))
                return 1;
        }
        
        // Send character
        if (sys_outb(base_addr + THR, strings[i]))
            return 1;
    }
    
    return 0;
}
