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

#pragma pack(1)
// All pointers are far pointers :(
typedef struct {
	uint8_t vbe_signature[4];
	uint16_t vbe_version;
	uint32_t oem_string_ptr;
	uint8_t vbe_capabilities[4];
	uint32_t video_mode_list_ptr;
	uint16_t total_memory; //number of 64kb memory blocks

	uint16_t oem_software_rev;
	uint32_t vendor_name_ptr;
	uint32_t vendor_product_name_ptr;
	uint32_t vendor_product_rev_ptr;
	uint8_t reserved[222];
	uint8_t oem_data[256];
} vbe_controller_straight_from_hell_t;
#pragma options align = reset

extern vbe_mode_info_summary_t vg_info;

void privctl();

uint8_t vg_draw_xpm(xpm_map_t xpm, uint16_t x, uint16_t y);
uint8_t vg_update_xpm(xpm_map_t xpm, uint16_t old_x, uint16_t old_y, uint16_t new_x, uint16_t new_y);
