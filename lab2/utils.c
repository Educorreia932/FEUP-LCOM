#include <lcom/lcf.h>

#include <stdint.h>

int(util_get_LSB)(uint16_t val, uint8_t *lsb) {
  if (!lsb) // Verify if pointer is NULL
    return 1;
  *lsb = (uint8_t) val;
  return 0;
}

int(util_get_MSB)(uint16_t val, uint8_t *msb) {
  if (!msb) // Verify if pointer is NULL
    return 1;
  val = val >> 8;
  *msb = (uint8_t) val;
  return 0;
}

int (util_sys_inb)(int port, uint8_t *value) {
  if (!value) // Verify if pointer is NULL
    return 1;

  u32_t value32; // Aux u32_t to avoid errors in the sys_inb call
  if (sys_inb(port, &value32))
    return 1;
  *value = (uint8_t) value32; // Copies the result of the call to the 8 bit argument
  return 0;
}
