#include "ui_elements.h"
#include "input_events.h"
#include "mouse_cursor.h"
#include "math_utils.h"

/* COLOR CHANGES IN THE UI */
#define COLOR_WHEN_HOVERED 0xEF5D
#define COLOR_INACTIVE 0x9492

#define KNOB_ANGLE_PRECISION 0.1f
#define KNOB_ANGLE_RETAKE_CONTROL_PRECISION 0.25f

/** @name Button */
///@{
struct Button {
    Rect_t rect;
    Sprite_t* sprite;
    bool shown, is_active, hovered;
    void (*func)();
};

Button_t* new_button(const char* sprite_file_name, void (*func)(), Rect_t rect) {
    Button_t* button = (Button_t*) malloc(sizeof(Button_t));
    
    if (button == NULL) {
        printf("new_button: Failed to allocate memory for the Button\n");
        return NULL;
    }
    
    button->rect = rect;

    if (strcmp("", sprite_file_name) == 0)
        button->sprite = NULL;
    
    else {
        button->sprite = new_sprite(0, 0, 1, sprite_file_name);
    
        if (button->sprite == NULL) {
            printf("new_button: Failed to create Button's Sprite\n");
            free(button);
            return NULL;
        }
    }

    button->is_active = true;
    button->shown = true;
    button->hovered = false;

    if (func == NULL) {
        printf("new_button: Function pointer invalid\n");
        free_sprite(button->sprite);
        free(button);
        return NULL;
    }

    button->func = func;

    return button;
}

Button_t* new_button_auto_size(const char* sprite_file_name, void (*func)(), Vec2d_t pos) {
    
    Button_t* button = (Button_t*) malloc(sizeof(Button_t));
    if (button == NULL) {
        printf("new_button: Failed to allocate memory for the Button\n");
        return NULL;
    }
    button->sprite = new_sprite(0, 0, 1, sprite_file_name);
    if (button->sprite == NULL) {
        printf("new_button: Failed to create Button's Sprite\n");
        free(button);
        return NULL;
    }
    button->rect = rect(pos.x, pos.y, sprite_get_width(button->sprite), sprite_get_height(button->sprite));
    button->is_active = true;
    button->shown = true;
    button->hovered = false;
    if (func == NULL) {
        printf("new_button: Function pointer invalid\n");
        free_sprite(button->sprite);
        free(button);
        return NULL;
    }
    button->func = func;
    return button;
}

Button_t* new_button_existing_sprite(Sprite_t *sprite, void (*func)(), Rect_t rect) {
    
    Button_t* button = (Button_t*) malloc(sizeof(Button_t));
    if (button == NULL) {
        printf("new_button_existing_sprite: Failed to allocate memory for the Button\n");
        return NULL;
    }
    button->rect = rect;
    if (sprite == NULL) {
        printf("new_button_existing_sprite: Sprite cannot be a NULL pointer\n");
        free(button);
        return NULL;
    }
    button->sprite = sprite;
    button->is_active = true;
    button->shown = true;
    button->hovered = false;
    if (func == NULL) {
        printf("new_button_existing_sprite: Function pointer invalid\n");
        free(button);
        return NULL;
    }
    button->func = func;
    return button;
}

void free_button(Button_t* button) {
    if (button == NULL) {
        printf("free_button_and_sprite: Cannot free a NULL pointer\n");
        return;
    }
    if (button->sprite != NULL)
        free_sprite(button->sprite);
    free(button);
}

void free_button_without_sprite(Button_t* button) {
    if (button == NULL) {
        printf("free_button: Cannot free a NULL pointer\n");
        return;
    }
    free(button);
}

inline void button_hide(Button_t* button) {
    button->shown = false;
}

inline void button_show(Button_t* button) {
    button->shown = true;
}

inline void button_activate(Button_t* button) {
    button->is_active = true;
}

inline void button_deactivate (Button_t* button) {
    button->is_active = false;
}

inline void button_set_activation(Button_t* button, bool is_active) {
    button->is_active = is_active;
}

Rect_t* button_get_rect(Button_t* button) {
    return &button->rect;
}

void button_move(Button_t* button, Vec2d_t delta) {
    button->rect.x += delta.x;
    button->rect.y += delta.y;
}

