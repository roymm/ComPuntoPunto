#ifndef QUEUE_H
#define QUEUE_H

#include <stdbool.h>
#include <stddef.h>

/**
An opaque record that represents a node of a simply-linked list.
*/
typedef struct queue_node queue_node_t;

/**
An opaque record that contains attributes for managing a queue of pointers.
*/
typedef struct queue queue_t;

/**
An opaque iterator for traversing a queue of pointers.
*/
typedef queue_node_t* queue_iterator_t;

/**
 * @brief Create an empty queue
 * @return Pointer to the new created queue in heap memory.
 * @remarks The pointed queue must be freed with @a queue_destroy().
 */
queue_t* queue_create();

/// Appends an element to the queue
/// @return A pointer to the data on success, NULL on error
void* queue_append(queue_t* queue, void* data);

/// Returns a pointer to the data in the first element of the queue
/// The element is not removed from the queue
void* queue_peek(queue_t* queue);

/// Return the number of elements in the queue
size_t queue_count(const queue_t* queue);

/// Return true if the given queue is empty
bool queue_is_empty(const queue_t* queue);

/// Clear all elements from a queue
/// @param queue The queue to be cleared
/// @param remove_data true if free() must be called for the data in each element
void queue_clear(queue_t* queue, bool remove_data);

/// Destroy a queue
void queue_destroy(queue_t* queue, bool remove_data);

/// Extracts the first element from the queue and returns its data
void* queue_pop(queue_t* queue);


/// Returns an iterator to the first element of the queue
queue_iterator_t queue_begin(queue_t* queue);

/// Returns an iterator that represents an invalid element
queue_iterator_t queue_end(queue_t* queue);

/// Moves the iterator to the next element
queue_iterator_t queue_next(queue_iterator_t iterator);

/// Returns the data pointed by the given iterator
void* queue_data(queue_iterator_t iterator);

/// Sorts the queue.
//void queue_sort(queue_t* queue);
#endif // QUEUE_H
