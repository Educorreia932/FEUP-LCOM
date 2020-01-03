#include "switchboard.h"
#include "ui_elements.h"
#include "sprite.h"
#include "mouse_cursor.h"
#include "math_utils.h"
#include "game_manager.h"
#include "hw_manager.h"

/* SWITCHBOARD MINIGAMES MACROS */

#define MINIGAME_INITIAL_WAIT_TIME 3
#define MINIGAME_WAIT_TIME 8
#define MINIGAME1_BUTTON_SPEED -3
#define MINIGAME1_SPAWN_RANGE 800
#define MINIGAME1_MAX_BUTTONS 8
#define MINIGAME1_GLITCH_TIME 45

/* Minigames definition for the Switchboard definition */

struct Minigame1 {
    Sprite_t *sprites[4], *sprite_mask;
    Button_t *buttons[MINIGAME1_MAX_BUTTONS]; // Fixed size though
    Rect_t death_zone;
    Rect_t active_zone;
    void (*on_loss)();
};

// Forward declarations to be used by the switchboard
static struct Minigame1* new_minigame1();
static void free_minigame1(struct Minigame1* mg);
static void update_minigame1(struct Minigame1* mg);
static void render_minigame1(struct Minigame1* mg);
static void minigame1_spawn_buttons(struct Minigame1* mg);


/* SWITCHBOARD ITSELF */

struct SwitchBoard {
    Sprite_t *background;
    Button_t *laser_buttons[3];
    Slider_t *speed_slider, *jump_slider;
    Knob_t *gravity_knob;

    bool set_first_alarm;
    struct Minigame1 *mg1;

    PlayerUnlockedPowers default_powers, current_powers;
    bool is_player_alive;

    uint16_t time_to_reset_rendering;
};

void switchboard_start_minigame(SwitchBoard_t* s_board) {
    hw_manager_rtc_set_alarm(MINIGAME_WAIT_TIME);

    minigame1_spawn_buttons(s_board->mg1);
}

static void send_laser0() {
    hw_manager_uart_send_char(HEADER_LASER);
    hw_manager_uart_send_char(0);
    hw_manager_uart_send_char(HEADER_TERMINATOR);
}

static void send_laser1() {
    hw_manager_uart_send_char(HEADER_LASER);
    hw_manager_uart_send_char(1);
    hw_manager_uart_send_char(HEADER_TERMINATOR);
}

static void send_laser2() {
    hw_manager_uart_send_char(HEADER_LASER);
    hw_manager_uart_send_char(2);
    hw_manager_uart_send_char(HEADER_TERMINATOR);
}

static void send_speed_mult(uint8_t speed_mult) {
    hw_manager_uart_send_char(HEADER_SPEED_MULT);
    hw_manager_uart_send_char(speed_mult);
    hw_manager_uart_send_char(HEADER_TERMINATOR);
}

static void send_jump_mult(uint8_t jump_mult) {
    hw_manager_uart_send_char(HEADER_JUMP_MULT);
    hw_manager_uart_send_char(jump_mult);
    hw_manager_uart_send_char(HEADER_TERMINATOR);
}

static void send_gravity_normal() {
    hw_manager_uart_send_char(HEADER_GRAVITY);
    hw_manager_uart_send_char(UART_GRAVITY_NORMAL);
    hw_manager_uart_send_char(HEADER_TERMINATOR);
}

static void send_gravity_reversed() {
    hw_manager_uart_send_char(HEADER_GRAVITY);
    hw_manager_uart_send_char(UART_GRAVITY_REVERSED);
    hw_manager_uart_send_char(HEADER_TERMINATOR);
}

static void switchboard_update_powers(SwitchBoard_t* sw) {
    slider_set_activation(sw->speed_slider, sw->current_powers & UNLOCKED_SPEED);
    slider_set_activation(sw->jump_slider, sw->current_powers & UNLOCKED_JUMP);
    button_set_activation(sw->laser_buttons[0], sw->current_powers & UNLOCKED_LASERS);
    button_set_activation(sw->laser_buttons[1], sw->current_powers & UNLOCKED_LASERS);
    button_set_activation(sw->laser_buttons[2], sw->current_powers & UNLOCKED_LASERS);
    knob_set_activation(sw->gravity_knob, sw->current_powers & UNLOCKED_GRAVITY);
    knob_reset_state(sw->gravity_knob);
}

