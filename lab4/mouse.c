#include "mouse.h"

uint8_t counter = 0;
bool error = false;

uint8_t (mouse_subscribe_int)(uint8_t *bit_no) {
    if (!bit_no) // Check if pointer is NULL
        return 1;

    mouse_hook_id = MOUSE_IRQ;
    *bit_no = MOUSE_IRQ;	

    if (sys_irqsetpolicy(MOUSE_IRQ, IRQ_EXCLUSIVE | IRQ_REENABLE, &mouse_hook_id))
        return 1;

    return 0;
}

uint8_t (mouse_unsubscribe_int)() {
    if (sys_irqrmpolicy(&mouse_hook_id))
        return 1;
                
    return 0;
}

void (mouse_ih)() {
    if (util_sys_inb(STAT_REG, &st)) {
        error = 1; 
        return;        
    }

    // If either one is set to 1, there's an error
    if (st & (ST_PAR_ERR | ST_TO_ERR)) {
        error = 1; 
        return;
    }

    if (util_sys_inb(OUT_BUF, &data_bytes[counter])) {
        error = 1;
        return;
    }

    counter++;       

    if (counter > 2)
        counter = 0;
}

uint8_t (read_data)() {
    uint8_t first_byte = data_bytes[0]; 
    bool x_sign, y_sign; 

    data.lb = first_byte & BIT(0);     
    data.rb = first_byte >> 1 & BIT(0);
    data.mb = first_byte >> 1 & BIT(0);   

    y_sign = first_byte >> 2 & BIT(0);
    x_sign = first_byte >> 1 & BIT(0);

    data.y_ov = first_byte >> 1 & BIT(0);
    data.x_ov = first_byte >> 1 & BIT(0);

    data.delta_x = data_bytes[1];
    data.delta_y = data_bytes[2];

    if (x_sign)
        data.delta_x |= EXTEND_SIGN;

    if (y_sign)
        data.delta_y |= EXTEND_SIGN;

    data.bytes[0] = data_bytes[0];
    data.bytes[1] = data_bytes[1];
    data.bytes[2] = data_bytes[2]; 

    return 0;   
}

uint8_t (kbc_send_cmd)(uint8_t port, uint8_t cmd) {        
    // This section waits until it can write to input buffer or it reaches a timeout state
    uint8_t i = TIMEOUT_ATTEMPTS;

    while (i) { // Tries for i attempts
        if (util_sys_inb(STAT_REG, &st)) // Read Status
            return 1;

        // Can only write if the ST_IN_BUF is set to 0
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
