#include "i8042.h"
#include "utils.h"

extern bool mouse_ih_error, is_mouse_packet_complete;
extern struct packet mouse_parsed_packet;

uint8_t (mouse_subscribe_int)(uint8_t *bit_no);
uint8_t (mouse_unsubscribe_int)();
void (mouse_ih)();
uint8_t (mouse_data_handler)();

uint8_t mouse_data_reporting_enable();
uint8_t mouse_data_reporting_disable();
uint8_t mouse_set_stream_mode();
uint8_t mouse_set_remote_mode();
uint8_t mouse_read_data();
