/**
  * @details Queue implementation using linked list in C\n
  * Inspired by https://codeforwin.org/2018/08/queue-implementation-using-linked-list-in-c.html \n
  * This original implementation was REALLY altered\n
  * It was full of bugs, you could only have one queue per program, the datatype was  wrong, it has major bugs (memory leaks, wrong pointers that impacted the rest of the program), it was only two nodes (front and rear) that you had to play around with both of them as arguments\n
  * TL:DR\n
  * It was buggy as impossible to work with, so we added a LOT of code, fixed all its bugs and implemented a proper Queue struct
 */

#include "queue.h"

/* Queue structure definition */
typedef struct node 
{
    uint8_t data;
    struct node * next;
} node_t;

// Kinda like a deque because I can access the bottom, but who cares
struct Queue {
    node_t *front, *rear;
    unsigned int size;
};

Queue_t* new_queue() {
    Queue_t *q = (Queue_t*) calloc(1, sizeof(Queue_t));
    if (q == NULL) {
        printf("new_queue: Failed to create the new queue\n");
        return NULL;
    }
    q->size = 0;
    // front and rear are already NULL
    return q;
}

void free_queue(Queue_t* q) {
    queue_clear(q);
    free(q);
}

void queue_clear(Queue_t* q) {

    node_t *cur_node = q->front, *next_node;

    if (q->front == q->rear) {
        free(q->front);
    }
    else {
        while (cur_node != NULL) {
            next_node = cur_node->next;
            free(cur_node);
            cur_node = next_node;
        }
    }

    q->front = NULL;
    q->rear = NULL;
    q->size = 0;
}

/**
 * Enqueues/Insert an element at the rear of a queue.
 * Function returns 1 on success otherwise returns 0.
 */
uint8_t queue_push_back(Queue_t *q, uint8_t data)
{
    node_t * newNode = NULL;

    // Check queue out of capacity error
    if (queue_is_full(q))
    {
        return 1;
    }

    // Create a new node of queue type
    newNode = (node_t *) malloc (sizeof(node_t));

    // Assign data to new node
    newNode->data = data;

    // Initially new node does not point anything
    newNode->next = NULL;

    // Link new node with existing last node 
    if ( q->rear != NULL )
        q->rear->next = newNode;
    

    // Make sure newly created node is at rear
    q->rear = newNode;

    // Link first node to front if its NULL
    if ( q->front == NULL )
    {
        q->front = q->rear;
    }

    // Increment quque size
    q->size++;

    return 0;
}


/**
 * Dequeues/Removes an element from front of the queue.
 * It returns the element on success otherwise returns 
 * 0xFF as error code.
 */
uint8_t queue_pop(Queue_t *q)
{
    node_t *toDequque = NULL;
    uint8_t data = 0xFF;

    // Queue empty error
    if (queue_is_empty(q))
    {
        return 0xFF;
    }

    // Get element and data to dequeue
    toDequque = q->front;
    data = toDequque->data;

    // Move front ahead
    q->front = q->front->next;

    // Decrement size
    --q->size;

    // If the queue is now empty, set the rear to NULL
    if (q->size == 0) {
        q->rear = NULL;
    }

    // Clear dequeued element from memory
    free(toDequque);

    return data;
}


/**
 * Gets, element at rear of the queue. It returns the element
 * at rear of the queue on success otherwise return 0xFF as 
 * error code.
 */
uint8_t queue_back(Queue_t *q)
{
    // Return INT_MIN if queue is empty otherwise rear.
    return (queue_is_empty(q))
            ? 0xFF
            : q->rear->data;

}


/**
 * Gets, element at front of the queue. It returns the element
 * at front of the queue on success otherwise return 0xFF as 
 * error code.
 */
uint8_t queue_front(Queue_t *q)
{
    // Return 0xFF if queue is empty otherwise front.
    return (queue_is_empty(q))
            ? 0xFF
            : q->front->data;
}


/**
 * Checks, if queue is empty or not.
 */
bool queue_is_empty(Queue_t *q)
{
    return (q->size <= 0);
}


/**
 * Checks, if queue is within the maximum queue capacity.
 */
bool queue_is_full(Queue_t *q)
{
    return (q->size >= QUEUE_MAX_CAPACITY);
}

/**
 * @brief Returns the current size
 * 
 * @param q 
 * @return unsigned int 
 */
unsigned int queue_size(Queue_t *q) {
    return q->size;
}
