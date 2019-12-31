#pragma once

#include <lcom/lcf.h>

typedef struct Queue Queue_t;

Queue_t* new_queue();
void free_queue(Queue_t *q);

uint8_t queue_push_back(Queue_t *q, uint8_t data);
uint8_t queue_pop(Queue_t *q);
uint8_t queue_back(Queue_t *q);
uint8_t queue_front(Queue_t *q);
bool queue_is_empty(Queue_t *q);
bool queue_is_full(Queue_t *q);
unsigned int queue_size(Queue_t *q);