void update_button(Button_t* button) {
    if (button->is_active && button->shown) {
        if (is_cursor_inside_rect(&button->rect)) {
            button->hovered = true;
            if (mouse_get_lb_down())
                button->func();
        }
        else
            button->hovered = false;
    }
}

void render_button(Button_t* button) {
    if (button->shown && button->sprite != NULL) {
        if (!button->is_active)
            draw_sprite(button->sprite, &button->rect, COLOR_INACTIVE, SPRITE_NORMAL);
        else if (button->hovered)
            draw_sprite(button->sprite, &button->rect, COLOR_WHEN_HOVERED, SPRITE_NORMAL);
        else
            draw_sprite(button->sprite, &button->rect, COLOR_NO_MULTIPLY, SPRITE_NORMAL);
    }
}

///@}

/** @name Slider */
///@{

struct Slider {
    Vec2d_t bar_pos;
    Rect_t handle_rect;
    Sprite_t *bar_sprite, *handle_sprite;
    bool shown, is_active, hovered, being_moved;
    // state is in [0, max_state[
    uint8_t state, max_state; // Also represents the icon's position in the bar
    Vec2d_t start_pos, end_pos, cursor_offset;
    void (*func)(uint8_t);
};

Slider_t* new_slider(const char* bar_sprite_file_name, const char* handle_sprite_file_name, void (*func)(uint8_t), Vec2d_t bar_pos, uint8_t max_state, Vec2d_t start_pos, Vec2d_t end_pos) {
    
    Slider_t* slider = (Slider_t*) malloc(sizeof(Slider_t));
    if (slider == NULL) {
        printf("new_slider: Failed to allocate memory for the Slider\n");
        return NULL;
    }
    slider->bar_pos = bar_pos;
    slider->bar_sprite = new_sprite(0, 0, 1, bar_sprite_file_name);
    if (slider->bar_sprite == NULL) {
        printf("new_slider: Failed to create Slider's bar Sprite\n");
        free(slider);
        return NULL;
    }
    slider->handle_sprite = new_sprite(0, 0, 1, handle_sprite_file_name);
    if (slider->handle_sprite == NULL) {
        printf("new_slider: Failed to create Slider's handle Sprite\n");
        free_sprite(slider->bar_sprite);
        free(slider);
        return NULL;
    }
    slider->is_active = true;
    slider->shown = true;
    slider->hovered = true;
    slider->being_moved = false;
    slider->state = 0;
    slider->max_state = max_state;
    slider->start_pos = start_pos;
    slider->end_pos = end_pos;
    slider->cursor_offset = vec2d(0.0f, 0.0f);
    slider->handle_rect = rect(
        start_pos.x + (end_pos.x - start_pos.x) / 2.0f,
        start_pos.y + (end_pos.y - start_pos.y) / 2.0f,
        sprite_get_width(slider->handle_sprite),
        sprite_get_height(slider->handle_sprite)
    );
    if (func == NULL) {
        printf("new_slider: Function pointer invalid\n");
        free_sprite(slider->bar_sprite);
        free_sprite(slider->handle_sprite);
        free(slider);
        return NULL;
    }
    slider->func = func;
    return slider;
}

void free_slider(Slider_t* slider) {
    if (slider == NULL) {
        printf("free_slider_and_sprite: Cannot free a NULL pointer\n");
        return;
    }
    free_sprite(slider->bar_sprite);
    free_sprite(slider->handle_sprite);
    free(slider);
}

inline void slider_hide(Slider_t* slider) {
    slider->shown = false;
}

inline void slider_show(Slider_t* slider) {
    slider->shown = true;
}

inline void slider_activate(Slider_t* slider) {
    slider->is_active = true;
}

inline void slider_deactivate (Slider_t* slider) {
    slider->is_active = false;
}

inline void slider_set_activation(Slider_t* slider, bool is_active) {
    slider->is_active = is_active;
}

