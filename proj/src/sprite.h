#pragma once

#include <lcom/lcf.h>
#include "geometry.h"

/** @file sprite.h */

/** @defgroup sprite Sprite */
/** @addtogroup sprite
  *
  * @brief	The high level interface to create and render images onto the screen
  *
  * @{
  */

#define COLOR_NO_MULTIPLY 0xFFFF /** <@brief This value always preserves the original Sprite */
#define COLOR_RED 0xF800 /** <@brief A red color filter */
#define COLOR_BLUE 0x001F /** <@brief A blue color filter */
#define COLOR_PINK 0xFB39 /** <@brief A pink color filter */

/**
 * @brief Renders a bitmap to the screen, while allowing it to be animated
 * 
 * @details This is done by allowing up to 255 bitmaps to be loaded at once, and having a mutable state that decides which of these bitmaps to render. Clever use of this state alonside a state machine allows you to create custom animated Sprites.\n
 * All calls to the renderer allow you to reverse the image along any of its two axis
 * 
 */
typedef struct Sprite Sprite_t;

/**
 * @brief Represents all the possible symmetries for the call to the renderer
 * 
 */
typedef enum SpriteReverse {
    SPRITE_NORMAL = 0, /** <@brief No alteration to the Sprite */
    SPRITE_Y_AXIS = 1, /** <@brief Symmetric along its vertical axis */
    SPRITE_X_AXIS = 2, /** <@brief Symmetric along its horizontal axis */
    SPRITE_BOTH_AXIS = 3 /** <@brief Symmetric along both axis */
} SpriteReverse;

/* SPRITE */

/**
 * @brief Creates a new Sprite object
 * 
 * @param x_offset Offset along the x axis for all calls to the renderer
 * @param y_offset Offset along the y axis for all calls to the renderer
 * @param num The number of bitmaps the Sprite will have
 * @param ... All the 'num' filenames (relative path to the assets folder)
 * @return Sprite_t* Ptr to the Sprite object on success\n
 * NULL otherwise
 */
Sprite_t* new_sprite(float x_offset, float y_offset, int num, ...);
/**
 * @brief Frees the Sprite passed on the pointer
 * @details If Sprite is NULL a waning message will be output
 * @param s The Sprite to freed
 */
void free_sprite(Sprite_t *s);

/**
 * @brief Draws a Sprite on the screen (check the documentation of draw_sprite_floats for more details)
 * 
 * @param s The Sprite to draw
 * @param r The Rect (only the x and y components are used) of where to draw
 * @param color_to_multiply Multiply all bits by a given color
 * @param reversed Should the Sprite be reversed along any axis
 */
void draw_sprite(Sprite_t *s, Rect_t *r, uint32_t color_to_multiply, SpriteReverse reversed);
/**
 * @brief Draws a Sprite on the screen (check the documentation of draw_sprite_floats for more details)
 * 
 * @param s The Sprite to draw
 * @param v The coordinats of the point to draw at
 * @param color_to_multiply Multiply all bits by a given color
 * @param reversed Should the Sprite be reversed along any axis
 */
void draw_sprite_vec2d(Sprite_t *s, Vec2d_t v, uint32_t color_to_multiply, SpriteReverse reversed);
/**
 * @brief Draws a Sprite on the screen (check the documentation of draw_sprite_floats for more details)
 * @warning The color_to_multiply uses a bitwise AND between the colors
 * @param s The Sprite to draw
 * @param x The x coordinate to draw at
 * @param y The y coordinate to draw at
 * @param color_to_multiply Multiply all bits by a given color
 * @param reversed Should the Sprite be reversed along any axis
 */
void draw_sprite_floats(Sprite_t *s, float x, float y, uint32_t color_to_multiply, SpriteReverse reversed);

/**
 * @brief Returns the Sprite's width
 * @details Only takes into consideration its first Bitmap
 * @param s The Sprite to get the width of
 * @return uint16_t The width of the Sprite
 */
uint16_t sprite_get_width(Sprite_t *s);
/**
 * @brief Returns the Sprite's height
 * @details Only takes into consideration its first Bitmap
 * @param s The Sprite to get the height of
 * @return uint16_t The height of the Sprite
 */
uint16_t sprite_get_height(Sprite_t *s);
/**
 * @brief Returns a Vec2d with the Sprite's size
 * @details Only takes into consideration its first Bitmap
 * @param s The Sprite to get the size of
 * @return Vec2d_t Vec2d with (width, height)
 */
Vec2d_t sprite_get_size(Sprite_t *s);

/**
 * @brief Set the animation state
 * 
 * @param s The Sprite to set the state
 * @param state The state to set the Sprite to
 */
void set_animation_state(Sprite_t *s, uint8_t state);
/**
 * @brief Get the animation state
 * 
 * @param s The Sprite to get the state
 * @return uint8_t The current state of the Sprite
 */
uint8_t get_animation_state(const Sprite_t *s);

/* SPRITE DYNAMIC */

/**
 * @brief Renders a bitmap onto the screen, multiplying parts of the bitmap to fulfill the required size
 * @warning For optimal results, the bitmap should be square and have a lenghth that is a multiple of 3, the length / 3 will give the ideal dynamic_slice_size
 * 
 */
typedef struct SpriteDynamic SpriteDynamic_t;

/**
 * @brief Creates a new SpriteDynamic object
 * @details For this to work as expected, the sprite 
 * 
 * @param file_name The path (relative to the assets folder) to the bitmap (preferebly, a square one)
 * @param dynamic_slice_size The size of the image slice (should be one third of the bitmap's size)
 * @param x_offset The x offset to be used when drawing this Sprite 
 * @param y_offset The y offset to be used when drawing this Sprite
 * @return SpriteDynamic_t* Returns a pointer to the full SpriteDynamic object when successful\n
 * NULL otherwise
 */
SpriteDynamic_t* new_sprite_dynamic(const char* file_name, uint16_t dynamic_slice_size, uint16_t x_offset, uint16_t y_offset);
/**
 * @brief Frees a SpriteDynamic
 * 
 * @param sd Pointer to the SpriteDynamic to be freed
 */
void free_sprite_dynamic(SpriteDynamic_t* sd);

/**
 * @brief Draws a SpriteDynamic on the screen
 * @details Upon rendering the image will be 'stretched' (reproduced) to fulfill the indicated size
 * @warning The color_to_multiply uses a bitwise AND between the colors
 * @param sd The SpriteDynamic to draw
 * @param r Pointer to a rect with the point for the Sprite to be drawn at and it's size (both width and height). Calls to render a size lesser than twice the object's dynamic_slice_size will be ignored
 * @param color_to_multiply Multiply all bits by a given color
 */
void draw_sprite_dynamic(SpriteDynamic_t *sd, Rect_t *r, uint32_t color_to_multiply);

/** @} */
