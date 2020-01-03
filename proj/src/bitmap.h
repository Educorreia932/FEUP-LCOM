#pragma once

#include <lcom/lcf.h>

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

typedef struct BitmapFileHeader BitmapFileHeader_t;

typedef struct BitmapInfoHeader BitmapInfoHeader_t;

typedef struct Bitmap Bitmap_t;

Bitmap_t *new_bitmap(const char *filename);

void free_bitmap(Bitmap_t *bmp);

uint16_t bitmap_get_width(Bitmap_t* bmp);
uint16_t bitmap_get_height(Bitmap_t* bmp);

void draw_bitmap(Bitmap_t *bmp, int32_t x, int32_t y, Alignment alignment, uint16_t multiply);
void draw_bitmap_reversed_y_axis(Bitmap_t *bmp, int32_t x, int32_t y, Alignment alignment, uint16_t multiply);
void draw_bitmap_reversed_x_axis(Bitmap_t *bmp, int32_t x, int32_t y, Alignment alignment, uint16_t multiply);
void draw_bitmap_reversed_both_axis(Bitmap_t *bmp, int32_t x, int32_t y, Alignment alignment, uint16_t multiply);

void draw_bitmap_dynamic(Bitmap_t *bmp, uint16_t dynamic_slice_size, int32_t x, int32_t y, uint16_t width, uint16_t height, Alignment alignment, uint16_t multiply);

