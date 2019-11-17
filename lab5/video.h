#pragma once

#include <lcom/lcf.h>
#include "video_macros.h"

typedef struct {
	uint16_t x_res, y_res;
	uint8_t bits_per_pixel, bytes_per_pixel, vg_mode;
	uint32_t red_mask, green_mask, blue_mask;
	uint8_t red_field_position, green_field_position, blue_field_position;
	uint8_t red_mask_size, green_mask_size, blue_mask_size;
} vbe_mode_info_summary_t;

extern vbe_mode_info_summary_t vg_info;

void privctl();

int vg_draw_xpm(xpm_map_t xpm, uint16_t x, uint16_t y);
