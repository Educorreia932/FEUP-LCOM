#pragma once

#include <lcom/lcf.h>
#include "geometry.h"
#include "sprite.h"

/** @file ui_elements.h */

/** @addtogroup ui 
 * @{
 */

/** @name Button
 * @{
*/

/** @brief Representation that can be clicked to issue a command */
typedef struct Button Button_t;

/**
 * @brief Creates a new Button object
 * 
 * @param sprite_file_name Name of the file containing the sprite of the button. Empty string to not have (and draw) a sprite
 * @param func Function to be executed when the user clicks the button
 * @param rect Button "clickable" area
 */
Button_t* new_button(const char* sprite_file_name, void (*func)(), Rect_t rect);

/**
 * @brief Creates a new Button object and auto sizes the button
 * @note Must have a Sprite
 * 
 * @param sprite_file_name Name of the file containing the sprite of the button
 * @param func Function to be executed when the user clicks the button
 * @param rect Button position in screen and "clickable" area
 */
Button_t* new_button_auto_size(const char* sprite_file_name, void (*func)(), Vec2d_t pos);

/** 
 * @brief Creates a new Button object and auto sizes the button, reusing a Sprite_t from somewhere else
 * @param sprite_file_name Name of the file containing the sprite of the button
 * @param func Function to be executed when the user clicks the button
 * @param rect Button "clickable" area
 */
Button_t* new_button_existing_sprite(Sprite_t *sprite, void (*func)(), Rect_t rect);

/**
 * @brief Frees a given Button object from memory
 * 
 * @param button Pointer to the Button to be freed
 */
void free_button(Button_t* button);

/**
 * @brief Frees a given Button object from memory that doesn't have a Sprite
 * 
 * @param button Pointer to the Button to be freed
 */
void free_button_without_sprite(Button_t* button);

// Button Methods

/** @brief Hides a Button, essentialy by not rendering it 
 * 
 * @param button Pointer to the Button object to be hidden 
*/
void button_hide(Button_t* button);

/** @brief Does the opposite of button_hide()
 * 
 * @param button Pointer to the Button object to be shown
 */
void button_show(Button_t* button);

/** @brief Makes a Button "clickable"
 * @note The button may appear, but it only executes its function if it's activated
 * 
 * @param button Pointer to the Button object to be activated
 */
void button_activate(Button_t* button);

/** @brief Makes a Button "unclickable"
 * @details The opposite of button_activate()
 * 
 * @param button Pointer to the Button object to be deactivated
 */
void button_deactivate (Button_t* button);

/** 
 * @brief Activates or deactivates a Button
 * 
 * @param button Pointer to the Button object to be (de)activated
 * @param is_active Specifies whether the button is to be set with active or not
 */
void button_set_activation(Button_t* button, bool is_active);

/**
 * @brief Gets the Rect object of a Button
 * @note Only exist for use by the minigames
 * 
 * @param button Pointer to the Button object in cause
 * @return Pointer to the Rect object of the button
 */
Rect_t* button_get_rect(Button_t* button);

/**
 * @brief Moves a Button object by a certain displacement
 * 
 * @param button Pointer to the Button object that's being moved
 * @param delta Horizontal and Vertical displacement
 */
void button_move(Button_t* button, Vec2d_t delta);

void update_button(Button_t* button);
void render_button(Button_t* button);

/** @} end of Button */

/** @name Slider 
 * @{
*/

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

/** @} end of Slider */

/** @name Knob
 * @{
 */

typedef struct Knob Knob_t;

Knob_t* new_knob(const char* backdrop_sprite_file_name, const char* knob_sprite_file_name, void (*on_drop)(), void (*on_reset)(), float angle_setback, Vec2d_t pos, float start_angle, float end_angle, float radius);

void free_knob(Knob_t* knob);

void knob_hide(Knob_t* knob);
void knob_show(Knob_t* knob);
void knob_activate(Knob_t* knob);
void knob_deactivate (Knob_t* knob);
void knob_set_activation(Knob_t* knob, bool is_active);

void knob_reset_state(Knob_t *knob);

void update_knob(Knob_t* knob);

void render_knob(Knob_t* knob);

/** @name Score
 * @{
*/

/** @brief Used to render a sequence of digits
 * @details Its main purpose its to render the score of a player in Arcade but it's also used to display time passed
 */
typedef struct Score Score_t;

/** 
 * @brief Creates a new score object
 * @details Auto adjust both the size and the value to fit the Score accordingly
 * 
 * @param x Horizontal position
 * @param y Vertical positions
 * @param value Value to be displayed
 * @param size Number of digits to be displayed
 * @param color Color of the rendered Score
 * 
 * @return Score_t* Pointer to the fully built Score object on success\n
 * NULL otherwise
 */
Score_t* new_score(uint16_t x, uint16_t y, uint16_t value, uint8_t size, uint16_t color);
/**
 * @brief Frees a Score object from memory
 * 
 * @param score Pointer to the Score object to be freed
 */
void free_score(Score_t* score);

/** 
 * @brief Gets the stored value of a Score object 
 * 
 * @param score Pointer to the Score object of which we're getting the value
 * @returns The numeric value of the Score object
 */ 
uint16_t get_score(Score_t* score);

/**
 * @brief Sets the Score object value to a given one
 * 
 * @param score Pointer to the Score Object that's being set
 * @param value New value of the Score
 */
void set_score(Score_t* score, uint16_t value);

/**
 * @brief Increments the numeric value of a Score object by one
 * 
 * @param score Pointer to the Score that is being updated
 */
void update_score(Score_t* score);

void score_set_color(Score_t* score, uint16_t color);
/**
 * @brief Renders a Score object
 * 
 * @param score Pointer to the Score to be reset
 */
void render_score(Score_t* score);
/**
 * @brief Resets the score to 0 whenever the player loses
 * 
 * @param score Pointer to the Score to be reset
 */
void reset_score(Score_t* score);

/** @} end of Score */

/** @} end of UI Elements */
