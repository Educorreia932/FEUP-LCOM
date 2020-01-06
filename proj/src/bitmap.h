#pragma once

#include <lcom/lcf.h>

/** @file */

/** @addtogroup sprite */

#define CHROMA_KEY_PINK_565 0xF81F /** <@brief The color to be considered as 'invisible' to the renderer */

/**
 * @brief Enum detailing the alignment of a bitmap upon rendering (relative to the given top leftmost point)
 * 
 */
typedef enum {
    ALIGN_LEFT, /** <@brief Default alignment */
    ALIGN_CENTER, /** <@brief Horizontally centers the bitmap in relation to the point */
    ALIGN_RIGHT /** <@brief The given point becomes the top leftmost point */
} Alignment;

/**
 * @brief The object representing a bitmap file (.bmp)
 * @details The basis for the more advanced Sprite datatype
 */
typedef struct Bitmap Bitmap_t;

/**
 * @brief Creates a new Bitmap object from a given filename
 * @warning The file's path is relative to the assets folder specified in the Game Manager (or defined as a command line argument)
 * 
 * @param filename The name of the Bitmap file (.bmp) to load
 * @return Bitmap_t* Returns a pointer to the fully built Bitmap object on success\n
 * NULL otherwise 
 */
Bitmap_t* new_bitmap(const char *filename);

/**
 * @brief Frees a given Bitmap from memory
 * 
 * @param bmp Pointer to the Bitmap to be freed
 */
void free_bitmap(Bitmap_t *bmp);

/**
 * @brief Returns the width (in pixels) of a given Bitmap
 * 
 * @param bmp Pointer to the Bitmap to operate over
 * @return uint16_t Width of the given Bitmap
 */
uint16_t bitmap_get_width(Bitmap_t* bmp);
/**
 * @brief Returns the height (in pixels) of a given Bitmap
 * 
 * @param bmp Pointer to the Bitmap to operate over
 * @return uint16_t Height of the given Bitmap
 */
uint16_t bitmap_get_height(Bitmap_t* bmp);

/**
 * @brief Draws a given Bitmap onto the screen
 * 
 * @param bmp Pointer to the Bitmap to be rendered
 * @param x The x coordinate to draw on
 * @param y The y coordinate to draw on
 * @param alignment The Bitmap's relative Alignment
 * @param multiply The RGB 565 color to have a bitwise & operation with each pixel\n
 * Use 0xFF (or the macro defined in sprite.h) to draw a perfect copy of the Bitmap
 */
void draw_bitmap(Bitmap_t *bmp, int32_t x, int32_t y, Alignment alignment, uint16_t multiply);
/**
 * @brief Draws a given Bitmap onto the screen
 * @details Draws the Bitmap according to a reflection over its y axis (-> becomes <-)
 * 
 * @param bmp Pointer to the Bitmap to be rendered
 * @param x The x coordinate to draw on
 * @param y The y coordinate to draw on
 * @param alignment The Bitmap's relative Alignment
 * @param multiply The RGB 565 color to have a bitwise & operation with each pixel\n
 * Use 0xFF (or the macro defined in sprite.h) to draw a perfect copy of the Bitmap
 */
void draw_bitmap_reversed_y_axis(Bitmap_t *bmp, int32_t x, int32_t y, Alignment alignment, uint16_t multiply);
/**
 * @brief Draws a given Bitmap onto the screen
 * @details Draws the Bitmap according to a reflection over its x axis (^ becomes v)
 * @param bmp Pointer to the Bitmap to be rendered
 * @param x The x coordinate to draw on
 * @param y The y coordinate to draw on
 * @param alignment The Bitmap's relative Alignment
 * @param multiply The RGB 565 color to have a bitwise & operation with each pixel\n
 * Use 0xFF (or the macro defined in sprite.h) to draw a perfect copy of the Bitmap
 */
void draw_bitmap_reversed_x_axis(Bitmap_t *bmp, int32_t x, int32_t y, Alignment alignment, uint16_t multiply);
/**
 * @brief Draws a given Bitmap onto the screen
 * @details Draws the Bitmap according to a reflection over both its axis (-> becomes <- and ^ becomes v)
 * @param bmp Pointer to the Bitmap to be rendered
 * @param x The x coordinate to draw on
 * @param y The y coordinate to draw on
 * @param alignment The Bitmap's relative Alignment
 * @param multiply The RGB 565 color to have a bitwise & operation with each pixel\n
 * Use 0xFF (or the macro defined in sprite.h) to draw a perfect copy of the Bitmap
 */
void draw_bitmap_reversed_both_axis(Bitmap_t *bmp, int32_t x, int32_t y, Alignment alignment, uint16_t multiply);

/**
 * @brief Draws a Bitmap on the screen
 * @details Upon rendering, the image will be 'stretched' (reproduced) to fulfill the indicated size\n
 * @warning For optimal results, the bitmap should be square and have a lenghth that is a multiple of 3, the length / 3 will give the ideal dynamic_slice_size
 * 
 * @param bmp Pointer to the Bitmap to be rendered
 * @param dynamic_slice_size The slice size
 * @param x The x coordinate to draw on
 * @param y The y coordinate to draw on
 * @param width The width to draw\n
 * Calls to render a width lesser than twice the object's dynamic_slice_size will be ignored
 * @param height The height to draw\n
 * Calls to render a height lesser than twice the object's dynamic_slice_size will be ignored
 * @param alignment The Bitmap's relative Alignment
 * @param multiply The RGB 565 color to have a bitwise & operation with each pixel\n
 * Use 0xFF (or the macro defined in sprite.h) to draw a perfect copy of the Bitmap
 */
void draw_bitmap_dynamic(Bitmap_t *bmp, uint16_t dynamic_slice_size, int32_t x, int32_t y, uint16_t width, uint16_t height, Alignment alignment, uint16_t multiply);

/** @} */