void update_slider(Slider_t* slider) {
    if (slider->is_active && slider->shown) {
        if (slider->being_moved) {
            // Update position
            slider->handle_rect.x = fclampf(
                cursor_get_x() + slider->cursor_offset.x, slider->start_pos.x, slider->end_pos.x);
            
            slider->handle_rect.y = fclampf(
                cursor_get_y() + slider->cursor_offset.y,
                slider->start_pos.y, slider->end_pos.y);
            if (!mouse_get_lb()) {
                // Get the most appropriate state
                Vec2d_t p = rect_get_origin(&slider->handle_rect);
                float endpoint_distace = distance_vec2d(slider->start_pos, slider->end_pos);
                float distance_to_start = distance_vec2d(slider->start_pos, p);
                // Calculate closest possible state
                slider->state = (uint8_t) floorf(distance_to_start / endpoint_distace * slider->max_state);
                slider->being_moved = false;
                slider->func(slider->state);
            }
        }
        else if (is_cursor_inside_rect(&slider->handle_rect)) {
            slider->hovered = true;
            if (mouse_get_lb_down()) {
                slider->cursor_offset = subtract_vec2d(
                    rect_get_origin(&slider->handle_rect),
                    vec2d(  
                            cursor_get_x(),
                            cursor_get_y()
                        )
                    );
                slider->being_moved = true;
            }
        }
        else
            slider->hovered = false;
    }
}

void render_slider(Slider_t* slider) {
    if (slider->shown) {
        if (!slider->is_active) {
            // Draw default bar
            draw_sprite_floats(slider->bar_sprite, slider->bar_pos.x, slider->bar_pos.y, COLOR_INACTIVE, SPRITE_NORMAL);
            // Draw handle
            draw_sprite(slider->handle_sprite, &slider->handle_rect, COLOR_INACTIVE, SPRITE_NORMAL);
        }
        else if (slider->hovered) {
            // Draw default bar
            draw_sprite_floats(slider->bar_sprite, slider->bar_pos.x, slider->bar_pos.y, COLOR_NO_MULTIPLY, SPRITE_NORMAL);
            // Draw handle
            draw_sprite(slider->handle_sprite, &slider->handle_rect, COLOR_WHEN_HOVERED, SPRITE_NORMAL);
        }
        else {
            // Draw default bar
            draw_sprite_floats(slider->bar_sprite, slider->bar_pos.x, slider->bar_pos.y, COLOR_NO_MULTIPLY, SPRITE_NORMAL);
            // Draw handle
            draw_sprite(slider->handle_sprite, &slider->handle_rect, COLOR_NO_MULTIPLY, SPRITE_NORMAL);
        }
    }
}

///@}

/** @name Knob */
///@{

enum KnobAngleLock {
    ANGLE_NO_LOCK = 0,
    ANGLE_LOCK_START,
    ANGLE_LOCK_END
};

struct Knob {
    Sprite_t *backdrop_sprite, *knob_sprite;
    Rect_t backdrop_rect, knob_rect;
    Vec2d_t center;
    bool shown, is_active, being_moved, hovered;
    float radius, start_angle, end_angle, angle_offset;
    float angle_setback;
    enum KnobAngleLock angle_lock;
    bool has_reset;
    void (*on_drop)(), (*on_reset)();
};

