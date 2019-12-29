#include <lcom/lcf.h>

typedef struct Queue Queue_t;

Queue_t* new_queue();
void free_queue(Queue_t *q);

uint8_t queue_push(Queue_t *q, uint8_t data);
uint8_t queue_pop(Queue_t *q);
//uint8_t queue_rear(Queue_t *q);
uint8_t queue_top(Queue_t *q);
bool queue_is_empty(Queue_t *q);
bool queue_is_full(Queue_t *q);
