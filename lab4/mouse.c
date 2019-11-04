#include "mouse.h"

// Global variables
bool mouse_ih_error = false, is_mouse_packet_complete = false;
struct packet mouse_parsed_packet;

// Static variables
static int mouse_hook_id;
static uint8_t st, counter = 0;
static uint8_t packet_bytes[3];
static bool found_first_byte = false;

uint8_t (mouse_subscribe_int)(uint8_t *bit_no) {
    if (!bit_no) // Check if pointer is NULL
        return 1;

    mouse_hook_id = MOUSE_IRQ;
    *bit_no = MOUSE_IRQ;	

    return sys_irqsetpolicy(MOUSE_IRQ, IRQ_EXCLUSIVE | IRQ_REENABLE, &mouse_hook_id);
}

uint8_t (mouse_unsubscribe_int)() {
    return sys_irqrmpolicy(&mouse_hook_id);
}

uint8_t mouse_enable_int() {
    return sys_irqenable(&mouse_hook_id);
}

uint8_t mouse_disable_int() {
    return sys_irqdisable(&mouse_hook_id);
}

// Has a more complicated structure than simple KBC commands,
// so this function handles that interaction
uint8_t mouse_send_cmd(uint8_t cmd) {

    // If a command's response is NACK, we need to retry all over again
    uint8_t attempts = 3;
    while (attempts) {

        // Write cmd to tell the KBC that we'll send a mouse cmd
        if (kbc_send_cmd(MOUSE_CTRL_REG, KBC_WRITE_BYTE_TO_MOUSE))
            return 1;
        // Write the cmd that will be sent to the mouse itself
        if (kbc_send_cmd(MOUSE_IN_BUF, cmd))
            return 1;

        // Make sure the mouse responded with ACK
        uint8_t byte_received;
        if (kbc_read_outbf(MOUSE_OUT_BUF, &byte_received, true))
            return 1;
        
        // If ACK return 0, it's all fine
        if (byte_received == MOUSE_CTRL_ACK)
            return 0;
        
        // If NACK, retry again
        if (byte_received == MOUSE_CTRL_NACK) {
            --attempts;
            continue;
        }
        
        // If it was MOUSE_CTR_ERROR or
        // if it was anything else... Something is REALLY wrong, return 1
        return 1;
    }

    return 1;
}

// Enables mouse data reporting
uint8_t mouse_data_reporting_enable() {
    return mouse_send_cmd(MOUSE_CMD_ENABLE_DATA_REPORT);
}

// Disables mosue data reporting
uint8_t mouse_data_reporting_disable() {
    return mouse_send_cmd(MOUSE_CMD_DISABLE_DATA_REPORT);
}

// For now unused, but just garantees that data reporting was
// disabled before sending the command, reenabling it afterwards
uint8_t mouse_send_cmd_stream_mode(uint8_t cmd) {
    if (mouse_data_reporting_disable())
        return 1;
    if (mouse_send_cmd(cmd))
        return 1;
    if (mouse_data_reporting_enable())
        return 1;
    return 0;
}

// Sets mouse to stream mode
uint8_t mouse_set_stream_mode() {
    return mouse_send_cmd(MOUSE_CMD_STREAM_MODE);
}

// Sets mouse to remote mode
uint8_t mouse_set_remote_mode() {
    return mouse_send_cmd(MOUSE_CMD_REMOTE_MODE);
}

// Sends cmd to mouse signaling we want to read a packet
uint8_t mouse_read_data() {
    return mouse_send_cmd(MOUSE_CMD_READ_DATA);
}

void (mouse_ih)() {
    if (util_sys_inb(STAT_REG, &st)) {
        mouse_ih_error = 1;
        uint8_t discarded_byte;
        util_sys_inb(OUT_BUF, &discarded_byte);
        return;        
    }

    // If either one is set to 1, there's an error
    if (st & (ST_PAR_ERR | ST_TO_ERR)) {
        mouse_ih_error = 1;
        uint8_t discarded_byte;
        util_sys_inb(OUT_BUF, &discarded_byte);
        return;
    }

    if (util_sys_inb(OUT_BUF, &packet_bytes[counter])) {
        mouse_ih_error = 1;
        return;
    }
}

uint8_t (parse_packet)() {
    // Copy raw bytes to the struct
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
    if (!found_first_byte) { // Business as usual
        ++counter;

        if (counter > 2) {
            is_mouse_packet_complete = true;
            
            if (parse_packet())
                return 1;

            counter = 0;
        }

        else 
            is_mouse_packet_complete = false;
        
        return 0;
    }

    else { // We need to find out the first byte
        // Assume that if that bit is 1, it's the first byte/ We need to find out the first byte
        // Assume that if that bit is 1, it's the first byte
        is_mouse_packet_complete = false;
        if (packet_bytes[counter] & MOUSE_PARSE_FIRST_PACKET_IDENTIFIER) {
            packet_bytes[0] = packet_bytes[counter];
            found_first_byte = true;
        }

        return 0;
    }
}