void switchboard_set_default_powers(SwitchBoard_t* sw, PlayerUnlockedPowers default_powers) {
    sw->default_powers = default_powers;
    sw->current_powers = default_powers;
    switchboard_update_powers(sw);
}

void switchboard_unlock_powers(SwitchBoard_t* sw, PlayerUnlockedPowers new_powers) {
    sw->current_powers |= new_powers;
    switchboard_update_powers(sw);
}

void switchboard_player_respawn(SwitchBoard_t* sw) {
    sw->current_powers = sw->default_powers;
    switchboard_update_powers(sw);
}

SwitchBoard_t* new_switchboard() {
    SwitchBoard_t* s_board = (SwitchBoard_t*) malloc(sizeof(SwitchBoard_t));
    if (s_board == NULL) {
        printf("new_switchboard: Failed to allocate memory for Switchboard object\n");
        return NULL;
    }

    s_board->background = new_sprite(0, 0, 1, "ui/switch_board.bmp");
    if (s_board->background == NULL) {
        printf("new_switchboard: Failed to create background Sprite\n");
        free(s_board);
        return NULL;
    }

    s_board->laser_buttons[0] = new_button_auto_size("ui/laser_button_red.bmp", send_laser0, vec2d(840, 180));
    if (s_board->laser_buttons[0] == NULL) {
        printf("new_switchboard: Failed to create laser button 0\n");
        free_sprite(s_board->background);
        free(s_board);
        return NULL;
    }
    
    s_board->laser_buttons[1] = new_button_auto_size("ui/laser_button_blue.bmp", send_laser1, vec2d(840, 280));
    if (s_board->laser_buttons[1] == NULL) {
        printf("new_switchboard: Failed to create laser button 1\n");
        free_sprite(s_board->background);
        free_button(s_board->laser_buttons[0]);
        free(s_board);
        return NULL;
    }

    s_board->laser_buttons[2] = new_button_auto_size("ui/laser_button_pink.bmp", send_laser2, vec2d(840, 380));
    if (s_board->laser_buttons[2] == NULL) {
        printf("new_switchboard: Failed to create laser button 2\n");
        free_sprite(s_board->background);
        free_button(s_board->laser_buttons[0]);
        free_button(s_board->laser_buttons[1]);
        free(s_board);
        return NULL;
    }

    s_board->speed_slider = new_slider("ui/speed_slider.bmp", "ui/speed_slider_handle.bmp", send_speed_mult, vec2d(160, 40), 255, vec2d(180, 50), vec2d(440, 50));
    if (s_board->speed_slider == NULL) {
        free_sprite(s_board->background);
        free_button(s_board->laser_buttons[0]);
        free_button(s_board->laser_buttons[1]);
        free_button(s_board->laser_buttons[2]);
        free(s_board);
        printf("new_switchboard: Failed to create horizontal slider\n");
    }

    s_board->jump_slider = new_slider("ui/jump_slider.bmp", "ui/jump_slider_handle.bmp", send_jump_mult, vec2d(20, 220), 255, vec2d(30, 240), vec2d(30, 500));
    if (s_board->jump_slider == NULL) {
        free_sprite(s_board->background);
        free_button(s_board->laser_buttons[0]);
        free_button(s_board->laser_buttons[1]);
        free_button(s_board->laser_buttons[2]);
        free_slider(s_board->speed_slider);
        free(s_board);
        printf("new_switchboard: Failed to create vertical slider\n");
    }
    
    s_board->gravity_knob = new_knob("ui/large_knob.bmp", "ui/small_knob.bmp", send_gravity_reversed, send_gravity_normal, 0.006f, vec2d(780.0f, 540.0f), 210.0f, 330.0f, 100.0f);
    if (s_board->gravity_knob == NULL) {
        printf("new_switchboard: Failed to create gravity knob\n");
        free_sprite(s_board->background);
        free_button(s_board->laser_buttons[0]);
        free_button(s_board->laser_buttons[1]);
        free_button(s_board->laser_buttons[2]);
        free_slider(s_board->speed_slider);
        free_slider(s_board->jump_slider);
        free(s_board);
        return NULL;
    }
    
    s_board->mg1 = new_minigame1();
    if (s_board->mg1 == NULL) {
        printf("new_switchboard: Failed to create the minigame 1\n");
        free_sprite(s_board->background);
        free_button(s_board->laser_buttons[0]);
        free_button(s_board->laser_buttons[1]);
        free_button(s_board->laser_buttons[2]);
        free_slider(s_board->speed_slider);
        free_slider(s_board->jump_slider);
        free_knob(s_board->gravity_knob);
        free(s_board);
    }

    s_board->set_first_alarm = true;

    return s_board;
}

