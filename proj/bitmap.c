#include "bitmap.h"
#include "video.h"

#define BITMAP_ID 0x4D42

extern void* buffer_base;
extern vbe_mode_info_summary_t vg_info;

// TODO: Convert to our bit specifying data types
struct BitmapFileHeader {
    uint16_t type; // specifies the file type
	uint32_t size; // specifies the size in bytes of the bitmap file
	uint32_t reserved; // reserved; must be 0
	uint32_t offset; // specifies the offset in bytes from the bitmapfileheader to the bitmap bits
};

struct BitmapInfoHeader {
    uint32_t size; // specifies the number of bytes required by the struct
	uint32_t width; // specifies width in pixels
	uint32_t height; // specifies height in pixels
	uint16_t planes; // specifies the number of color planes, must be 1
	uint16_t bits; // specifies the number of bit per pixel
	uint32_t compression; // specifies the type of compression
	uint32_t imageSize; // size of image in bytes
	int32_t xResolution; // number of pixels per meter in x axis
	int32_t yResolution; // number of pixels per meter in y axis
	uint32_t nColors; // number of colors used by the bitmap
	uint32_t importantColors; // number of colors that are important
};

struct Bitmap {
    BitmapInfoHeader_t info;
    uint16_t* data;
};


Bitmap_t* new_bitmap(const char *filename) {
    // allocating necessary size
    Bitmap_t* bmp = malloc(sizeof(Bitmap_t));
    if (bmp == NULL) {
        printf("new_bmp: Failed to allocate memory for the Bitmap object\n");
        return NULL;
    }

    // open filename in read binary mode
    FILE* file = fopen(filename, "rb");

    if (file == NULL) {
        printf("new_bmp: Failed to open file in binary mode\n");
        return NULL;
    }

    // read the bitmap file header
    BitmapFileHeader_t fileHeader;

    // ptr, size, nelem, stream
    fread(&fileHeader, 2, 1, file);    

    // verify that this is a bmp file by check bitmap id
    if (fileHeader.type != BITMAP_ID) {
        fclose(file);
        printf("new_bitmap: This file was not a valid bitmap (.bmp) file\n");
        return NULL;
    }

    int32_t rd;

    do {
        if ((rd = fread(&fileHeader.size, 4, 1, file)) != 1)
            break;

        if ((rd = fread(&fileHeader.reserved, 4, 1, file)) != 1)
            break;

        if ((rd = fread(&fileHeader.offset, 4, 1, file)) != 1)
            break;    
    } while (0);

    // TODO: Do we really want to exit here?
    if (rd != 1)
        exit(-1);

    // read the bitmap info header
    BitmapInfoHeader_t infoHeader;

    fread(&infoHeader, sizeof(BitmapInfoHeader_t), 1, file);

    // move file pointer to the begining of bitmap data
    fseek(file, fileHeader.offset, SEEK_SET);

    // allocate enough memory for the bitmap image data
    uint16_t* bitmapImage = (uint16_t*) malloc(infoHeader.imageSize);
    if (bitmapImage == NULL) {
        fclose(file);
        printf("new_bitmap: Failed to allocate memory for the image's data\n");
        return NULL;
    }

    // verify memory allocation
    if (!bitmapImage) {
        free(bmp);
        free(bitmapImage);
        fclose(file);
        printf("new_bitmap: Second memory allocation verification\n");
        return NULL;
    }

    // read in the bitmap image data
    fread(bitmapImage, infoHeader.imageSize, 1, file);

    // make sure bitmap image data was read
    if (bitmapImage == NULL) {
        fclose(file);
        return NULL;
    }

    // close file and return bitmap image data
    fclose(file);

    bmp->data = bitmapImage;
    bmp->info = infoHeader;

    return bmp;
}

void free_bitmap(Bitmap_t *bmp) {
    free(bmp->data);
    free(bmp);
}


inline uint16_t bitmap_get_width(Bitmap_t* bmp) {
    return (uint16_t) bmp->info.width;
}

inline uint16_t bitmap_get_height(Bitmap_t* bmp) {
    return (uint16_t) bmp->info.height;
}


inline bool is_background(uint16_t color) {
    return (color == CHROMA_KEY_PINK_565);
}

inline bool is_not_background(uint16_t color) {
    return (color != CHROMA_KEY_PINK_565);
}

