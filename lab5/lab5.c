// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>

#include <lcom/lab5.h>

// Any header files included below this line should have been created by you

#include "video.h"
#include "keyboard.h"
#include "timer.h"
#include "utils.h"

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

	uint16_t period_per_frame = 60 / fr_rate;
	int16_t x_delta = 0, y_delta = 0;
	uint16_t old_x, old_y;
	bool animation_ongoing = true;

	// x and y deltas are the number of pixels to move each update
	if (speed > 0) {
		// How many pixels to move per update
		// period_per_frame *= 1;
		if (xi == xf) {
			// Vertical movement
			y_delta = speed;
		}
		else {
			// Horizontal movement
			x_delta = speed;
		}
	}
	else {
		// Number of frames to wait to move 1 pixel
		// Speed is already negative here
		period_per_frame *= -speed;
		if (xi == xf) {
			// Vertical movement
			y_delta = 1;
		}
		else {
			// Horizontal movement
			x_delta = 1;
		}
	}
	if (xf < xi)
		x_delta *= -1;
	if (yf < yi)
		y_delta *= -1;
	
	// printf("Frame period: %d\nDelta: (%d, %d)\n", period_per_frame, x_delta, y_delta);

	// Set mode 0x105
	if (vg_init(0x105) == NULL) {
		vg_exit();
		return 1;
	}

	// draw first xpm
	if (vg_draw_xpm(xpm, xi, yi)) {
		vg_exit();
		return 1;
	}

	// Keyboard
 	uint8_t kbd_bit_no = KBD_IRQ;
	uint8_t timer0_bit_no = TIMER0_IRQ;
        
	// Only avoids making this operation on every notification
	uint32_t kbd_bit_mask = BIT(kbd_bit_no);
	uint32_t timer0_bit_mask = BIT(timer0_bit_no);
    
	if (kbd_subscribe_int(&kbd_bit_no)) {
		vg_exit();
    return 1;
	}

	if (timer_subscribe_int(&timer0_bit_no)) {
		vg_exit();
		kbd_unsubscribe_int();
		return 1;
	}
    
	int r, ipc_status;
  	message msg;

	if (x_delta > 0 || y_delta > 0) {
		// Interrupt loop
		while (scancode != ESC_BREAKCODE) { 
				if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
						printf("driver_receive failed with: %d", r);
						continue;
				}

				if (is_ipc_notify(ipc_status)) {
						switch (_ENDPOINT_P(msg.m_source)) {
								case HARDWARE: /* hardware interrupt notification */
										if (msg.m_notify.interrupts & timer0_bit_mask) {
											timer_int_handler();
											if (animation_ongoing) {
												if (global_timer0_counter >= period_per_frame) {
													// Update coordinate values
													old_x = xi;
													old_y = yi;
													xi = xi + x_delta;
													yi = yi + y_delta;

													if ((xi >= xf && x_delta) || (yi >= yf && y_delta)) {
														// Fix final movement
														vg_update_xpm(xpm, old_x, old_y, xf, yf);
														animation_ongoing = false;
													}
													else {
														vg_update_xpm(xpm, old_x, old_y, xi, yi);
													}
													// Reset counter
													global_timer0_counter = 0;
												}
											}
										}
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
	}
	else
	{
		// Interrupt loop
		while (scancode != ESC_BREAKCODE) { 
				if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
						printf("driver_receive failed with: %d", r);
						continue;
				}

				if (is_ipc_notify(ipc_status)) {
						switch (_ENDPOINT_P(msg.m_source)) {
								case HARDWARE: /* hardware interrupt notification */
										if (msg.m_notify.interrupts & timer0_bit_mask) {
											timer_int_handler();
											if (animation_ongoing) {
												if (global_timer0_counter >= period_per_frame) {
													// Update coordinate values
													old_x = xi;
													old_y = yi;
													xi = xi + x_delta;
													yi = yi + y_delta;

													if ((xi <= xf && x_delta) || (yi <= yf && y_delta)) {
														// Fix final movement
														vg_update_xpm(xpm, old_x, old_y, xf, yf);
														animation_ongoing = false;
													}
													else {
														vg_update_xpm(xpm, old_x, old_y, xi, yi);
													}
													// Reset counter
													global_timer0_counter = 0;
												}
											}
										}
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
	}
	
	kbd_unsubscribe_int();
	timer_unsubscribe_int();
	vg_exit();

	return 0;
}

int(video_test_controller)() {
  
	vbe_controller_straight_from_hell_t ctrl;

	mmap_t map;
	if(lm_alloc(sizeof(vbe_controller_straight_from_hell_t), &map) == NULL) {
    printf("Error: failed to allocate memory block.");
    return 1;
  }

	char* vbe_signature_ptr = map.virt;

	// VBE_CONTROLLER_SIGNATURE_VBE2 = 0x0200
	vbe_signature_ptr[0] = 'V';
	vbe_signature_ptr[1] = 'B';
	vbe_signature_ptr[2] = 'E';
	vbe_signature_ptr[3] = '2';

	reg86_t reg;
	memset(&reg, 0, sizeof(reg86_t));

	// Set function
	reg.ah = VBE_AH_FUNCTION_INDICATOR;
	reg.al = VBE_AL_CONTROLLER_INFO;
	// The struct to save the information
	reg.es = PB2BASE(map.phys);
	reg.di = PB2OFF(map.phys);
	// Set interrupt
	reg.intno = VBE_INTERRUPT_NUMBER;

	if(sys_int86(&reg) != OK ) {
		printf("vbe_get_controller_info: sys_int86() failed \n");
		return 1;
	}

	if (reg.ah != VBE_FUNCTION_RET_SUCCESS || reg.al != VBE_AH_FUNCTION_INDICATOR) {
		printf("vbe_get_controller_info: function return value different from success: %x\n", reg.ax);
		printf("reg.ah: %x reg.ax: %x reg.al: %x\n", reg.ah, reg.ax, reg.al);
		return 1;
	}

	ctrl = *((vbe_controller_straight_from_hell_t*) map.virt);
	if (!lm_free(&map)) {
		printf("vbe_get_mode_info_ours: couldn't deallocate memory in the low memory region\n");
		return 1;
	}

	// PROCESS CRTL HERE

	vg_vbe_contr_info_t ctrl_summary;
	ctrl_summary.VBESignature[0] = ctrl.vbe_signature[0];
	ctrl_summary.VBESignature[1] = ctrl.vbe_signature[1];
	ctrl_summary.VBESignature[2] = ctrl.vbe_signature[2];
	ctrl_summary.VBESignature[3] = ctrl.vbe_signature[3];
	// these 2 may be in the wrong order
	util_get_LSB(ctrl.vbe_version, &ctrl_summary.VBEVersion[0]);
	util_get_MSB(ctrl.vbe_version, &ctrl_summary.VBEVersion[1]);
	// Total memory is in 64KB blocks, so we need to multiply by 64, aka shift left 6
	ctrl_summary.TotalMemory = ctrl.total_memory << 6;

	// pointer time :(

	// oem string
	uint8_t size = 0;
	char* str_ptr = linear_to_virt(far_ptr_to_linear(ctrl.oem_string_ptr), &map);
	for (char *aux = str_ptr; *aux != '\0'; ++aux) {
		++size;
	}
	++size;
	ctrl_summary.OEMString = (char *) malloc(sizeof(char) * size);
	strcpy(ctrl_summary.OEMString, str_ptr);

	//vendor name
	size = 0;
	str_ptr = linear_to_virt(far_ptr_to_linear(ctrl.vendor_name_ptr), &map);
	for (char *aux = str_ptr; *aux != '\0'; ++aux) {
		++size;
	}
	++size;
	ctrl_summary.OEMVendorNamePtr = (char *) malloc(sizeof(char) * size);
	strcpy(ctrl_summary.OEMVendorNamePtr, str_ptr);

	//vendor product name
	size = 0;
	str_ptr = linear_to_virt(far_ptr_to_linear(ctrl.vendor_product_name_ptr), &map);
	for (char *aux = str_ptr; *aux != '\0'; ++aux) {
		++size;
	}
	++size;
	ctrl_summary.OEMProductNamePtr = (char *) malloc(sizeof(char) * size);
	strcpy(ctrl_summary.OEMProductNamePtr, str_ptr);

	//vendor product rev
	size = 0;
	str_ptr = linear_to_virt(far_ptr_to_linear(ctrl.vendor_product_rev_ptr), &map);
	for (char *aux = str_ptr; *aux != '\0'; ++aux) {
		++size;
	}
	++size;
	ctrl_summary.OEMProductRevPtr = (char *) malloc(sizeof(char) * size);
	strcpy(ctrl_summary.OEMProductRevPtr, str_ptr);
	
	// video mode list

	uint16_t *video_mode_list = linear_to_virt(far_ptr_to_linear(ctrl.video_mode_list_ptr), &map);
	size = 0;
	for (uint16_t *aux = video_mode_list; *aux != VBE_CONTROLLER_MODE_LIST_TERMINATOR; ++aux) {
		++size;
	}
	// One extra for the terminator
	++size;
	// printf("Ran for: %d\n", size);

	// printf("Copying now\n");
	ctrl_summary.VideoModeList = (uint16_t*) malloc(sizeof(uint16_t) * size);
	uint16_t *aux = ctrl_summary.VideoModeList;
	for (uint8_t i = 0; i < size; ++i) {
		// printf("Iteration: %d\n", i);
		*aux = *video_mode_list;
		++video_mode_list;
		++aux;
	}

	
	vg_display_vbe_contr_info(&ctrl_summary);

	// END FUNCTION HERE

	// frees here
	free(ctrl_summary.OEMString);
	free(ctrl_summary.OEMVendorNamePtr);
	free(ctrl_summary.OEMProductNamePtr);
	free(ctrl_summary.OEMProductRevPtr);
	free(ctrl_summary.VideoModeList);

	return 0;
	
}
