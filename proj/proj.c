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
    printf("Usage: game < player number >\n            0 - Singleplayer\n            1 - Player 1 (platformer)\n            2 - Player 2 (switchboard)\n        test\n");

    return 1;
}

int proj_game(enum PlayerNumber player_number) {
	if (vg_init(0x117) == NULL)
		return 1;

	start_game(player_number);

	vg_exit();

	return 0;
}

int test_uart(uint8_t tx) {
    uint16_t base_addr = COM1;
    printf("ADDR: %x\n", COM1);

    if (ser_test_set(base_addr, 1, 3, 0, 10))
        return 1;

    printf("\n");

    if (ser_test_conf(base_addr))
        return 1;
    
    char* strings[3] = {"a."};

    if (ser_test_poll(base_addr, tx, 8, 2, -1, 100, 1, strings))
        return 1;

    return 0;
}

int (proj_main_loop)(int argc, char *argv[]) {
	if (argc <= 0 || argc > 2)
		return print_usage();

	if (strcmp(argv[0], "game") == 0) {
        enum PlayerNumber pn = SINGLEPLAYER;
        uint8_t selected_player = (uint8_t) (*argv[1] - '0');
        
        if (selected_player == 0)
            pn = SINGLEPLAYER;
        
        else if (selected_player == 1)
            pn = PLAYER_1;

        else if (selected_player == 2)
            pn = PLAYER_2;
        
        else {
            printf("Invalid player number\n");
            return 1;
        }
            
        return proj_game(pn);
	}

	else if (strcmp(argv[0], "test") == 0) {
        uint8_t tx = (uint8_t) (*argv[1] - '0');

        if (!tx)
            printf("Configured to receive data\n");

        else
            printf("Configured to send data\n");

		test_uart(tx);

		return 1;
	}

	else {
		printf("Invalid command\n");
		return 1;
	}
}