void draw_bitmap(Bitmap_t *bmp, int32_t x, int32_t y, Alignment alignment, uint16_t multiply) {

    if (bmp == NULL)
        return;

    int32_t width = bmp->info.width;
    int32_t drawWidth = width;
    int32_t height = bmp->info.height;

    if (alignment == ALIGN_CENTER)
        x -= width / 2;

    else if (alignment == ALIGN_RIGHT)
        x -= width;

    if (x + width < 0 || x > vg_info.x_res || y + height < 0 || y > vg_info.y_res)
        return;

    int xCorrection = 0;

    if (x < 0) {
        xCorrection = -x;
        drawWidth -= xCorrection;
        x = 0;

        if (drawWidth > vg_info.x_res )
            drawWidth = vg_info.x_res;
    }

    else if (x + drawWidth >= vg_info.x_res )
        drawWidth = vg_info.x_res - x;

    uint16_t* bufferStartPos;
    uint16_t* imgStartPos;

    for (int32_t i = 0; i < height; i++) {
        int pos = y + height - 1 - i;

        if (pos < 0 || pos >= vg_info.y_res)
            continue;

        bufferStartPos = (uint16_t*) buffer_base;
        bufferStartPos += x + pos * vg_info.x_res;

        imgStartPos = (uint16_t*) bmp->data + xCorrection + i * width;

        for (int j = 0; j < drawWidth; ++j)
            if (is_not_background(imgStartPos[j])) {
                bufferStartPos[j] = imgStartPos[j] & multiply;
            }
    }

}

