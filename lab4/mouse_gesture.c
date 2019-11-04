#include "mouse_gesture.h"

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
    //printf("%u\n", state);
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


