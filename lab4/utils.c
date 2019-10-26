#include "utils.h"

unsigned long int no_of_calls = 0;

int(util_get_LSB)(uint16_t val, uint8_t *lsb) {
	if (!lsb)
		return 1;

	*lsb = (uint8_t) val;

	return 0;
}

int (util_get_MSB)(uint16_t val, uint8_t *msb) {
	if (!msb)
		return 1;

	val = val >> 8;
	*msb = (uint8_t) val;

	return 0;
}

int (util_sys_inb)(int port, uint8_t *value) {
  u32_t value32;

  if (sys_inb(port, &value32))
    return 1;

  *value = (uint8_t) value32;

  no_of_calls++;

  return 0;
}
