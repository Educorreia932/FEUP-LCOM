#include "switchboard.h"
#include "ui_elements.h"
#include "sprite.h"
#include "mouse_cursor.h"
#include "math_utils.h"

/* SWITCHBOARD MINIGAMES */

#define MINIGAME1_MAX_BUTTONS 7

#define MINIGAME2_MIN_COOLDOWN 2
#define MINIGAME2_MAX_COOLDOWN 8
#define MINIGAME2_REACTION_TIME 2
#define MINIGAME2_STATE1_X 0
#define MINIGAME2_STATE1_Y 0
#define MINIGAME2_STATE2_X 0
#define MINIGAME2_STATE2_Y 0


struct Minigame1 {
    SwitchBoard_t* sb; // Only a reference
    Sprite_t *sprite;
    Button_t **buttons; // Fixed size though
    void (*func)();
};

struct Minigame1* new_minigame1() {
    return NULL;
}

void free_minigame1(struct Minigame1* mg) {
    free_sprite(mg->sprite);
    Button_t **ptr = mg->buttons;
    for (uint8_t i = 0; i < MINIGAME1_MAX_BUTTONS; ++i) {
        if (*ptr != NULL)
            free_button_without_sprite(*ptr);
        ++ptr;
    }
}

void update_minigame1(struct Minigame1* mg) {

}

void render_minigame1(struct Minigame1* mg) {
    
}

/* MINIGAME 2 */
/* Connect them wires bro */

struct Minigame2 {
    Button_t *button;
    Sprite_t *state1_sprite, *state2_sprite, *warning_sprite;

};


/* SWITCHBOARD ITSELF */

struct SwitchBoard {
    Sprite_t *background;
    Button_t *laser_buttons[3];
    Slider_t *speed_slider, *jump_slider;
    Knob_t *gravity_knob;
    MouseCursor_t* cursor;
};

void print_laser1() {
    printf("Toggled to link id 1\n");
}

void print_laser2() {
    printf("Toggled to link id 2\n");
}

void print_laser3() {
    printf("Toggled to link id 3\n");
}

void print_speed_mult(uint8_t speed_mult) {
    printf("New speed mult is in state: %d\n", speed_mult);
}

void print_jump_mult(uint8_t jump_mult) {
    printf("New jump mult is in state: %d\n", jump_mult);
}

void print_gravity_time(float f) {
    printf("Gravity time (rounded to int): %d\n", (int32_t) f);
}

