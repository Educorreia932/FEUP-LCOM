// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>

#include <lcom/lab5.h>

// Any header files included below this line should have been created by you

#include "video.h"
#include "keyboard.h"

int main(int argc, char *argv[]) {
	// sets the language of LCF messages (can be either EN-US or PT-PT)
	lcf_set_language("EN-US");

	// enables to log function invocations that are being "wrapped" by LCF
	// [comment this out if you don't want/need it]
	lcf_trace_calls("/home/lcom/labs/lab5/trace.txt");

	// enables to save the output of printf function calls on a file
	// [comment this out if you don't want/need it]
	lcf_log_output("/home/lcom/labs/lab5/output.txt");

  	// handles control over to LCF
  	// [LCF handles command line arguments and invokes the right function]
  	if (lcf_start(argc, argv))
		return 1;

	// LCF clean up tasks
	// [must be the last statement before return]
	lcf_cleanup();

	return 0;
}

int(video_test_init)(uint16_t mode, uint8_t delay) {
	if (vg_init(mode) == NULL) {
		vg_exit();
		return 1;
	}

	sleep(delay);

	vg_exit();

	return 0;
}

int kbd_esc_loop() {
	// Keyboard
 	uint8_t bit_no = KBD_IRQ;
        
	// Only avoids making this operation on every notification
	int kbd_bit_mask = BIT(bit_no);
    
	if (kbd_subscribe_int(&bit_no)) {
		vg_exit();
    return 1;
	}
    
	int r, ipc_status;
  	message msg;

	// Interrupt loop
	while (scancode != ESC_BREAKCODE) { 
			if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
					printf("driver_receive failed with: %d", r);
					continue;
			}

			if (is_ipc_notify(ipc_status)) {
					switch (_ENDPOINT_P(msg.m_source)) {
							case HARDWARE: /* hardware interrupt notification */
									if (msg.m_notify.interrupts & kbd_bit_mask) { /* subscribed interrupt */
										kbc_ih();
										analyse_scancode(); 
									}
									break;
							default:
									break; /* no other notifications expected: do nothing */     
					}
			}
			else { /* received a standard message, not a notification */
					/* no standard messages expected: do nothing */
			}
	}

	if (kbd_unsubscribe_int(&bit_no)){
    return 1;
	}

	return 0;
}

int(video_test_rectangle)(uint16_t mode, uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color) {

	if (vg_init(mode) == NULL) {
		vg_exit();
		return 1;
	}

	if (vg_draw_rectangle(x, y, width, height, color)) {
		vg_exit();
		return 1;
	}

	if (kbd_esc_loop()) {
		vg_exit();
		return 1;
	}

	vg_exit();

	return 0;
}

int(video_test_pattern)(uint16_t mode, uint8_t no_rectangles, uint32_t first, uint8_t step) {
		if (vg_init(mode) == NULL) {
		vg_exit();
		return 1;
	}


	uint16_t width = vg_info.x_res / no_rectangles;
	uint16_t height = vg_info.y_res / no_rectangles;
	uint32_t color;

	if (vg_info.vg_mode == VG_MODE_INDEXED) {
		for (uint16_t i = 0; i < no_rectangles; ++i) { // Linhas, Row
			for (uint16_t j = 0; j < no_rectangles; ++j) { // Colunas, Col
					color = (first + (i * no_rectangles + j) * step) % BIT(vg_info.bits_per_pixel);
				vg_draw_rectangle(j * width, i * height, width, height, color);
			}
		}
	}

	else if (vg_info.vg_mode == VG_MODE_DIRECT) {
		uint16_t red, green, blue;
		uint16_t first_red = ((first & vg_info.red_mask) >> vg_info.red_field_position);
		uint16_t first_green = ((first & vg_info.green_mask) >> vg_info.green_field_position);
		uint16_t first_blue = ((first & vg_info.blue_mask) >> vg_info.blue_field_position);
		
		for (uint16_t i = 0; i < no_rectangles; ++i) { // Linhas, Row
			for (uint16_t j = 0; j < no_rectangles; ++j) { // Colunas, Col
			
				red = (first_red + j * step) % BIT(vg_info.red_mask_size);
				green = (first_green + i * step) % BIT(vg_info.green_mask_size);
				blue = (first_blue + (j + i) * step) % BIT(vg_info.blue_mask_size);

				color = 0 | (red << vg_info.red_field_position) | (green << vg_info.green_field_position) | 	(blue << vg_info.blue_field_position);
					
				vg_draw_rectangle(j * width, i * height, width, height, color);
			}
		}
	}
	else {
		vg_exit();
		return 1;
	}


	if (kbd_esc_loop()) {
		vg_exit();
		return 1;
	}

	vg_exit();

	return 0;
}

int(video_test_xpm)(xpm_map_t xpm, uint16_t x, uint16_t y) {

    if (vg_init(0x105) == NULL) {
			vg_exit();
			return 1;
    }

		if (vg_draw_xpm(xpm, x, y)) {
			vg_exit();
			return 1;
		}

    if (kbd_esc_loop()) {
			vg_exit();
			return 1;
    }

    vg_exit();

    return 0;
}

int(video_test_move)(xpm_map_t xpm, uint16_t xi, uint16_t yi, uint16_t xf, uint16_t yf,
					 int16_t speed, uint8_t fr_rate) {
  /* To be completed */
  printf("%s(%8p, %u, %u, %u, %u, %d, %u): under construction\n",
		 __func__, xpm, xi, yi, xf, yf, speed, fr_rate);

  return 1;
}

int(video_test_controller)() {
  /* To be completed */
  printf("%s(): under construction\n", __func__);

  return 1;
}
