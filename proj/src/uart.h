#pragma once

#include <lcom/lcf.h>

#include "utils.h"

#define SER_1_IRQ 3
#define SER_2_IRQ 4

#define COM1 0x3F8 
#define COM2 0x2F8

#define DLL 0 /**< @brief Divisor Latch LSB */
#define DLM 1 /**< @brief Divisor Latch MSB */

/** @name UART Accessible (8-bit) Registers */
///@{
#define RBR 0 /**< @brief Receiver Buffer Register */
#define THR 0 /**< @brief Transmitter Holding Register */
#define IER 1 /**< @brief Interrupt Enable Register */
#define IIR 2 /**< @brief Interrupt Identification Register */
#define FCR 2 /**< @brief FIFO Control Register */
#define LCR 3 /**< @brief Line Control Register */
#define MCR 4 /**< @brief Modem Control Register */
#define LSR 5 /**< @brief Line Status Register */
#define MSR 6 /**< @brief Modem Status Register */
#define SR 7 /**< @brief Scratchpad Register */
///@}

/** @name Line Control Register (LCR) */
///@{
#define NO_BITS_PER_CHAR BIT(1) | BIT(0)
#define STOP_BIT BIT(2)
#define PARITY_CONTROL BIT(5) | BIT(4) | BIT(3)
#define BREAK_CONTROL BIT(6)
#define DLAB BIT(7)
///@}

/** @name Line Status Register (LSR) (Read only) */
///@{
#define RECEIVER_DATA BIT(0) /**< @brief Set to 1 when there is data for receiving */
#define OVERRUN_ERROR BIT(1) /**< @brief  Set to 1 when a characters received is overwritten by another one */
#define PARITY_ERROR BIT(2) /**< @brief Set to 1 when a character with a parity error is received */
#define FRAMING_ERROR BIT(3) /**< @brief Set to 1 when a received character does not have a valid Stop bit */
#define BREAK_INTERRUPT BIT(4) /**< @brief Set to 1 when the serial data input line is held in the low level for longer than a full “word” transmission */
#define THR_EMPTY BIT(5) /**< @brief When set, means that the UART is ready to accept a new character for transmitting */
#define THR_TSR_EMPTY BIT(6) /**< @brief When set, means that both the THR and the Transmitter Shift Register are both empty */
#define FIFO_ERROR BIT(7) /**< @brief Set to 1 when there is at least one parity error or framing error or break indication in the FIFO Reset to 0 when the LSR is read, if there are no subsequent errors in the FIFO */
///@}

uint8_t (ser_subscribe_int)(uint8_t *bit_no);
uint8_t (ser_unsubscribe_int)();

/** 
 * @brief Read the control/status registers of the UART.
 * @details And displays it in a convenient format for humans to read.
 */
int uart_test_conf(uint16_t base_addr);

/**
 * @param bits Number of bits per character
 * @param stop Number of stop bits
 * @param parity -1, 0 or 1
 * @param rate Bit-rate
 */
int uart_test_set(uint16_t base_addr, uint8_t bits, uint8_t stop, int8_t parity, uint8_t rate);

/**
 * @brief Use the UART in polled mode for simplex communication
 * @param tx Role (transmitter or receiver)
 * @param bits Number of bits per character
 * @param stop Number of stop bits
 * @param parity -1, 0 or 1
 * @param rate Bit-rate
 * @param stringc
 * @param strings
 */
int uart_test_poll(uint16_t base_addr, uint8_t tx, uint8_t bits, uint8_t stop, int8_t parity, uint8_t rate, int stringc, char *strings[]);

int receive_data(uint16_t base_addr, uint8_t delay);

int send_data(uint8_t base_addr, int stringc, char *strings[], uint8_t delay);

int uart_test_int();

int uart_test_fifo();

