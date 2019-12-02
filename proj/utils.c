#include "utils.h"
#include "i8042.h"

unsigned long int no_of_calls = 0;
static uint8_t st;

/** @returns 0 upon success, 1 otherwise */
int(util_get_LSB)(uint16_t val, uint8_t *lsb) {
	if (!lsb)
		return 1;

	*lsb = (uint8_t) val;

	return 0;
}

/** @returns 0 upon success, 1 otherwise */
int (util_get_MSB)(uint16_t val, uint8_t *msb) {
	if (!msb)
		return 1;

	val = val >> 8;
	*msb = (uint8_t) val;

	return 0;
}

/** @returns 0 upon success, 1 otherwise */
int (util_sys_inb)(int port, uint8_t *value) {
	if (!value)
		return 1;
		
	u32_t value32;

	if (sys_inb(port, &value32))
		return 1;

	*value = (uint8_t) value32;

	no_of_calls++;

	return 0;
}

/** @brief Sends a KBC command 
 * @param port Port in which the command will be written
 * @param cmd Command to be issued
 * @returns 0 upon success, 1 otherwise 
 */
uint8_t (kbc_send_cmd)(uint8_t port, uint8_t cmd) {        
    // This section waits until it can write to input buffer or it reaches a timeout state
    uint8_t i = TIMEOUT_ATTEMPTS;

    while (i) { // Tries for i attempts
		if (util_sys_inb(STAT_REG, &st)) // Read Status
			return 1;

		// Can only write if the ST_IN_BUF is set to 0 (is empty)
		if (st & ST_IN_BUF) {
			i--;

			if (tickdelay(micros_to_ticks(KBC_WAIT)))
				return 1;

			continue;
		}

		else
			break;                
    }

    if (i == 0) // Timed out
     	 return 1;

    if (sys_outb(port, cmd)) // Write the command
      	return 1;

    return 0;
}

/** @brief Reads scancodes from the keyboard's output buffer 
 * @returns 0 upon success, 1 otherwise 
 */
uint8_t kbc_read_outbf(uint8_t port, uint8_t *content, bool isMouse) {
	// This section waits until there is something to read from the output buffer or it reaches a time out state
	uint8_t i = TIMEOUT_ATTEMPTS;

	// Tries for i attempts
	while (i) {
		if (util_sys_inb(STAT_REG, &st)) // Read Status
  			return 1;

		// Can only read if the ST_OUT_BUF is set to 1
		// AND if isMouse is true, check if the status has Mouse Data
		if ((st & ST_OUT_BUF) && ((st & ST_MOUSE_DATA) == isMouse * ST_MOUSE_DATA))
		// (st & BIT(n)) has BIT(n) as 0 or 1, compare to isMouse (1 or 0) * BIT(n)
			break;

		else {
			i--;
		  
		  	if (tickdelay(micros_to_ticks(KBC_WAIT)))
			 	return 1;
			
			continue;
		}		
	}

	if (i == 0) // Timed out
		return 1;

	if (util_sys_inb(OUT_BUF, content))
		return 1;
	
	return 0;

}

/** @brief Restores KBC byte to default state (returned by minix_get_dflt_kbc_byte()) */
uint8_t restore_kbc_byte() {
	if (kbc_send_cmd(IN_BUF_CMD, WRITE_CMD_BYTE))
		return 1;

	return kbc_send_cmd(IN_BUF_ARGS, minix_get_dflt_kbc_cmd_byte());
}

phys_bytes far_ptr_to_linear(uint32_t far_ptr) {
	// já vem com shift de 4 para a direita (16 - 4 = 12)
	return (phys_bytes) (((far_ptr & 0xFFFF0000) >> 12) + (far_ptr & 0x0000FFFF));
}

void* linear_to_virt(uint32_t linear_ptr, mmap_t *map) {
	return (void*) (((uint32_t) map->virt) + (linear_ptr - map->phys));
}
