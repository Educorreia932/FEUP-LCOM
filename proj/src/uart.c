#include "uart.h"
#include "queue.h"

// tq = transmitter queue, rq = receiver queue
static Queue_t *rq;
static int ser_hook_id;

void uart_initialize_receiver_queue() {
    if (rq == NULL)
        rq = new_queue();
}

void uart_free_receiver_queue() {
    if (rq != NULL) {
        free_queue(rq);
        rq = NULL;
    }
}

uint8_t uart_receiver_q_front() {
    return queue_front(rq);
}

uint8_t uart_receiver_q_pop() {
    return queue_pop(rq);
}

bool uart_receiver_q_empty() {
    return queue_is_empty(rq);
}

uint8_t uart_subscribe_int(uint32_t *bit_mask) {
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

    uint8_t parity = (lcr & LCR_PARITY_CONTROL_MASK);
    printf("Parity control:\n");
    switch (parity) {
        case LCR_PARITY_ODD: // 0b001
            printf("\tOdd parity\n");
            break;
        case LCR_PARITY_EVEN: // 0b011
            printf("\tEven parity\n");
            break;
        case LCR_PARITY_ALWAYS_1: // 0b101
            printf("\tParity bit is 1\n");
            break;
        case LCR_PARITY_ALWAYS_0: // 0b111
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


    // IER

    uint8_t ier;
    if (util_sys_inb(COM1_BASE + UART_REG_IER, &ier))
        return 1;
    
    if (ier & IER_RECEIVED_DATA_INT)
        printf("IER: Received data int on\n");
    else
        printf("IER: Received data int off\n");
    if (ier & IER_TRANSMITTER_EMPTY_INT)
        printf("IER: THR empty int on\n");
    else
        printf("IER: THR empty int off\n");
    if (ier & IER_RECEIVER_LINE_STATUS_INT)
        printf("IER: Receiver line status int on\n");
    else
        printf("IER: Receiver line status int is off\n");
    if (ier & IER_MODEM_STATUS_INT)
        printf("IER: Modem status int on\n");
    else
        printf("IER: Modem status int off\n");
    

    return 0;
}

uint8_t uart_set_conf() {

    /* LCR */
    // Set stop bit to true and toggle DLAB to write the bit rate
    uint8_t lcr = LCR_STOP_BIT | LCR_DLAB;

    // Set mode parity to odd
    lcr |= LCR_PARITY_ODD;

    // Set no of bits per char to 8
    // bx11 = 3 -> 3 + 5 = 8 -> 8 bits per char
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
    // uint8_t ier = IER_RECEIVED_DATA_INT | IER_TRANSMITTER_EMPTY_INT;
    uint8_t ier = IER_RECEIVED_DATA_INT;
    if (sys_outb(COM1_BASE + UART_REG_IER, ier))
        return 1;

    uint8_t fcr = FCR_ENABLE_BOTH_FIFO | FCR_TRIGGER_LEVEL_1;
    if (sys_outb(COM1_BASE + UART_REG_FCR, fcr))
        return 1;

    return 0;
}


// Called when the when we receive the THR empty interrupt
static void uart_thr_empty_ih() {
    
    printf("THR empty ih called... Unexpected\n");
    
}

uint8_t uart_send_char(uint8_t to_send) {    

    sys_outb(COM1_BASE + UART_REG_THR, to_send);

    return 0;
}

uint8_t uart_receive_char() {

    // Read LSR
    uint8_t lsr;
    if (util_sys_inb(COM1_BASE + UART_REG_LSR, &lsr))
        return 1;
    
    uint8_t received_char;

    while (lsr & LSR_RECEIVER_DATA) {
        if (util_sys_inb(COM1_BASE + UART_REG_RBR, &received_char))
            return 1;

        // Check for errors in the communication
        if (!(lsr & (LSR_OVERRUN_ERROR | LSR_PARITY_ERROR | LSR_FRAMING_ERROR))) {
            // aka It's all good
            queue_push_back(rq, received_char);
            // printf("Received: %d\n", queue_back(rq));
        }

        if (util_sys_inb(COM1_BASE + UART_REG_LSR, &lsr))
            return 1;
    }

    return 0;

}


void uart_ih() {
    
    // Read iir
    uint8_t iir;
    util_sys_inb(COM1_BASE + UART_REG_IIR, &iir);

    if (iir & IIR_NO_INT_PENDING)
        return;

    // Find what caused the interrupt
    switch (iir & IIR_INT_SOURCE_MASK) {
        case IIR_INT_SOURCE_RECEIVED_DATA_AVAILABLE:
            uart_receive_char();
            break;
        case IIR_INT_SOURCE_THR_EMPTY:
            uart_thr_empty_ih();
            break;
        // case IIR_INT_SOURCE_RECEIVER_LINE_STATUS:
        //     break;
        // case IIR_INT_SOURCE_CHARACTER_TIMEOUT_FIFO:
        //     break;
        // case IIR_INT_SOURCE_MODEM_STATUS:
        //     break;
        default:
            break;
    }

}
