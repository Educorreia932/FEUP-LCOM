#pragma once

#include <lcom/lcf.h>

/** @file queue.h */

/** @defgroup queue Queue */
/** @addtogroup queue
  *
  * @brief	Handles a Queue of uint8_t data types
  *
  * @{
  */

/**
 * @brief The max size of any Queue
 * 
 */
#define QUEUE_MAX_CAPACITY 200

/**
 * @brief Queue data structure (FIFO)
 * @details Implemented using a linked list
 * @warning Has a maxsize defined on the macro QUEUE_MAX_CAPACITY
 * 
 */
typedef struct Queue Queue_t;

/**
 * @brief Called to create a new Queue
 * 
 * @return Queue_t* Pointer to the Queue upon success\n
 * NULL otherwise
 */
Queue_t* new_queue();
/**
 * @brief Frees an entire Queue (alongside all of its stored data)
 * 
 * @param q Pointer to the Queue to be freed
 */
void free_queue(Queue_t *q);

/**
 * @brief Pushes an element onto the back of the Queue
 * 
 * @param q Pointer to the Queue to operate over
 * @param data The data to be pushed back
 * @return uint8_t 1 on success\n
 * 0 otherwise
 */
uint8_t queue_push_back(Queue_t *q, uint8_t data);
/**
 * @brief Removes an element from the front of the Queue, returning it in the process
 * 
 * @param q Pointer to the Queue to operate over
 * @return uint8_t The data of the element popped on success\n
 * 0xFF otherwise
 */
uint8_t queue_pop(Queue_t *q);
/**
 * @brief Peeks and returns the element on the back of the Queue
 * 
 * @param q Pointer to the Queue to operate over
 * @return uint8_t The data of the element on the back of the Queue on success\n
 * 0xFF otherwise
 */
uint8_t queue_back(Queue_t *q);
/**
 * @brief Peeks and returns the element on the front of the Queue
 * 
 * @param q Pointer toThe data of the element on the front of the Queue on success\n
 * 0xFF otherwise the Queue to operate over
 * @return uint8_t 
 */
uint8_t queue_front(Queue_t *q);
/**
 * @brief Returns true when the Queue is empty
 * 
 * @param q Pointer to the Queue to operate over
 * @return True when the Queue is empty\n
 * False otherwise
 */
bool queue_is_empty(Queue_t *q);
/**
 * @brief Returns true when the Queue is at its max capacity
 * 
 * @param q Pointer to the Queue to operate over
 * @return True when the number of elements of the Queue is at/over the QUEUE_MAX_CAPACITY
 * False otherwise
 */
bool queue_is_full(Queue_t *q);
/**
 * @brief Returns the current size of the Queue (the number of elements stored)
 * 
 * @param q Pointer to the Queue to operate over
 * @return unsigned int The number of elements in the Queue
 */
unsigned int queue_size(Queue_t *q);

/** @} */
