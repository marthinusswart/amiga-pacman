#ifndef QUEUE_ROUTINES_H
#define QUEUE_ROUTINES_H

#include <exec/types.h>

// Key-Value Pair structure
typedef struct KeyValuePair {
    void *key;      // Generic key pointer
    void *value;    // Generic value pointer
} KeyValuePair;

// Queue node structure for linked list implementation
typedef struct QueueNode {
    KeyValuePair data;
    struct QueueNode *next;
} QueueNode;

// Queue structure
typedef struct Queue {
    QueueNode *front;   // Points to the front of the queue
    QueueNode *rear;    // Points to the rear of the queue
    ULONG size;         // Number of elements in the queue
} Queue;

// Queue functions
Queue* queueCreate(void);
void queueDestroy(Queue *queue);
BOOL queuePush(Queue *queue, void *key, void *value);
KeyValuePair* queuePop(Queue *queue);
BOOL queueIsEmpty(Queue *queue);
ULONG queueSize(Queue *queue);

#endif // QUEUE_ROUTINES_H
