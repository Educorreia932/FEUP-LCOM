#pragma once

#include <lcom/lcf.h>
#include "geometry.h"

/** @file mouse_cursor.h */

/** @addtogroup inputs
  *
  * @brief	The interfaces to receive all information related to direct user input
  *
  * @{
  */

/* CONSTRUCTORS */

/**
 * @brief The cursor must be initialized in order for all the functions of this file to work
 * @details Initializes Mouse Input Events if they weren't already
 * 
 */
void initialize_cursor();
/**
 * @brief Frees the occupied data when the cursor is no longer needed
 * 
 */
void free_cursor();

/* Activate and deactivate Cursor rendering */

/**
 * @brief Prevents the cursor from being rendered onto the screen
 * @note Cursor is rendered by default
 * 
 */
void cursor_hide();
/**
 * @brief Makes the cursor be rendered onto the screen
 * @note Cursor is renderd by default
 * 
 */
void cursor_show();
/**
 * @brief Returns wether the cursor is currently being rendered or not
 * 
 * @return True when the cursor is being show\n
 * False otherwise
 */
bool cursor_is_shown();


/* CURSOR METHODS */

/**
 * @brief Updates the needed data for the cursor to be rendered
 * @details Must be called every frame
 * 
 */
void update_cursor();
/**
 * @brief Renders the cursor onto the screen
 * @details Only renders if the cursor is currenly set to show (on by default)\n
 * Must be called every frame
 * 
 */
void render_cursor();

/**
 * @brief Checks if the cursor is hovering inside the area defined by the rect
 * 
 * @param rect A Rect_t indicating the area's top leftmos point, its width and height
 * @return True when the cursor is hovering over the specified area\n
 * False otherwise
 */
bool is_cursor_inside_rect(Rect_t* rect);
/**
 * @brief Gets the current x coordinate of the cursor inside the limits of the screen
 * 
 * @return float The cursor's x coordinate on the screen
 */
float cursor_get_x();
/**
 * @brief Gets the current y coordinate of the cursor inside the limits of the screen
 * 
 * @return float The cursor's y coordinate on the screen
 */
float cursor_get_y();
/**
 * @brief Gets the current coordinates of the cursor inside the limits of the screen
 * 
 * @return float The cursor's coordinates on the screen
 */
Vec2d_t cursor_get_pos();

/** @} */
