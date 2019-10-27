#include "i8042.h"
#include "utils.h"

extern bool mouse_ih_error, is_mouse_packet_complete;
extern struct packet mouse_parsed_packet;

uint8_t (mouse_subscribe_int)(uint8_t *bit_no);
uint8_t (mouse_unsubscribe_int)();
void (mouse_ih)();
uint8_t (mouse_data_handler)();
uint8_t (kbc_send_cmd)(uint8_t port, uint8_t cmd);
