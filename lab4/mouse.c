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

    if (sys_irqsetpolicy(MOUSE_IRQ, IRQ_EXCLUSIVE | IRQ_REENABLE, &mouse_hook_id))
        return 1;

    return 0;
}

uint8_t (mouse_unsubscribe_int)() {
    if (sys_irqrmpolicy(&mouse_hook_id))
        return 1;
                
    return 0;
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
        if (kbc_read_outbf(MOUSE_OUT_BUF, &byte_received))
            return 1;
        
        // If ACK return 0, it's all fine
        if (byte_received == MOUSE_CTRL_ACK)
            return 0;
        
        // If NACK, retry again
        if (byte_received == MOUSE_CTRL_NACK) {
            --attempts;
            continue;
        }
        
        // If it was anything else... Something is REALLY wrong, return 1
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

/* ----------------------------------- */
/* ---------- Gesture Stuff ---------- */
/* ----------------------------------- */

typedef enum {
    GSM_START = 0, GSM_LB, GSM_LINE_1, GSM_N_LB, GSM_RM, GSM_LINE_2
} GestureSM; // GSM = Gesture State Machine

typedef struct {
    bool LEFT_BUTTON_CLICK;
    bool RIGHT_BUTTON_CLICK;
    bool NO_BUTTON_CLICK;
    bool MOUSE_UP_MOVEMENT;
    bool TOLERANCE;
    bool MOUSE_DOWN_MOVEMENT;
} MouseEvents;

static MouseEvents mouse_events;
static const MouseEvents reset_mouse_events = {false, false, false, false, false, false};

void handle_events(uint8_t x_len, uint8_t tolerance) {
    mouse_events = reset_mouse_events;

    // Checks if ONLY left button is being clicked
    if (mouse_parsed_packet.lb && !mouse_parsed_packet.mb && !mouse_parsed_packet.rb)  
        mouse_events.LEFT_BUTTON_CLICK = true;
    
    // Checks if ONLY right button is being clicked
    if (mouse_parsed_packet.rb && !mouse_parsed_packet.mb && !mouse_parsed_packet.lb)
        mouse_events.RIGHT_BUTTON_CLICK = true;

    // Checks if the mouse moves upwards    
    if (mouse_parsed_packet.delta_x > 0 && mouse_parsed_packet.delta_y > 0)
        mouse_events.MOUSE_UP_MOVEMENT = true;

    // Check tolerence in the vertex
    if (abs(mouse_parsed_packet.delta_x) <= tolerance && abs(mouse_parsed_packet.delta_y) <= tolerance)
        mouse_events.TOLERANCE = true;

    // Checks if the mouse moves downwards
    if (mouse_parsed_packet.delta_x > 0 && mouse_parsed_packet.delta_y < 0)
        mouse_events.MOUSE_DOWN_MOVEMENT = true;

    // Checks if a button is being click
    if (!mouse_parsed_packet.lb && !mouse_parsed_packet.mb && !mouse_parsed_packet.rb)    
        mouse_events.NO_BUTTON_CLICK = true;

}

static GestureSM state = 0; // Sets initial state
static int x_displacement = 0;
static int y_displacement = 0;

uint8_t gesture_sm(uint8_t x_len, uint8_t tolerance) {
    printf("%u\n", state);
    //printf("y= %d\n x= %d\n", y_displacement, x_displacement);

    handle_events(x_len, tolerance);

    switch (state) {
        case GSM_START: // q0 ~ Starting state

            if (mouse_events.LEFT_BUTTON_CLICK) {
                // Reset displacement
                x_displacement = 0;
                y_displacement = 0;
                state = GSM_LB;
            } else
            { // This break here allows us to process the movement of the same
            // packet that the button was pressed down
                break;
            }

        case GSM_LB: // q1 ~ Waiting for x_len size line
            // If button is not pressed (or other buttons were pressed), go to start
            if (!mouse_events.LEFT_BUTTON_CLICK)
                state = GSM_START;
            
            else {
                x_displacement += mouse_parsed_packet.delta_x;
                y_displacement += mouse_parsed_packet.delta_y;

                // Check if it's still moving up or if the "error" was within the tolerance
                if (mouse_events.MOUSE_UP_MOVEMENT || mouse_events.TOLERANCE) {
                    // If x_displacement is >= x_len, this part of the line is "complete" (minimum)
                    if (x_displacement >= x_len)
                        state = GSM_LINE_1;
                }
                else
                    state = GSM_START;
            }

            break;

        case GSM_LINE_1: // q2 ~ Line already has intended size

            // If no button is pressed atm, we're at the vertex, so move to next state
            if (mouse_events.NO_BUTTON_CLICK) {
                // Make sure the user hasn't sneaked a dirty line past us
                // Only verify slope here, just before we go to vertex
                // NOTE: (y_displacement / x_displacement) >= 1 is the
                if (x_displacement >= x_len && (y_displacement >= x_displacement))
                    state = GSM_N_LB; // Vertex
                else
                    state = GSM_START; // It's invalid
            }
            else { // THIS ENTIRE ELSE IS IT TO KEEP DRAWING
                // If button is not pressed (or other buttons were pressed), go to start
                if (!mouse_events.LEFT_BUTTON_CLICK)
                    state = GSM_START;
                
                else {
                    x_displacement += mouse_parsed_packet.delta_x;
                    y_displacement += mouse_parsed_packet.delta_y;

                // Check if not (it's still moving up or if the "error" was within the tolerance)
                } if (!(mouse_events.MOUSE_UP_MOVEMENT || mouse_events.TOLERANCE))
                    state = GSM_START;
            }
    
            break;
        case GSM_N_LB: // q3 ~ Vertex        
            
            // If only right button was pressed, we move onto the next state, without break
            // because the movement (if any) will be processed in the next state
            if (mouse_events.RIGHT_BUTTON_CLICK) {
                // Also reset the displacement before moving forward
                x_displacement = 0;
                y_displacement = 0;
                state = GSM_RM;
            }

            // If any button is pressed (right click only was caught above) or if the movement was
            // past the acceptable tolerance, go to the start
            // if (!mouse_events.NO_BUTTON_CLICK || !mouse_events.TOLERANCE)
            else if (!(mouse_events.NO_BUTTON_CLICK && mouse_events.TOLERANCE)) {
                state = GSM_START;
                break;
            } else
                break;
            

        case GSM_RM: // q4 ~ Waiting for x_len length
            // If button is not pressed (or other buttons were pressed), go to start
            if (!mouse_events.RIGHT_BUTTON_CLICK) {
                state = GSM_START;
            }
            else {
                x_displacement += mouse_parsed_packet.delta_x;
                y_displacement += mouse_parsed_packet.delta_y;

                // Check if it's still moving down or if the "error" was within the tolerance
                if (mouse_events.MOUSE_DOWN_MOVEMENT || mouse_events.TOLERANCE) {
                    // If x_displacement is >= x_len, this part of the line is "complete" (minimum)
                    if (x_displacement >= x_len)
                        state = GSM_LINE_2;
                }
                else
                    state = GSM_START;
            }

            break;

        case GSM_LINE_2: // q5 ~ Line already has intended size

            // If no button is pressed atm, we'll be at the end, so we return 1
            if (mouse_events.NO_BUTTON_CLICK) { 
                // Make sure the user hasn't sneaked a dirty line past us
                // Only verify slope here, just before we end
                // NOTE: (y_displacement / x_displacement) <= -1 is the same
                if (x_displacement >= x_len && (-y_displacement >= x_displacement))
                    return 1;
                else
                    state = GSM_START; // It's invalid
            }
            else { // THIS ENTIRE ELSE IS IT TO KEEP DRAWING
                // If button is not pressed (or other buttons were pressed), go to start
                if (!mouse_events.RIGHT_BUTTON_CLICK)
                    state = GSM_START;
                
                else {
                    x_displacement += mouse_parsed_packet.delta_x;
                    y_displacement += mouse_parsed_packet.delta_y;

                    // Check if not (it's still moving down or if the "error" was within the tolerance)
                    if (!(mouse_events.MOUSE_DOWN_MOVEMENT || mouse_events.TOLERANCE))
                        state = GSM_START;
                }
            }
    
            break;

        default:
            break;
    }

    return 0;
}


