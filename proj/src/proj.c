// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>
#include <lcom/liblm.h>
#include <lcom/proj.h>

#include <stdbool.h>
#include <stdint.h>

// Any header files included below this line should have been created by you
#include "game_manager.h"
#include "uart.h"

int main(int argc, char *argv[]) {
    
    // sets the language of LCF messages (can be either EN-US or PT-PT)
    lcf_set_language("EN-US");

    // enables to log function invocations that are being "wrapped" by LCF
    // [comment this out if you don't want/need it]
    lcf_trace_calls("/home/lcom/labs/proj/trace_2.txt");

    // enables to save the output of printf function calls on a file
    // [comment this out if you don't want/need it]
    lcf_log_output("/home/lcom/labs/proj/output_2.txt");

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
    printf("Usage: level | (sw | board) | arcade\n");

    return 1;
}


int (proj_main_loop)(int argc, char *argv[]) {
	if (argc <= 0 || argc > 2)
		return print_usage();

    if (strcmp(argv[0], "sp") == 0) {
        return start_game(GM_LEVEL);
    }
	if (strcmp(argv[0], "level") == 0) {
        return start_game(GM_LEVEL | GM_UART);
	}
    else if (strcmp(argv[0], "sw") == 0 || strcmp(argv[0], "board") == 0) {
        return start_game(GM_SWITCHBOARD | GM_UART);
    }
    else if (strcmp(argv[0], "arcade") == 0) {
        return start_game(GM_ARCADE);
    }
    // Add multiplayer arcade later down the road

	else if (strcmp(argv[0], "test") == 0) {

        uint8_t tx = (uint8_t) (*argv[1] - '0');

		return test_uart(tx);
	}

	else {
		printf("Invalid command\n");
		return 1;
	}
}
