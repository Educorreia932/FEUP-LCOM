/**
 * Queue implementation using linked list in C.
 * Credit to https://codeforwin.org/2018/08/queue-implementation-using-linked-list-in-c.html
 * Major changes done by us though
 */

#include "queue.h"

#define CAPACITY 200    // Queue max capacity

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

    free(q);

    return;
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
    if ( q->rear )
        q->rear->next = newNode;
    

    // Make sure newly created node is at rear
    q->rear = newNode;

    // Link first node to front if its NULL
    if ( ! q->front )
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
    return (q->size >= CAPACITY);
}