Knob_t* new_knob(const char* backdrop_sprite_file_name, const char* knob_sprite_file_name, void (*on_drop)(), void (*on_reset)(), float angle_setback, Vec2d_t pos, float start_angle, float end_angle, float radius) {    
    Knob_t* knob = (Knob_t*) malloc(sizeof(Knob_t));
    if (knob == NULL) {
        printf("new_knob: Failed to allocate memory for the Knob\n");
        return NULL;
    }
    knob->backdrop_sprite = new_sprite(0, 0, 1, backdrop_sprite_file_name);
    if (knob->backdrop_sprite == NULL) {
        printf("new_knob: Failed to create Knob's backdrop Sprite\n");
        free(knob);
        return NULL;
    }
    knob->backdrop_rect = rect_from_vec2d(
        pos,
        vec2d(
            sprite_get_width(knob->backdrop_sprite),
            sprite_get_height(knob->backdrop_sprite)
            )
        );
    knob->center = sum_vec2d(pos, vec2d(
        sprite_get_width(knob->backdrop_sprite) / 2.0f,
        sprite_get_height(knob->backdrop_sprite) / 2.0f
        ));
    knob->knob_sprite = new_sprite(0, 0, 1, knob_sprite_file_name);
    if (knob->knob_sprite == NULL) {
        printf("new_knob: Failed to create Knob's handle Sprite\n");
        free_sprite(knob->backdrop_sprite);
        free(knob);
        return NULL;
    }
    knob->is_active = true;
    knob->shown = true;
    knob->hovered = true;
    knob->being_moved = false;
    knob->radius = radius;
    knob->start_angle = start_angle * (2 * M_PI) / 360.0f;
    knob->end_angle = end_angle * (2 * M_PI) / 360.0f;
    knob->angle_offset = 0.0f;
    knob->angle_lock = ANGLE_NO_LOCK;
    knob->angle_setback = fminf(angle_setback, KNOB_ANGLE_PRECISION / 2);
    knob->has_reset = true;
    knob->knob_rect = rect_from_vec2d(
        subtract_vec2d(
            circumference_vec2d(knob->center, radius, knob->start_angle),
            vec2d(
                    sprite_get_width(knob->knob_sprite) / 2.0f,
                    sprite_get_height(knob->knob_sprite) / 2.0f
                )
            ),
        vec2d(
            sprite_get_width(knob->knob_sprite),
            sprite_get_height(knob->knob_sprite)
            )
        );
    if (on_drop == NULL || on_reset == NULL) {
        printf("new_knob: Function pointer invalid\n");
        free_sprite(knob->backdrop_sprite);
        free_sprite(knob->knob_sprite);
        free(knob);
        return NULL;
    }
    knob->on_drop = on_drop;
    knob->on_reset = on_reset;

    return knob;
}

void free_knob(Knob_t* knob) {
    if (knob == NULL) {
        printf("free_knob_and_sprite: Cannot free a NULL pointer\n");
        return;
    }
    free_sprite(knob->backdrop_sprite);
    free_sprite(knob->knob_sprite);
    free(knob);
}

inline void knob_hide(Knob_t* knob) {
    knob->shown = false;
}

inline void knob_show(Knob_t* knob) {
    knob->shown = true;
}

inline void knob_activate(Knob_t* knob) {
    knob->is_active = true;
}

inline void knob_deactivate (Knob_t* knob) {
    knob->is_active = false;
}

inline void knob_set_activation(Knob_t* knob, bool is_active) {
    knob->is_active = is_active;
}

static float knob_get_cursor_angle(Knob_t *knob) {
    float angle = angle_vec2d(vec2d(1, 0), subtract_vec2d(                        cursor_get_pos(), knob->center));
    
    angle += knob->angle_offset;

    if (knob->center.y > cursor_get_y() + (knob->radius * sinf(knob->angle_offset)))
        angle = 2 * M_PI - angle;
    
    return angle;
}

static float knob_get_cur_angle(Knob_t *knob) {

    float cur_angle = angle_vec2d(
        vec2d(1, 0),
        subtract_vec2d(
            sum_vec2d(
                rect_get_origin(&knob->knob_rect),
                vec2d(
                    sprite_get_width(knob->knob_sprite) / 2.0f, sprite_get_height(knob->knob_sprite) / 2.0f)
                ),
            knob->center
        )
    );

    if (knob->center.y > knob->knob_rect.y + sprite_get_height(knob->knob_sprite) / 2.0f)
        cur_angle = 2 * M_PI - cur_angle;
    
    return cur_angle;
}

static void knob_update_pos(Knob_t *knob, float angle) {

    Vec2d_t pos = circumference_vec2d(knob->center, knob->radius, 
                fclampf(angle, knob->start_angle, knob->end_angle));

    pos = subtract_vec2d(pos, vec2d(sprite_get_width(knob->knob_sprite) / 2.0f, sprite_get_height(knob->knob_sprite) / 2.0f));

    knob->knob_rect.x = pos.x;
    knob->knob_rect.y = pos.y;

}

void knob_reset_state(Knob_t *knob) {
    knob_update_pos(knob, knob->start_angle);
}