void free_switchboard(SwitchBoard_t* s_board) {
    free_button(s_board->laser_buttons[0]);
    free_button(s_board->laser_buttons[1]);
    free_button(s_board->laser_buttons[2]);
    free_slider(s_board->speed_slider);
    free_slider(s_board->jump_slider);
    free_knob(s_board->gravity_knob);
    free_minigame1(s_board->mg1);
    free(s_board);
}

void update_switchboard(SwitchBoard_t* s_board) {
    update_button(s_board->laser_buttons[0]);
    update_button(s_board->laser_buttons[1]);
    update_button(s_board->laser_buttons[2]);
    update_slider(s_board->speed_slider);
    update_slider(s_board->jump_slider);
    update_knob(s_board->gravity_knob);

    if (s_board->set_first_alarm) {
        hw_manager_rtc_set_alarm(MINIGAME_INITIAL_WAIT_TIME);
        s_board->set_first_alarm = false;
    }
    update_minigame1(s_board->mg1);

    if (s_board->time_to_reset_rendering == 0) {
        get_game_manager()->normal_rendering = true;
    }
    else {
        get_game_manager()->normal_rendering = false;
        --s_board->time_to_reset_rendering;
    }
}

void render_switchboard(SwitchBoard_t* s_board) {
    draw_sprite_floats(s_board->background, 0, 0, COLOR_NO_MULTIPLY, SPRITE_NORMAL);
    
    render_minigame1(s_board->mg1);

    render_button(s_board->laser_buttons[0]);
    render_button(s_board->laser_buttons[1]);
    render_button(s_board->laser_buttons[2]);
    
    render_slider(s_board->speed_slider);
    render_slider(s_board->jump_slider);
    render_knob(s_board->gravity_knob);
}


/* MINIGAMES */
// These 8 next functions are to avoid having to implement and ID system into the UI buttons
static void mg_1_db_0() {
    Button_t **b = &get_game_manager()->s_board->mg1->buttons[0];
    if (*b != NULL) {
        free_button_without_sprite(*b);
        *b = NULL;
    }
}
static void mg_1_db_1() {
    Button_t **b = &get_game_manager()->s_board->mg1->buttons[1];
    if (*b != NULL) {
        free_button_without_sprite(*b);
        *b = NULL;
    }
}
static void mg_1_db_2() {
    Button_t **b = &get_game_manager()->s_board->mg1->buttons[2];
    if (*b != NULL) {
        free_button_without_sprite(*b);
        *b = NULL;
    }
}
static void mg_1_db_3() {
    Button_t **b = &get_game_manager()->s_board->mg1->buttons[3];
    if (*b != NULL) {
        free_button_without_sprite(*b);
        *b = NULL;
    }
}
static void mg_1_db_4() {
    Button_t **b = &get_game_manager()->s_board->mg1->buttons[4];
    if (*b != NULL) {
        free_button_without_sprite(*b);
        *b = NULL;
    }
}
static void mg_1_db_5() {
    Button_t **b = &get_game_manager()->s_board->mg1->buttons[5];
    if (*b != NULL) {
        free_button_without_sprite(*b);
        *b = NULL;
    }
}
static void mg_1_db_6() {
    Button_t **b = &get_game_manager()->s_board->mg1->buttons[6];
    if (*b != NULL) {
        free_button_without_sprite(*b);
        *b = NULL;
    }
}
static void mg_1_db_7() {
    Button_t **b = &get_game_manager()->s_board->mg1->buttons[7];
    if (*b != NULL) {
        free_button_without_sprite(*b);
        *b = NULL;
    }
}

static void (*minigame_1_delete_button[MINIGAME1_MAX_BUTTONS])() = {
    mg_1_db_0, mg_1_db_1, mg_1_db_2, mg_1_db_3, mg_1_db_4, mg_1_db_5, mg_1_db_6, mg_1_db_7
};

static void glitched_rendering_mg1() {
    get_game_manager()->s_board->time_to_reset_rendering += MINIGAME1_GLITCH_TIME;
}

static struct Minigame1* new_minigame1() {
    struct Minigame1* mg1 = (struct Minigame1*) calloc(1, sizeof(struct Minigame1));
    if (mg1 == NULL) {
        printf("new_minigame1: Failed to allocate memory for the minigame 1 object\n");
        return NULL;
    }

