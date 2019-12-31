#pragma once

#include <lcom/lcf.h>

#include "utils.h"

#define SER_1_IRQ 4
#define SER_2_IRQ 3

#define COM1_BASE 0x3F8 
#define COM2_BASE 0x2F8

#define UART_REG_DLL 0 /**< @brief Divisor Latch LSB */
#define UART_REG_DLM 1 /**< @brief Divisor Latch MSB */

/** @name UART Accessible (8-bit) Registers */
///@{
#define UART_REG_RBR 0 /**< @brief Receiver Buffer Register. */
#define UART_REG_THR 0 /**< @brief Transmitter Holding Register. */
#define UART_REG_IER 1 /**< @brief Interrupt Enable Register. */
#define UART_REG_IIR 2 /**< @brief Interrupt Identification Register. */
#define UART_REG_FCR 2 /**< @brief FIFO Control Register. */
#define UART_REG_LCR 3 /**< @brief Line Control Register.*/
#define UART_REG_MCR 4 /**< @brief Modem Control Register. */
#define UART_REG_LSR 5 /**< @brief Line Status Register. */
#define UART_REG_MSR 6 /**< @brief Modem Status Register. */
#define UART_REG_SR 7 /**< @brief Scratchpad Register. */
///@}

/** @name Line Control Register (LCR) */
///@{
#define LCR_NO_BITS_PER_CHAR (BIT(1) | BIT(0))
#define LCR_STOP_BIT BIT(2)
#define LCR_PARITY_CONTROL_MASK (BIT(5) | BIT(4) | BIT(3))
// We would include no parity here, but it's a strange special case
#define LCR_PARITY_ODD BIT(3)
#define LCR_PARITY_EVEN (BIT(4) | BIT(3))
#define LCR_PARITY_ALWAYS_1 (BIT(5) | BIT(4))
#define LCR_PARITY_ALWAYS_0 (BIT(5) | BIT(4) | BIT(3))
#define LCR_BREAK_CONTROL BIT(6)
#define LCR_DLAB BIT(7)
///@}

/** @name Line Status Register (LSR) (Read only) */
///@{

/**
 * @brief Set to 1 when there is data for receiving.
 * Reset upon reading LSR.
 */
#define LSR_RECEIVER_DATA BIT(0)
/**
 * @brief Set to 1 when a characters received is overwritten by another one.
 *      Reset upon reading LSR. 
 */
#define LSR_OVERRUN_ERROR BIT(1)
/**
 * @brief Set to 1 when a character with a parity error is received.
 *      Reset upon reading LSR.
 */
#define LSR_PARITY_ERROR BIT(2)
/**
 * @brief Set to 1 when a received character does not have a valid Stop bit.
 *      Reset upon reading LSR.
 */
#define LSR_FRAMING_ERROR BIT(3)
/**
 * @brief Set to 1 when the serial data input line is held in the low level for longer than a full “word” transmission.
 *  Reset upon reading LSR.
 */
#define LSR_BREAK_INTERRUPT BIT(4)
/**
 * @brief When set, means that the UART is ready to accept a new character for transmitting.
 */
#define LSR_THR_EMPTY BIT(5)
/**
 * @brief When set, means that both the THR and the Transmitter Shift Register are both empty. 
 */
#define LSR_THR_TSR_EMPTY BIT(6)
/**
 * @brief Set to 1 when there is at least one parity error or framing error or break indication in the FIFO Reset to 0 when the LSR is read, if there are no subsequent errors in the FIFO. 
 */
#define LSR_FIFO_ERROR BIT(7)
///@}

/** @name FIFO Control Register (FCR) */
///@{

#define FCR_ENABLE_BOTH_FIFO BIT(0) /**< @brief FIFO Control Register. */
#define FCR_CLEAR_RCVR BIT(1) /**< @brief Set to 1 to clear all bytes in RCVR FIFO. Self-clearing.  */
#define FCR_CLEAR_XMIT BIT(2) /**< @brief Set to 1 to clear all bytes in the XMIT FIFO. Self-clearing */
#define FCR_ENABLE_64_BYTE_FIFO BIT(5) /**< @brief Enable 64 byte FIFO (for 16750 only) */
#define FCR_TRIGGER_LEVEL_MASK (BIT(7) | BIT(6)) /**< @brief Mask for the trigger level */
#define FCR_TRIGGER_LEVEL_1 0 /**< @brief Set trigger level to 1 (16 bit FIFO value only) */
#define FCR_TRIGGER_LEVEL_4 BIT(6) /**< @brief Set trigger level to 4 (16 bit FIFO value only) */
#define FCR_TRIGGER_LEVEL_8 BIT(7) /**< @brief Set trigger level to 8 (16 bit FIFO value only) */
#define FCR_TRIGGER_LEVEL_14 (BIT(7) | BIT(6)) /**< @brief Set trigger level to 14 (16 bit FIFO value only) */
///@}


/** @name Interrupt Enable Register (IER) */
///@{
/**
 * @brief Enables the Received Data Available Interrupt.
 */
#define IER_RECEIVED_DATA_INT BIT(0)
/**
 * @brief Enables the Transmitter Holding Register Empty Interrupt.
 */
#define IER_TRANSMITTER_EMPTY_INT BIT(1)
/**
 * @brief Enables the Receiver Line Status Interrupt This event is gen-
erated when there is a change in the state of bits 1 to 4, i.e.
 */
#define IER_RECEIVER_LINE_STATUS_INT BIT(2)
/**
 * @brief Enable Receiver Line Status Interrupt.
 */
#define IER_MODEM_STATUS_INT BIT(3)

///@}


/** @name Interrupt Identification Register (IIR) */
///@{

/**
 * @brief If set, no interrupt is pending.
 */
#define IIR_NO_INT_PENDING BIT(0)
/**
 * @brief A mask to be used to distinguish the source of the interrupts.
 */
#define IIR_INT_SOURCE_MASK (BIT(1) | BIT(2) | BIT(3))
/**
 * @brief Receiver Line Status.
 */
#define IIR_INT_SOURCE_RECEIVER_LINE_STATUS (BIT(1) | BIT(2))
/**
 * @brief Reived Data Available.
 */
#define IIR_INT_SOURCE_RECEIVED_DATA_AVAILABLE BIT(2)
/**
 * @brief Character Timeout (FIFO), discussed below.
 */
#define IIR_INT_SOURCE_CHARACTER_TIMEOUT_FIFO (BIT(3) | BIT(2))
/**
 * @brief Transmitter Holding Register Empty.
 */
#define IIR_INT_SOURCE_THR_EMPTY BIT(1)
/**
 * @brief Modem Status.
 */
#define IIR_INT_SOURCE_MODEM_STATUS 0
///@}



/* VARIABLES */

uint8_t uart_received_char;


/* FUNCTIONS */

uint8_t uart_subscribe_int(uint32_t *bit_mask);
uint8_t uart_unsubscribe_int();

void uart_initialize_receiver_queue();
void uart_free_receiver_queue();

uint8_t uart_receiver_q_front();
void uart_receiver_q_pop();
bool uart_receiver_q_empty();

uint8_t uart_print_conf();
uint8_t uart_set_conf();

uint8_t uart_send_char(uint8_t to_send);

void uart_ih();