void update_knob(Knob_t *knob) {
    if (knob->is_active && knob->shown) {
        // Knob is in movement
        if (knob->being_moved) {
            // Update position
            float angle = knob_get_cursor_angle(knob);

            if (knob->angle_lock == ANGLE_NO_LOCK) {
                if (angle <= knob->start_angle)
                    knob->angle_lock = ANGLE_LOCK_START;
                else if (angle >= knob->end_angle)
                    knob->angle_lock = ANGLE_LOCK_END;
                else
                    angle = fclampf(angle, knob->start_angle, knob->end_angle);
            }
            
            if (knob->angle_lock != ANGLE_NO_LOCK) {
                if (knob->angle_lock == ANGLE_LOCK_START) {
                    if (fabs(knob->start_angle - angle) <= KNOB_ANGLE_RETAKE_CONTROL_PRECISION) {
                        knob->angle_lock = ANGLE_NO_LOCK;
                        angle = fclampf(angle, knob->start_angle, knob->end_angle);
                    }
                    else
                        angle = knob->start_angle;
                }
                else {
                    if (fabs(knob->end_angle - angle) <= KNOB_ANGLE_RETAKE_CONTROL_PRECISION) {
                        knob->angle_lock = ANGLE_NO_LOCK;
                        angle = fclampf(angle, knob->start_angle, knob->end_angle);
                    }
                    else
                        angle = knob->end_angle;   
                }
            }
            
            knob_update_pos(knob, angle);
            
            if (!mouse_get_lb()) {
                knob->being_moved = false;
                knob->angle_lock = ANGLE_NO_LOCK;
                knob->on_drop();
                knob->has_reset = false;
            }
        }
        else {

            // Currently NOT being moved
            float cur_angle = knob_get_cur_angle(knob);
            if (fabs(cur_angle) > fabs(knob->start_angle) + KNOB_ANGLE_PRECISION) {
                // Resets itself
                cur_angle -= knob->angle_setback;
                knob_update_pos(knob, fmaxf(cur_angle, knob->start_angle));
            }
            else {
                if (!knob->has_reset) {
                    knob->has_reset = true;
                    knob->on_reset();
                }

                if (is_cursor_inside_rect(&knob->knob_rect)) {

                    // Mouse is hovering on the knob
                    knob->hovered = true;

                    if (mouse_get_lb_down()) {

                        // cur_angle was calculated just before
                        float cursor_angle = angle_vec2d(vec2d(1, 0),
                            subtract_vec2d(cursor_get_pos(), knob->center));
                        
                        if (knob->center.y > cursor_get_y())
                            cursor_angle = 2 * M_PI - cursor_angle;
                        
                        knob->angle_offset = cursor_angle - cur_angle;
                        knob->being_moved = true;
                    }
                }
                else {
                    knob->hovered = false;
                }
            }
        }
    }
}

void render_knob(Knob_t* knob) {
    if (knob->shown) {
        if (!knob->is_active) {
            // Draw default bar
            draw_sprite(knob->backdrop_sprite, &knob->backdrop_rect, COLOR_INACTIVE, SPRITE_NORMAL);
            // Draw handle
            draw_sprite(knob->knob_sprite, &knob->knob_rect, COLOR_INACTIVE, SPRITE_NORMAL);
        }

        else if (knob->hovered) {
            // Draw default bar
            draw_sprite(knob->backdrop_sprite, &knob->backdrop_rect, COLOR_WHEN_HOVERED, SPRITE_NORMAL);
            // Draw handle
            draw_sprite(knob->knob_sprite, &knob->knob_rect, COLOR_WHEN_HOVERED, SPRITE_NORMAL);
        }

        else {
            // Draw default bar
            draw_sprite(knob->backdrop_sprite, &knob->backdrop_rect, COLOR_NO_MULTIPLY, SPRITE_NORMAL);
            // Draw handle
            draw_sprite(knob->knob_sprite, &knob->knob_rect, COLOR_NO_MULTIPLY, SPRITE_NORMAL);
        }
    }
}

///@}

/** 
 * @brief Represents a single digit (0 to 9)
 */
typedef struct Number {
    Rect_t rect; 
    Sprite_t* sprite;
    uint8_t value;
} Number_t;

// All the file names (in the correct order)
static const char* number_file_names[10] = {
    "ui/0.bmp",
    "ui/1.bmp",
    "ui/2.bmp",
    "ui/3.bmp",
    "ui/4.bmp",
    "ui/5.bmp",
    "ui/6.bmp",
    "ui/7.bmp",
    "ui/8.bmp",
    "ui/9.bmp"
};

