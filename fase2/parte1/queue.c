#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"
//#include "array.h"

/// A generic node of the queue
typedef struct queue_node
{
	/// Pointer to the user data
	void* data;
	/// Next node in the queue
	struct queue_node* next;
} queue_node_t;

/// A queue of user data elements
typedef struct queue
{
	/// Number of elements in the queue
	size_t count;
	/// Pointer to the first element in the queue
	queue_node_t* head;
	/// Pointer to the last element in the queue
	queue_node_t* tail;
} queue_t;


queue_t* queue_create()
{
	// Create a record in dynamic memory with all its bits in zero
	queue_t* queue = calloc( 1, sizeof(queue_t) );
	assert(queue);
	return queue;
}

void* queue_append(queue_t* queue, void* data)
{
	assert(queue);

	// Create the node and copy the data
	queue_node_t* node = calloc(1, sizeof(queue_node_t));
	if ( node == NULL ) return NULL;
	node->data = data;

	// Append the new node to the queue
	if ( queue_is_empty(queue) )
		queue->head = queue->tail = node;
	else
		queue->tail = queue->tail->next = node;

	// Return a pointer to the data
	++queue->count;
	return node->data;
}

void* queue_peek(queue_t* queue)
{
	assert(queue);
	assert(queue_is_empty(queue) == false);

	return queue->head->data;
}

size_t queue_count(const queue_t* queue)
{
	assert(queue);

	return queue->count;
}

bool queue_is_empty(const queue_t* queue)
{
	assert(queue);

	return queue->head == NULL;
}

void queue_destroy(queue_t* queue, bool remove_data)
{
	assert(queue);

	queue_clear(queue, remove_data);
	free(queue);
}

void queue_clear(queue_t* queue, bool remove_data)
{
	assert(queue);

	while ( ! queue_is_empty(queue) )
	{
		if ( remove_data )
			free ( queue_pop(queue) );
		else
			queue_pop(queue);
	}

	queue->head = queue->tail = NULL;
}

void* queue_pop(queue_t* queue)
{
	assert(queue);
	assert(queue_is_empty(queue) == false);

	// Get a pointer to the next node and its data
	queue_node_t* node = queue->head;
	void* data = node->data;

	// Move the head to the next node
	queue->head = queue->head->next;
	--queue->count;

	// If queue bacame empty, update the tail
	if ( queue_is_empty(queue) )
		queue->tail = NULL;

	// Release the memory of the node, not its data
	free(node);
	return data;
}


queue_iterator_t queue_begin(queue_t* queue)
{
	assert(queue);
	return queue->head;
}

queue_iterator_t queue_end(queue_t* queue)
{
	(void)queue;
	return NULL;
}

queue_iterator_t queue_next(queue_iterator_t iterator)
{
	assert(iterator);
	return iterator->next;
}

void* queue_data(queue_iterator_t iterator)
{
	assert(iterator);
	return iterator->data;
}

/*
void queue_sort(queue_t* queue)
{
	assert(queue);
	size_t queue_size = queue_count(queue);
	char** all_data = (char**)calloc(queue_size, sizeof(char*)); //array that holds copies of the nodes' data
	int current = 0;
	for(queue_iterator_t i = queue_begin(queue); i != queue_end(queue); i = queue_next(i), ++current)
		all_data[current] = strdup((char*)queue_data(i));
	array_qsort(all_data, 0, (int)queue_size-1); //sorts the copies with array_sort
	current = 0;
	for(queue_iterator_t i = queue_begin(queue); i != queue_end(queue); i = queue_next(i), ++current){
		free(queue_data(i)); //frees the unsorted data of the nodes
		((queue_node_t*)i)->data = (void*) strdup(all_data[current]); //copies into the nodes the sorted data
		free(all_data[current]); //frees the copies
	}
	free(all_data);
}*/