void draw_bitmap_dynamic(Bitmap_t *bmp, uint16_t dynamic_slice_size, int32_t x, int32_t y, uint16_t width, uint16_t height, Alignment alignment, uint16_t multiply) {

    if (width < (dynamic_slice_size << 1) || height < (dynamic_slice_size << 1)) {
        printf("draw_bitmap_dynamic: Size is lesser than the minimum for this dynamic Bitmap (min is %d)\n", dynamic_slice_size << 1);
        return;
    }

    if (dynamic_slice_size == 0) {
        printf("draw_bitmap_dynamic: Bitmap is not dynamic, refusing to draw\n");
        return;
    }

    if (bmp == NULL) {
        printf("draw_bitmap_dynamic: Bitmap is invalid, refusing to draw\n");
        return;
    }

    uint16_t x_repetitions = width / dynamic_slice_size;
    uint16_t x_extra = width % dynamic_slice_size;

    if (alignment == ALIGN_CENTER)
        x -= width / 2;

    else if (alignment == ALIGN_RIGHT)
        x -= width;

    if (x + width < 0 || x > vg_info.x_res || y + height < 0 || y > vg_info.y_res)
        return;

    // int xCorrection = 0;

    // TODO: Nani dafuq que contas fazer para isto bater certo :(
    // if (x < 0) {
    //     xCorrection = -x;
    //     drawWidth -= xCorrection;
    //     x = 0;

    //     if (drawWidth > vg_info.x_res )
    //         drawWidth = vg_info.x_res;
    // }

    // else if (x + drawWidth >= vg_info.x_res )
    //     drawWidth = vg_info.x_res - x;

    // TODO: This currently has the naive approach, hoping that everything will be just fine

    uint16_t* buffer_cur_pos = (uint16_t*) buffer_base;
    uint16_t* img_cur_pos = (uint16_t*) bmp->data;
    uint16_t* img_rel_pos = (uint16_t*) bmp->data;
    
    /* TOP SECTION */

    // Top Left block

    for (uint16_t i = 0; i < dynamic_slice_size; ++i) {    

        int pos = y - 1 + i;
        if (pos < 0 || pos >= vg_info.y_res) {
            continue;
        }

        buffer_cur_pos = (uint16_t*) buffer_base + x + pos * vg_info.x_res;
        // dynamic_slice_size * 3 means an entire line
        img_cur_pos = (uint16_t*) bmp->data + i * dynamic_slice_size * 3;

        for (uint16_t k = 0; k < dynamic_slice_size; ++k) {
            if (is_not_background(*img_cur_pos)) {
                *buffer_cur_pos = *img_cur_pos;
            }
            ++buffer_cur_pos;
            ++img_cur_pos;
        }

        img_rel_pos = img_cur_pos;
        for (uint16_t j = 1; j < x_repetitions + x_extra - 1; ++j) {
            // Top Middle blocks
            img_cur_pos = img_rel_pos;
            for (uint16_t k = 0; k < dynamic_slice_size; ++k) {
                if (is_not_background(*img_cur_pos)) {
                    *buffer_cur_pos = *img_cur_pos;
                }
                ++buffer_cur_pos;
                ++img_cur_pos;
            }
        }

        // Top Right block
        img_cur_pos = (uint16_t*) bmp->data + i * dynamic_slice_size * 3
            + (dynamic_slice_size << 1);
        for (uint16_t k = 0; k < dynamic_slice_size; ++k) {
            if (is_not_background(*img_cur_pos)) {
                *buffer_cur_pos = *img_cur_pos;
            }
            ++buffer_cur_pos;
            ++img_cur_pos;
        }
    }

    /* MIDDLE SECTION */
    for (uint16_t i = dynamic_slice_size; i < height - dynamic_slice_size; ++i) {
        // Left Middle blocks
        int pos = y - 1 + i;
        if (pos < 0 || pos >= vg_info.y_res) {
            continue;
        }

        buffer_cur_pos = (uint16_t*) buffer_base + x + pos * vg_info.x_res;
        // dynamic_slice_size * 3 means an entire line
        img_cur_pos = (uint16_t*) bmp->data + (i % dynamic_slice_size + dynamic_slice_size) * dynamic_slice_size * 3;
        
        for (uint16_t k = 0; k < dynamic_slice_size; ++k) {
            if (is_not_background(*img_cur_pos)) {
                *buffer_cur_pos = *img_cur_pos;
            }
            ++buffer_cur_pos;
            ++img_cur_pos;
        }

        img_rel_pos = img_cur_pos;
        for (uint16_t j = 1; j < x_repetitions + x_extra - 1; ++j) {
            // Middle blocks
            img_cur_pos = img_rel_pos;
            for (uint16_t k = 0; k < dynamic_slice_size; ++k) {
                if (is_not_background(*img_cur_pos)) {
                    *buffer_cur_pos = *img_cur_pos;
                }
                ++buffer_cur_pos;
                ++img_cur_pos;
            }
        }
        // Right middle blocks
        img_cur_pos = (uint16_t*) bmp->data
            + (i % dynamic_slice_size + dynamic_slice_size) * dynamic_slice_size * 3
            + (dynamic_slice_size << 1);
        for (uint16_t k = 0; k < dynamic_slice_size; ++k) {
            if (is_not_background(*img_cur_pos)) {
                *buffer_cur_pos = *img_cur_pos;
            }
            ++buffer_cur_pos;
            ++img_cur_pos;
        }

    }

    /* BOTTOM SECTION */

    // Bottom Left block

    uint16_t line = 0; 
    for (uint16_t i = height - dynamic_slice_size; i < height; ++i) {

        int pos = y - 1 + i;
        if (pos < 0 || pos >= vg_info.y_res) {
            continue;
        }

        buffer_cur_pos = (uint16_t*) buffer_base + x + pos * vg_info.x_res;
        // dynamic_slice_size * 3 means an entire line
        img_cur_pos = (uint16_t*) bmp->data + (line + (dynamic_slice_size << 1)) * dynamic_slice_size * 3;

        for (uint16_t k = 0; k < dynamic_slice_size; ++k) {
            if (is_not_background(*img_cur_pos)) {
                *buffer_cur_pos = *img_cur_pos;
            }
            ++buffer_cur_pos;
            ++img_cur_pos;
        }

        img_rel_pos = img_cur_pos;
        for (uint16_t j = 1; j < x_repetitions + x_extra - 1; ++j) {
            // Bottom Middle blocks
            img_cur_pos = img_rel_pos;
            for (uint16_t k = 0; k < dynamic_slice_size; ++k) {
                if (is_not_background(*img_cur_pos)) {
                    *buffer_cur_pos = *img_cur_pos;
                }
                ++buffer_cur_pos;
                ++img_cur_pos;
            }
        }
        
        // Bottom right block
        img_cur_pos = (uint16_t*) bmp->data
            + (line + (dynamic_slice_size << 1)) * dynamic_slice_size * 3
            + (dynamic_slice_size << 1);
        for (uint16_t k = 0; k < dynamic_slice_size; ++k) {
            if (is_not_background(*img_cur_pos)) {
                *buffer_cur_pos = *img_cur_pos;
            }
            ++buffer_cur_pos;
            ++img_cur_pos;
        }

        ++line;
    }

}
