#pragma once

#include <lcom/lcf.h>
#include "video_macros.h"

/** @defgroup video_gr Graphics
 * @{
 */

typedef struct {
	uint16_t x_res; /** @brief Horizontal resolution in pixels/characters */
	uint16_t y_res; /** @brief Vertical resolution in pixels/characters */
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
	uint16_t total_memory; /** @brief number of 64kb memory blocks */

	uint16_t oem_software_rev;
	uint32_t vendor_name_ptr;
	uint32_t vendor_product_name_ptr;
	uint32_t vendor_product_rev_ptr;
	uint8_t reserved[222];
	uint8_t oem_data[256];
} vbe_controller_complete_t;
#pragma options align = reset

extern vbe_mode_info_summary_t vg_info;

void privctl();
void vg_fill_black();
void vg_fill_color_rgb_565(uint16_t color);

void switch_double_buffer();
void glitched_switch_double_buffer();

/** @} end of Graphics */
