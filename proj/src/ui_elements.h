#pragma once

#include <lcom/lcf.h>
#include "geometry.h"
#include "sprite.h"


/* BUTTONS */

typedef struct Button Button_t;

/**
 * @param sprite_file_name Empty string to not have (and draw) a sprite
 */
Button_t* new_button(const char* sprite_file_name, void (*func)(), Rect_t rect);

/**
 * @brief Auto sizes the button, must have a Sprite
 */
Button_t* new_button_auto_size(const char* sprite_file_name, void (*func)(), Vec2d_t pos);

/** 
 * @brief Reuses a Sprite_t from somewhere else
 */
Button_t* new_button_existing_sprite(Sprite_t *sprite, void (*func)(), Rect_t rect);

void free_button(Button_t* button);
void free_button_without_sprite(Button_t* button);

// Button Methods

void button_hide(Button_t* button);
void button_show(Button_t* button);
void button_activate(Button_t* button);
void button_deactivate (Button_t* button);
void button_set_activation(Button_t* button, bool is_active);

void update_button(Button_t* button);
void render_button(Button_t* button);


/*----------------------------*/
/*------    SLIDERS   --------*/
/*----------------------------*/

typedef struct Slider Slider_t;

Slider_t* new_slider(const char* bar_sprite_file_name, const char* handle_sprite_file_name, void (*func)(uint8_t), Vec2d_t bar_pos, uint8_t max_state, Vec2d_t start_pos, Vec2d_t end_pos);

void free_slider(Slider_t* slider);

void slider_hide(Slider_t* slider);
void slider_show(Slider_t* slider);
void slider_activate(Slider_t* slider);
void slider_deactivate (Slider_t* slider);
void slider_set_activation(Slider_t* slider, bool is_active);

void update_slider(Slider_t* slider);

void render_slider(Slider_t* slider);


/*----------------------------*/
/*--------    KNOB    --------*/
/*----------------------------*/

typedef struct Knob Knob_t;

Knob_t* new_knob(const char* backdrop_sprite_file_name, const char* knob_sprite_file_name, void (*on_drop)(), void (*on_reset)(), float angle_setback, Vec2d_t pos, float start_angle, float end_angle, float radius);

void free_knob(Knob_t* knob);

void knob_hide(Knob_t* knob);
void knob_show(Knob_t* knob);
void knob_activate(Knob_t* knob);
void knob_deactivate (Knob_t* knob);
void knob_set_activation(Knob_t* knob, bool is_active);

void update_knob(Knob_t* knob);

void render_knob(Knob_t* knob);

/*----------------------------*/
/*------    NUMBERS    -------*/
/*----------------------------*/

typedef struct Number Number_t;

typedef struct Score Score_t;

Score_t* new_score(uint16_t x, uint16_t  y, uint8_t value);
void free_score(Score_t* score);

void update_score(Score_t* score);

void render_score(Score_t* score);

void reset_score(Score_t* score);