SwitchBoard_t* new_switchboard(MouseCursor_t *cursor) {
    if (cursor == NULL) {
        printf("new_switchboard: Cursor cannot be a NULL pointer");
        return NULL;
    }
    SwitchBoard_t* s_board = (SwitchBoard_t*) malloc(sizeof(SwitchBoard_t));
    if (s_board == NULL) {
        printf("new_switchboard: Failed to allocate memory for Switchboard object\n");
        return NULL;
    }
    s_board->cursor = cursor;
    s_board->background = new_sprite(0, 0, 1, "/home/lcom/labs/proj/assets/switchboard/switch_board.bmp");
    if (s_board->background == NULL) {
        printf("new_switchboard: Failed to create background Sprite\n");
        free(s_board);
        return NULL;
    }

    s_board->laser_buttons[0] = new_button_auto_size("/home/lcom/labs/proj/assets/switchboard/laser_button_red.bmp", print_laser1, vec2d(840, 180));
    if (s_board->laser_buttons[0] == NULL) {
        printf("new_switchboard: Failed to create laser button 0\n");
        free_sprite(s_board->background);
        free(s_board);
        return NULL;
    }
    s_board->laser_buttons[1] = new_button_auto_size("/home/lcom/labs/proj/assets/switchboard/laser_button_blue.bmp", print_laser2, vec2d(840, 280));
    if (s_board->laser_buttons[1] == NULL) {
        printf("new_switchboard: Failed to create laser button 1\n");
        free_sprite(s_board->background);
        free_button(s_board->laser_buttons[0]);
        free(s_board);
        return NULL;
    }
    s_board->laser_buttons[2] = new_button_auto_size("/home/lcom/labs/proj/assets/switchboard/laser_button_pink.bmp", print_laser3, vec2d(840, 380));
    if (s_board->laser_buttons[2] == NULL) {
        printf("new_switchboard: Failed to create laser button 2\n");
        free_sprite(s_board->background);
        free_button(s_board->laser_buttons[1]);
        free_button(s_board->laser_buttons[2]);
        free(s_board);
        return NULL;
    }

    s_board->speed_slider = new_slider("/home/lcom/labs/proj/assets/switchboard/speed_slider.bmp", "/home/lcom/labs/proj/assets/switchboard/speed_slider_handle.bmp", print_speed_mult, vec2d(160, 40), 255, vec2d(180, 50), vec2d(440, 50));
    if (s_board->speed_slider == NULL) {
        printf("new_switchboard: Failed to create horizontal slider\n");
    }
    s_board->jump_slider = new_slider("/home/lcom/labs/proj/assets/switchboard/jump_slider.bmp", "/home/lcom/labs/proj/assets/switchboard/jump_slider_handle.bmp", print_jump_mult, vec2d(20, 220), 255, vec2d(30, 240), vec2d(30, 500));
    if (s_board->jump_slider == NULL) {
        printf("new_switchboard: Failed to create vertical slider\n");
    }
    s_board->gravity_knob = new_knob("/home/lcom/labs/proj/assets/switchboard/large_knob.bmp", "/home/lcom/labs/proj/assets/switchboard/small_knob.bmp", print_gravity_time, vec2d(780.0f, 540.0f), 210.0f, 330.0f, 100.0f);
    if (s_board->gravity_knob == NULL) {
        printf("new_switchboard: Failed to create gravity knob\n");
        return NULL;
    }
    switchboard_set_two_lasers(s_board);
    // slider_deactivate(s_board->jump_slider);
    return s_board;
}

void free_switchboard(SwitchBoard_t* s_board) {
    free_button(s_board->laser_buttons[0]);
    free_button(s_board->laser_buttons[1]);
    free_button(s_board->laser_buttons[2]);
    free_slider(s_board->speed_slider);
    free_slider(s_board->jump_slider);
    free_knob(s_board->gravity_knob);
    free(s_board);
}

void switchboard_set_no_lasers(SwitchBoard_t* s_board) {
    button_deactivate(s_board->laser_buttons[0]);
    button_deactivate(s_board->laser_buttons[1]);
    button_deactivate(s_board->laser_buttons[2]);
}

void switchboard_set_two_lasers(SwitchBoard_t* s_board) {
    button_activate(s_board->laser_buttons[0]);
    button_activate(s_board->laser_buttons[1]);
    button_deactivate(s_board->laser_buttons[2]);
}

void switchboard_set_three_lasers(SwitchBoard_t* s_board) {
    button_activate(s_board->laser_buttons[0]);
    button_activate(s_board->laser_buttons[1]);
    button_activate(s_board->laser_buttons[2]);
}

void update_switchboard(SwitchBoard_t* s_board) {
    update_button(s_board->laser_buttons[0], s_board->cursor);
    update_button(s_board->laser_buttons[1], s_board->cursor);
    update_button(s_board->laser_buttons[2], s_board->cursor);
    update_slider(s_board->speed_slider, s_board->cursor);
    update_slider(s_board->jump_slider, s_board->cursor);
    update_knob(s_board->gravity_knob, s_board->cursor);
}

void render_switchboard(SwitchBoard_t* s_board) {
    draw_sprite_floats(s_board->background, 0, 0, COLOR_NO_MULTIPLY, false);
    
    render_button(s_board->laser_buttons[0]);
    render_button(s_board->laser_buttons[1]);
    render_button(s_board->laser_buttons[2]);
    
    render_slider(s_board->speed_slider);
    render_slider(s_board->jump_slider);
    render_knob(s_board->gravity_knob);
}
