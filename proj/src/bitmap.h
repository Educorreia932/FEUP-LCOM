#pragma once

#include <lcom/lcf.h>

#define CHROMA_KEY_PINK_565 0xF81F

// The number of pixels of the "dynamic" subdivision
#define BMP_DYNAMIC_SCALE 5

typedef enum {
    ALIGN_LEFT, ALIGN_CENTER, ALIGN_RIGHT
} Alignment;

typedef struct BitmapFileHeader BitmapFileHeader_t;

typedef struct BitmapInfoHeader BitmapInfoHeader_t;

typedef struct Bitmap Bitmap_t;

Bitmap_t *new_bitmap(const char *filename);

void free_bitmap(Bitmap_t *bmp);

uint16_t bitmap_get_width(Bitmap_t* bmp);
uint16_t bitmap_get_height(Bitmap_t* bmp);

void draw_bitmap(Bitmap_t *bmp, int32_t x, int32_t y, Alignment alignment, uint16_t multiply);

void draw_bitmap_reversed(Bitmap_t *bmp, int32_t x, int32_t y, Alignment alignment, uint16_t multiply);

void draw_bitmap_dynamic(Bitmap_t *bmp, uint16_t dynamic_slice_size, int32_t x, int32_t y, uint16_t width, uint16_t height, Alignment alignment, uint16_t multiply);

