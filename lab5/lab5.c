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
	vg_init(mode);

	sleep(delay);

	vg_exit();

	return 0;
}

int(video_test_rectangle)(uint16_t mode, uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color) {

	vg_init(mode);

	if (vg_draw_rectangle(x, y, width, height, color))
		return 1;

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
		vg_exit();
    return 1;
	}

	vg_exit();

	return 0;
}

int(video_test_pattern)(uint16_t mode, uint8_t no_rectangles, uint32_t first, uint8_t step) {
  /* To be completed */
  printf("%s(0x%03x, %u, 0x%08x, %d): under construction\n", __func__,
		 mode, no_rectangles, first, step);

  return 1;
}

int(video_test_xpm)(xpm_map_t xpm, uint16_t x, uint16_t y) {
  /* To be completed */
  printf("%s(%8p, %u, %u): under construction\n", __func__, xpm, x, y);

  return 1;
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
