#pragma once

#include <lcom/lcf.h>

extern unsigned long int no_of_calls;

/** 
 * @param val input 2 byte integer
 * @param lsb Address of memory location to be update with val's LSB
 * @returns 0 upon success, 1 otherwise 
 */
int (util_get_LSB) (uint16_t val, uint8_t *lsb);

/** 
 * @param val input 2 byte integer
 * @param msb Address of memory location to be update with val's MSB
 * @returns 0 upon success, 1 otherwise 
 */
int (util_get_MSB) (uint16_t val, uint8_t *msb);

/** 
 * @brief Invokes sys_inb() system call but reads the value into a uint8_t variable. 
 * @param port THe port that is to be read
 * @param value Address of the 8-bit variable to be update with the value read
 * @returns 0 upon success, 1 otherwise 
 */
int (util_sys_inb) (int port, uint8_t *value);

/** 
 * @brief Sends a KBC command 
 * @param port Port in which the command will be written
 * @param cmd Command to be issued
 * @returns 0 upon success, 1 otherwise 
 */
uint8_t (kbc_send_cmd) (uint8_t port, uint8_t cmd);

/** 
 * @brief Reads scancodes from the keyboard's output buffer 
 * @returns 0 upon success, 1 otherwise 
 */
uint8_t (kbc_read_outbf) (uint8_t port, uint8_t *content, bool isMouse);

/** 
 * @brief Restores KBC byte to default state (returned by minix_get_dflt_kbc_byte())
 */
uint8_t restore_kbc_byte();

phys_bytes far_ptr_to_linear(uint32_t far_ptr);

void* linear_to_virt(uint32_t linear_ptr, mmap_t *map);

/** 
 * @brief Function to return the digit of n-th position of num. Position starts from 0
 */
uint8_t get_digit(uint8_t num, uint8_t n);
