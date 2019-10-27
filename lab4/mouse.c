#include "mouse.h"

// Gloval Vars
bool mouse_ih_error = false, is_mouse_packet_complete = false;
struct packet mouse_parsed_packet;

// Static vars
static int mouse_hook_id;
static uint8_t st, counter = 0;
static uint8_t packet_bytes[3];

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
        mouse_ih_error = 1; 
        return;        
    }

    // If either one is set to 1, there's an error
    if (st & (ST_PAR_ERR | ST_TO_ERR)) {
        mouse_ih_error = 1; 
        return;
    }

    if (util_sys_inb(OUT_BUF, &packet_bytes[counter])) {
        mouse_ih_error = 1;
        return;
    }

}

uint8_t (parse_packet)() {

    mouse_parsed_packet.bytes[0] = packet_bytes[0];
    mouse_parsed_packet.bytes[1] = packet_bytes[1];
    mouse_parsed_packet.bytes[2] = packet_bytes[2]; 

    // According to stdbool, for numerical types (aka uint8_t)
    // 0 gets converted to 0 (false) and any other number gets converted
    // to 1 (true), so we don't need to shift the result of the parsed data
    
    /* MOUSE BUTTONS */
    mouse_parsed_packet.lb = packet_bytes[0] & MOUSE_PARSE_LB;     
    mouse_parsed_packet.rb = packet_bytes[0] & MOUSE_PARSE_RB;
    mouse_parsed_packet.mb = packet_bytes[0] & MOUSE_PARSE_MB;   

    /* MOUSE MOVEMENT DELTAS */
    // Doesn't take into account the sign
    mouse_parsed_packet.delta_x = (uint16_t) packet_bytes[1];
    mouse_parsed_packet.delta_y = (uint16_t) packet_bytes[2];

    // Fixes the sign if it was a negative number
    if (packet_bytes[0] & MOUSE_PARSE_X_DELTA_MSB)
        mouse_parsed_packet.delta_x |= EXTEND_SIGN;
    if (packet_bytes[0] & MOUSE_PARSE_Y_DELTA_MSB)
        mouse_parsed_packet.delta_y |= EXTEND_SIGN;

    mouse_parsed_packet.x_ov = packet_bytes[0] & MOUSE_PARSE_X_OVERFLOW;
    mouse_parsed_packet.y_ov = packet_bytes[0] & MOUSE_PARSE_Y_OVERFLOW;

    return 0;
}

uint8_t mouse_data_handler() {
    ++counter;

    if (counter > 2) {
        is_mouse_packet_complete = true;
        if (parse_packet())
            return 1;
        counter = 0;
    }
    else {
        is_mouse_packet_complete = false;
    }
    
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