    mg1->sprites[0] = new_sprite(0, 0, 1, "ui/storage_component_1k.bmp");
    if (mg1->sprites[0] == NULL) {
        printf("new_minigame1: Failed to create the Sprite 0\n");
        free(mg1);
        return NULL;
    }  
    mg1->sprites[1] = new_sprite(0, 0, 1, "ui/storage_component_4k.bmp");
    if (mg1->sprites[1] == NULL) {
        printf("new_minigame1: Failed to create the Sprite 1\n");
        free_sprite(mg1->sprites[0]);
        free(mg1);
        return NULL;
    }
    mg1->sprites[2] = new_sprite(0, 0, 1, "ui/storage_component_16k.bmp");
    if (mg1->sprites[2] == NULL) {
        printf("new_minigame1: Failed to create the Sprite 2\n");
        free_sprite(mg1->sprites[0]);
        free_sprite(mg1->sprites[1]);
        free(mg1);
        return NULL;
    }
    mg1->sprites[3] = new_sprite(0, 0, 1, "ui/storage_component_64k.bmp");
    if (mg1->sprites[3] == NULL) {
        printf("new_minigame1: Failed to create the Sprite 3\n");
        free_sprite(mg1->sprites[0]);
        free_sprite(mg1->sprites[1]);
        free_sprite(mg1->sprites[2]);
        free(mg1);
        return NULL;
    }

    mg1->sprite_mask = new_sprite(0, 0, 1, "ui/mg1_mask.bmp");
    if (mg1->sprite_mask == NULL) {
        printf("new_minigame1: Failed to create the Mask Sprite\n");
        free_sprite(mg1->sprites[0]);
        free_sprite(mg1->sprites[1]);
        free_sprite(mg1->sprites[2]);
        free_sprite(mg1->sprites[3]);
        free(mg1);
        return NULL;
    }

    mg1->death_zone = rect(152, 61, 277, 20);
    mg1->active_zone = rect(152, 714, 277, 20);
    mg1->on_loss = glitched_rendering_mg1;

    return mg1;
}

static void free_minigame1(struct Minigame1* mg) {
    free_sprite(mg->sprites[0]);
    free_sprite(mg->sprites[1]);
    free_sprite(mg->sprites[2]);
    free_sprite(mg->sprites[3]);
    for (uint8_t i = 0; i < MINIGAME1_MAX_BUTTONS; ++i) {
        if (mg->buttons[i] != NULL)
            free_button_without_sprite(mg->buttons[i]);
    }
    free(mg);
}

void minigame1_spawn_buttons(struct Minigame1* mg) {

    uint rng = rand();
    uint8_t n = rng % 5 + 3;
    for (uint8_t i = 0; i < MINIGAME1_MAX_BUTTONS && n > 0; ++i) {
        if (mg->buttons[i] == NULL) {
            mg->buttons[i] = new_button_existing_sprite(mg->sprites[rng % 4], minigame_1_delete_button[i], rect(rng % 213 + 152, rng % MINIGAME1_SPAWN_RANGE + 714, 64, 64));
            if (mg->buttons[i] != NULL) {
                button_hide(mg->buttons[i]);
                --n;
            }
            rng = rand();
        }
    }
}

static void update_minigame1(struct Minigame1* mg) {
    for (uint8_t i = 0; i < MINIGAME1_MAX_BUTTONS; ++i) {
        if (mg->buttons[i] != NULL) {
            if (rect_collision(button_get_rect(mg->buttons[i]), &mg->active_zone)) {
                button_show(mg->buttons[i]);
            }
            button_move(mg->buttons[i], vec2d(0, MINIGAME1_BUTTON_SPEED));
            if (rect_collision(button_get_rect(mg->buttons[i]), &mg->death_zone)) {
                minigame_1_delete_button[i]();
                mg->on_loss();
            }
            else {
                update_button(mg->buttons[i]);
            }   
        }
    }
}

static void render_minigame1(struct Minigame1* mg) {
    for (uint8_t i = 0; i < MINIGAME1_MAX_BUTTONS; ++i) {
        if (mg->buttons[i] != NULL) {
            render_button(mg->buttons[i]);
        }
    }

    draw_sprite_floats(mg->sprite_mask, 141, 57, COLOR_NO_MULTIPLY, SPRITE_NORMAL);
}

/* MINIGAME 2 */
/* Connect them wires bro */

struct Minigame2 {
    Button_t *button;
    Sprite_t *state1_sprite, *state2_sprite, *warning_sprite;

};