static Number_t* new_number(uint8_t value, Rect_t rect) {
    Number_t* number = (Number_t*) malloc(sizeof(Number_t));
    
    if (number == NULL) {
        printf("new_button: Failed to allocate memory for the number\n");
        return NULL;
    }
    
    number->rect = rect;
    number->value = max_uint8(0, value);
    
    number->sprite = new_sprite(0, 0, 1, number_file_names[number->value]);

    if (number->sprite == NULL) {
        printf("new_button: Failed to create the Number's Sprite\n");
        free(number);
        return NULL;
    }
    
    return number;
}

static void free_number(Number_t* number) {
    if (number == NULL) {
        printf("free_number: Cannot free a NULL pointer\n");
        return;
    }

    free_sprite(number->sprite);

    free(number);
}

static bool update_number(Number_t* number, uint8_t new_value) {

    if (number->value == new_value)
        return true;
    
    free_sprite(number->sprite);

    number->value = max_uint8(0, new_value);
    number->sprite = new_sprite(0, 0, 1, number_file_names[number->value]);
    if (number->sprite == NULL) {
        // Should we panic here?
        printf("update_number: Failed to create the Number's Sprite\n");
        return false;
    }

    return true;
}

static void render_number(Number_t* number, uint16_t color_to_multiply) {
    draw_sprite(number->sprite, &number->rect, color_to_multiply, SPRITE_NORMAL);
}

/** 
 * @brief Represents the score of the player in arcade mode, but can also be used to render a sequence of digits up to 65 535
 */
struct Score {
    Number_t** numbers;
    uint16_t value, max_score;
    uint8_t size;
    uint16_t x, y;
    uint16_t color;
};

Score_t* new_score(uint16_t x, uint16_t y, uint16_t value, uint8_t size, uint16_t color) {

    Score_t* score = (Score_t*) calloc(1, sizeof(Score_t*));
    
    if (score == NULL) {
        printf("new_score: Failed to allocate memory for the score\n");
        return NULL;
    }

    score->x = x;
    score->y = y;
    
    //  Auto adjusts size to the possible values
    score->size = clamp_uint8(size, 1, 5);
    
    switch (score->size) {
        case 1:
            score->max_score = 9; break;
        case 2:
            score->max_score = 99; break;
        case 3:
            score->max_score = 999; break;
        case 4:
            score->max_score = 9999; break;
        case 5:
            score->max_score = 0xFF; break;
    }
    value = min_uint16(value, score->max_score);
    score->value = value;

    score->numbers = (Number_t**) calloc(score->size, sizeof(Number_t*));

    if (score->numbers == NULL) {
        printf("new_score: Failed to allocate memory for the numbers\n");
        free(score);
        return NULL;   
    }

    score->color = color;

    uint16_t x_pos = x + 40 * score->size;
    uint16_t r;
    
    for (size_t i = 0; i < score->size; i++) {
        r = value % 10;
        value /= 10;
        score->numbers[i] = new_number(r, rect(x_pos - i * 40, y, 36, 48));
    }

    return score;
}

void free_score(Score_t* score) {
    if (score == NULL) {
        printf("free_score: Cannot free a NULL pointer\n");
        return;
    }

    for (uint8_t i = 0; i < score->size; ++i) {
        free_number(score->numbers[i]);
    }
    free(score->numbers);
    free(score);
}

inline uint16_t score_get_value(Score_t* score) {
    return score->value;
}

void set_score(Score_t* score, uint16_t value) {
    uint16_t r;
    value = min_uint16(value, score->max_score);
    score->value = value;

    for (size_t i = 0; i < score->size; i++) {
        r = value % 10;
        value /= 10;
        update_number(score->numbers[i], r);
    }
}

void update_score(Score_t* score) {
    set_score(score, score->value + 1);
}

void score_set_color(Score_t* score, uint16_t color) {
    score->color = color;
}

void render_score(Score_t* score) {
    for (size_t i = 0; i < score->size; i++)
        render_number(score->numbers[i], score->color);
}

void reset_score(Score_t* score) {
    score->value = 0;
    for (size_t i = 0; i < score->size; i++)
        update_number(score->numbers[i], 0);
}

