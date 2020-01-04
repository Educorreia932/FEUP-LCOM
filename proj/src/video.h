#pragma once

#include <lcom/lcf.h>
#include "video_macros.h"

/** @file video.h */

/** @defgroup video_gr Graphics
 * @brief Code used to set Minix in Graphic mode and render images
 * @{
 */

/** @brief A summary of the VBE info, containing only the strictly necessary */
typedef struct {
	/** @brief Horizontal resolution in pixels/characters */
	uint16_t x_res; 
	/** @brief Vertical resolution in pixels/characters */
	uint16_t y_res; 
	/** @brief Number of bits per pixel */
	uint8_t bits_per_pixel;
	/** @brief Number of bytes per pixel */
	uint8_t bytes_per_pixel;
	/** @brief Video mode to be initalized */
	uint8_t vg_mode;
	/** @brief Red color mask */
	uint32_t red_mask;
	/** @brief Green color mask */
	uint32_t green_mask;
	/** @brief Blue color mask */
	uint32_t blue_mask;
	/** @brief Red field position */
	uint8_t red_field_position;
	/** @brief Green field position */ 
	uint8_t green_field_position;
	/** @brief Blue field position */
	uint8_t blue_field_position;
	/** @brief Color masks size */
	uint8_t red_mask_size, green_mask_size, blue_mask_size;
} vbe_mode_info_summary_t;

extern vbe_mode_info_summary_t vg_info;

/** @brief Makes write to video memory possible */
void privctl();

/** @brief Fills the video buffer with the black color */
void vg_fill_black();

/**
 * @brief Fills the video buffer with a given color 
 * 
 * @param color Color to be set in rgb 565
 */
void vg_fill_color_rgb_565(uint16_t color);

/** 
 * @brief Copies the double buffer data to the vram
 */
void switch_double_buffer();

/**
 * @brief "Glitches" the screen, making it appear with a TV noise like appearance
 * @details Actually not a bug, but a feature (used for the switchboard)
 */
void glitched_switch_double_buffer();

/** @} end of Graphics */
