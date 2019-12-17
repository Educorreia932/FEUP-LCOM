#include "ui_elements.h"
#include "input_events.h"
#include "mouse_cursor.h"
#include "math_utils.h"

/* COLOR CHANGES IN THE UI */
#define COLOR_WHEN_HOVERED 0xef5d
#define COLOR_INACTIVE 0x9492

#define KNOB_ANGLE_PRECISION 0.1f
#define KNOB_ANGLE_SETBACK 0.01f
#define KNOB_ANGLE_RETAKE_CONTROL_PRECISION 0.25f

/** @name Button */
///@{

struct Button {
    Rect_t rect;
    Sprite_t* sprite;
    bool shown, is_active, hovered;
    void (*func)();
};

/**
 * @param sprite_file_name Empty string to not draw a sprite
 */
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

// Empty string to not draw a sprite
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
            draw_sprite(button->sprite, &button->rect, COLOR_INACTIVE, false);
        else if (button->hovered)
            draw_sprite(button->sprite, &button->rect, COLOR_WHEN_HOVERED, false);
        else
            draw_sprite(button->sprite, &button->rect, COLOR_NO_MULTIPLY, false);
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
            draw_sprite_floats(slider->bar_sprite, slider->bar_pos.x, slider->bar_pos.y, COLOR_INACTIVE, false);
            // Draw handle
            draw_sprite(slider->handle_sprite, &slider->handle_rect, COLOR_INACTIVE, false);
        }
        else if (slider->hovered) {
            // Draw default bar
            draw_sprite_floats(slider->bar_sprite, slider->bar_pos.x, slider->bar_pos.y, COLOR_NO_MULTIPLY, false);
            // Draw handle
            draw_sprite(slider->handle_sprite, &slider->handle_rect, COLOR_WHEN_HOVERED, false);
        }
        else {
            // Draw default bar
            draw_sprite_floats(slider->bar_sprite, slider->bar_pos.x, slider->bar_pos.y, COLOR_NO_MULTIPLY, false);
            // Draw handle
            draw_sprite(slider->handle_sprite, &slider->handle_rect, COLOR_NO_MULTIPLY, false);
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
    enum KnobAngleLock angle_lock;
    void (*func)(float);
};


Knob_t* new_knob(const char* backdrop_sprite_file_name, const char* knob_sprite_file_name, void (*func)(float), Vec2d_t pos, float start_angle, float end_angle, float radius) {    
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
    if (func == NULL) {
        printf("new_knob: Function pointer invalid\n");
        free_sprite(knob->backdrop_sprite);
        free_sprite(knob->knob_sprite);
        free(knob);
        return NULL;
    }
    knob->func = func;
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

float knob_get_cursor_angle(Knob_t *knob) {
    float angle = angle_vec2d(vec2d(1, 0), subtract_vec2d(                        cursor_get_pos(), knob->center));
    
    angle += knob->angle_offset;

    if (knob->center.y > cursor_get_y() + (knob->radius * sinf(knob->angle_offset)))
        angle = 2 * M_PI - angle;
    
    return angle;
}

float knob_get_cur_angle(Knob_t *knob) {

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

void knob_update_pos(Knob_t *knob, float angle) {

    Vec2d_t pos = circumference_vec2d(knob->center, knob->radius, 
                fclampf(angle, knob->start_angle, knob->end_angle));

    pos = subtract_vec2d(pos, vec2d(sprite_get_width(knob->knob_sprite) / 2.0f, sprite_get_height(knob->knob_sprite) / 2.0f));

    knob->knob_rect.x = pos.x;
    knob->knob_rect.y = pos.y;

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
                knob->func(fclampf(angle, knob->start_angle, knob->end_angle));
            }
        }
        else {

            float cur_angle = knob_get_cur_angle(knob);
            if (fabs(cur_angle) > fabs(knob->start_angle) + KNOB_ANGLE_PRECISION) {
                // If we want to reset dynamically, change this place here
                cur_angle -= KNOB_ANGLE_SETBACK;
                knob_update_pos(knob, fmaxf(cur_angle, knob->start_angle));
            }
            else if (is_cursor_inside_rect(&knob->knob_rect)) {

                // Mouse is hovering on the knob
                knob->hovered = true;

                if (mouse_get_lb_down()) {

                    // cur_angle was calculated just before
                    
                    float cursor_angle = angle_vec2d(vec2d(1, 0),
                        subtract_vec2d(cursor_get_pos(),                        knob->center));
                    
                    if (knob->center.y > cursor_get_y())
                        cursor_angle = 2 * M_PI - cursor_angle;
                    
                    knob->angle_offset = cursor_angle - cur_angle;
                    knob->being_moved = true;
                }
            }
        }
    }
}

void render_knob(Knob_t* knob) {
    if (knob->shown) {
        if (!knob->is_active) {
            // Draw default bar
            draw_sprite(knob->backdrop_sprite, &knob->backdrop_rect, COLOR_INACTIVE, false);
            // Draw handle
            draw_sprite(knob->knob_sprite, &knob->knob_rect, COLOR_INACTIVE, false);
        }

        else if (knob->hovered) {
            // Draw default bar
            draw_sprite(knob->backdrop_sprite, &knob->backdrop_rect, COLOR_WHEN_HOVERED, false);
            // Draw handle
            draw_sprite(knob->knob_sprite, &knob->knob_rect, COLOR_WHEN_HOVERED, false);
        }

        else {
            // Draw default bar
            draw_sprite(knob->backdrop_sprite, &knob->backdrop_rect, COLOR_NO_MULTIPLY, false);
            // Draw handle
            draw_sprite(knob->knob_sprite, &knob->knob_rect, COLOR_NO_MULTIPLY, false);
        }
    }
}

///@}
