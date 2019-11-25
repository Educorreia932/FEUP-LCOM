#pragma once

#include <lcom/lcf.h>

extern unsigned long int no_of_calls;

int (util_get_LSB) (uint16_t val, uint8_t *lsb);
int (util_get_MSB) (uint16_t val, uint8_t *msb);
int (util_sys_inb) (int port, uint8_t *value);

uint8_t (kbc_send_cmd) (uint8_t port, uint8_t cmd);
uint8_t (kbc_read_outbf) (uint8_t port, uint8_t *content, bool isMouse);
uint8_t restore_kbc_byte();

phys_bytes far_ptr_to_linear(uint32_t far_ptr);

void* linear_to_virt(uint32_t linear_ptr, mmap_t *map);
