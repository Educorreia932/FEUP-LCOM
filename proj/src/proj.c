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
    // lcf_trace_calls("/home/lcom/labs/proj/trace_2.txt");

    // enables to save the output of printf function calls on a file
    // [comment this out if you don't want/need it]
    // lcf_log_output("/home/lcom/labs/proj/output_2.txt");

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
    printf("Usage: literally just 'lcom_run proj'\n");

    return 1;
}


int (proj_main_loop)(int argc, char *argv[]) {
	if (argc <= 0)
        return start_game(GM_MAIN_MENU);

	else {
		printf("Invalid command\n");
        print_usage();
		return 1;
	}
}
