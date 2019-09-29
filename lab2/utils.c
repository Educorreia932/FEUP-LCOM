#include <lcom/lcf.h>

#include <stdint.h>

int(util_get_LSB)(uint16_t val, uint8_t *lsb) {
  // How can it ever fail?
  *lsb = (uint8_t) (val & 0x00FF);
  return 0;
}

int(util_get_MSB)(uint16_t val, uint8_t *msb) {
  // How can it ever fail?
  val = val >> 8;
  *msb = (uint8_t) val;
  return 0;
}

int (util_sys_inb)(int port, uint8_t *value) {
  // Check if it dies later
  u32_t *value32 = (u32_t *) value;
  return sys_inb(port, value32);
}
