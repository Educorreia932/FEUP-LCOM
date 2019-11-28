// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>
#include <lcom/liblm.h>
#include <lcom/proj.h>

#include <stdbool.h>
#include <stdint.h>

// Any header files included below this line should have been created by you
#include "game_manager.h"

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  // lcf_trace_calls("/home/lcom/labs/proj/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  // lcf_log_output("/home/lcom/labs/proj/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

static int print_usage() {
  printf("Usage: game < player | 1 or 2 >\n        test\n");

  return 1;
}

int proj_game(uint8_t player_number) {
  if (vg_init(0x117) == NULL)
    return 1;

  start_game(player_number);

  vg_exit();

  return 0;
}

int(proj_main_loop)(int argc, char *argv[]) {
	if (argc < 0 || argc > 2)
		return print_usage();

	if (strcmp(argv[0], "game") == 0) {
    	return proj_game((uint8_t) (*argv[1] - '0'));
	}

	else if (strcmp(argv[0], "test") == 0) {
		panic("No code to test");
		return 1;
	}
	
	else {
		printf("Invalid command\n");
		return 1;
	}
}
