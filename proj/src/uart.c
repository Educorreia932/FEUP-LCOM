#include "uart.h"

static int ser_hook_id;
uint8_t uart_received_char = 0;

uint8_t uart_subscribe_int(int *bit_mask) {
    if (!bit_mask) // Check if pointer is NULL
        return 1;

    ser_hook_id = SER_1_IRQ;
    *bit_mask = BIT(SER_1_IRQ);	

    return sys_irqsetpolicy(SER_1_IRQ, IRQ_EXCLUSIVE | IRQ_REENABLE, &ser_hook_id);
}

uint8_t uart_unsubscribe_int() {
    return sys_irqrmpolicy(&ser_hook_id);
}

uint8_t uart_print_conf() {

    uint8_t lcr;

    if (util_sys_inb(COM1_BASE + UART_REG_LCR, &lcr))
        return 1;

    printf("LCR: %x\n", lcr);

    uint8_t no_bits_per_char = lcr & LCR_NO_BITS_PER_CHAR;
    printf("Number of bits per char:\n%d bits per char\n", no_bits_per_char + 5);

    uint8_t stop_bit = (lcr & LCR_STOP_BIT) >> 2;
    printf("Number of stop bits:\n%d stop bit(s)\n", stop_bit + 1);

    uint8_t parity = (lcr & LCR_PARITY_CONTROL) >> 3;
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

    uint8_t dlab = (lcr & LCR_DLAB) >> 7;
    printf("Divisor Latch Access\n");
    switch (dlab) {
        case 0:
            printf("\tRBR (read)\n");
            break;
        case 1:
            printf("\tTHR (write)\n");
            break;
    }

    return 0;
}

uint8_t uart_set_conf() {

    /* LCR */
    // Set stop bit to true and toggle DLAB to write the bit rate
    uint8_t lcr = LCR_STOP_BIT | LCR_DLAB;

    // Set mode parity to even
    lcr |= BIT(3);

    // Set no of bits per char to 8
    lcr |= BIT(1) | BIT(0);

    // Write lcr (and get access to dll and ...)
    if (sys_outb(COM1_BASE + UART_REG_LCR, lcr))
        return 1;
    
    // Write our bitrate
    // Recommended value in the labs, 115200 / 115200 = 1
    uint16_t bitrate = 1;
    uint8_t dll, dlm;
    if (util_get_LSB(bitrate, &dll) || util_get_MSB(bitrate, &dlm))
        return 1;
    if (sys_outb(COM1_BASE + UART_REG_DLL, dll) || sys_outb(COM1_BASE + UART_REG_DLM, dlm))
        return 1;
    
    // Restore the DLAB bit to 0
    lcr &= ~LCR_DLAB;
    if (sys_outb(COM1_BASE + UART_REG_LCR, lcr))
        return 1;
    

    /* IER */
    // Get interrupts for receiving data only
    uint8_t ier = IER_RECEIVED_DATA_INT;
    if (sys_outb(COM1_BASE + UART_REG_IER, ier))
        return 1;

    return 0;
}


uint8_t uart_send_char(uint8_t to_send) {

    return sys_outb(COM1_BASE + UART_REG_THR, to_send);

}


uint8_t uart_receive_char() {
    
    // Read LSR
    uint8_t lsr;
    if (util_sys_inb(COM1_BASE + UART_REG_LSR, &lsr))
        return 1;
    
    // Check for errors in the communication
    if (lsr & (LSR_OVERRUN_ERROR | LSR_PARITY_ERROR | LSR_FRAMING_ERROR))
        return 1;

    return util_sys_inb(COM1_BASE + UART_REG_RBR, &uart_received_char);

}


void uart_ih() {
    
    // Read iir
    uint8_t iir;
    util_sys_inb(COM1_BASE + UART_REG_IIR, &iir);

    // Find what caused the interrupt
    switch (iir & IIR_INT_SOURCE_MASK) {
        case IIR_INT_SOURCE_RECEIVED_DATA_AVAILABLE:
            uart_receive_char();
            break;
        // case IIR_INT_SOURCE_RECEIVER_LINE_STATUS:
        //     break;
        // case IIR_INT_SOURCE_CHARACTER_TIMEOUT_FIFO:
        //     break;
        // case IIR_INT_SOURCE_THR_EMPTY:
        //     break;
        // case IIR_INT_SOURCE_MODEM_STATUS:
        //     break;
        default:
            break;
    }

}

// TESTING PURPUOSES
uint8_t test_uart(uint8_t tx) {
    
    uart_set_conf();
    uart_print_conf();

    if (tx == 0) {
        printf("Configured to receive data\n");
        while (1) {
            uart_send_char('Y');
            tickdelay(1);
        }
    }
    else {
        printf("Configured to send data\n"); 
        int uart_mask;
        uart_subscribe_int(&uart_mask);

        int r, ipc_status;
	    message msg;
        
        while (uart_received_char == 0) {
            if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
                printf("start_game: driver_receive failed with: %d", r);
                continue;
            }

            if (is_ipc_notify(ipc_status)) {
                switch (_ENDPOINT_P(msg.m_source)) {
                    case HARDWARE: /* hardware interrupt notification */
                        if (msg.m_notify.interrupts & uart_mask) {
                            uart_ih();
                        }

                        break;
                    default:
                        break; /* no other notifications expected: do nothing */     
                }
            }
        }

        printf("GOT SOMETHING!\nGot _%d_\n", uart_received_char);
        uart_unsubscribe_int();
    }

    return 0;
}
