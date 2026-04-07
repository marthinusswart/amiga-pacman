#include "queue_routines.h"
#include <proto/exec.h>

// ============================================================================
// Queue Implementation with Key-Value Pairs
// ============================================================================

/**
 * Creates a new empty queue
 * Returns: Pointer to the newly created queue, or NULL if allocation fails
 */
Queue* queueCreate(void)
{
    Queue *queue = (Queue *)AllocMem(sizeof(Queue), MEMF_PUBLIC | MEMF_CLEAR);
    if (!queue)
        return NULL;

    queue->front = NULL;
    queue->rear = NULL;
    queue->size = 0;

    return queue;
}

/**
 * Destroys the queue and frees all nodes
 * Note: Does NOT free the key/value pointers themselves - caller must manage those
 * Parameters:
 *   queue - The queue to destroy
 */
void queueDestroy(Queue *queue)
{
    if (!queue)
        return;

    // Free all nodes in the queue
    QueueNode *current = queue->front;
    while (current)
    {
        QueueNode *next = current->next;
        FreeMem(current, sizeof(QueueNode));
        current = next;
    }

    // Free the queue structure itself
    FreeMem(queue, sizeof(Queue));
}

/**
 * Pushes a key-value pair onto the rear of the queue
 * Parameters:
 *   queue - The queue to push to
 *   key   - Pointer to the key (stored as-is, not copied)
 *   value - Pointer to the value (stored as-is, not copied)
 * Returns: TRUE if successful, FALSE if allocation fails
 */
BOOL queuePush(Queue *queue, void *key, void *value)
{
    if (!queue)
        return FALSE;

    // Allocate a new node
    QueueNode *newNode = (QueueNode *)AllocMem(sizeof(QueueNode), MEMF_PUBLIC | MEMF_CLEAR);
    if (!newNode)
        return FALSE;

    // Set the data
    newNode->data.key = key;
    newNode->data.value = value;
    newNode->next = NULL;

    // If queue is empty, this becomes both front and rear
    if (queue->rear == NULL)
    {
        queue->front = newNode;
        queue->rear = newNode;
    }
    else
    {
        // Add to the rear and update rear pointer
        queue->rear->next = newNode;
        queue->rear = newNode;
    }

    queue->size++;
    return TRUE;
}

/**
 * Pops a key-value pair from the front of the queue
 * Parameters:
 *   queue - The queue to pop from
 * Returns: Pointer to the KeyValuePair data (statically allocated in the node)
 *          Caller should copy the data before next pop/destroy operation
 *          Returns NULL if queue is empty
 * Note: The popped node is freed, but the key/value pointers are NOT freed
 */
KeyValuePair* queuePop(Queue *queue)
{
    if (!queue || !queue->front)
        return NULL;

    // Get the front node
    QueueNode *frontNode = queue->front;
    static KeyValuePair result; // Static so it persists after return

    // Copy the data before freeing the node
    result.key = frontNode->data.key;
    result.value = frontNode->data.value;

    // Move front pointer to next node
    queue->front = frontNode->next;

    // If queue becomes empty, update rear pointer too
    if (queue->front == NULL)
        queue->rear = NULL;

    queue->size--;

    // Free the node
    FreeMem(frontNode, sizeof(QueueNode));

    return &result;
}

/**
 * Checks if the queue is empty
 * Parameters:
 *   queue - The queue to check
 * Returns: TRUE if empty or NULL, FALSE otherwise
 */
BOOL queueIsEmpty(Queue *queue)
{
    if (!queue)
        return TRUE;

    return (queue->front == NULL);
}

/**
 * Returns the current size of the queue
 * Parameters:
 *   queue - The queue to check
 * Returns: Number of elements in the queue, or 0 if NULL
 */
ULONG queueSize(Queue *queue)
{
    if (!queue)
        return 0;

    return queue->size;
}
