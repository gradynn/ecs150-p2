#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "queue.h"

typedef struct node* node_t;

/* 
 *	Linked list node structure
*/
struct node {
	void* data;
	struct node* next;
};

/*
 *	Queue structure, implemented as a linked list
*/
struct queue {
	node_t head;
	node_t tail;
	int length;
};

/*
 *	Queue constructor
 */
queue_t queue_create(void)
{
	queue_t queue = (queue_t) malloc(sizeof(struct queue));

	if (queue == NULL)
		return NULL;

	queue->length = 0;
	return queue;
}

/*
 *	Queue destructor
 */
int queue_destroy(queue_t queue)
{
	if (queue == NULL || queue->length != 0)
		return -1;

	free(queue);

	return 0;
}


int queue_enqueue(queue_t queue, void *data)
{
	if (queue == NULL || data == NULL)
		return -1;

	node_t new_node = (node_t) malloc(sizeof(struct node));
	if (new_node == NULL)
		return -1;
	new_node->data = data;

	// Differentiates between enqueueing the first item and all subsequent items
	if (queue->length == 0) {
		queue->head = new_node;
		queue->tail = new_node;
	} else {
		queue->tail->next = new_node;
		queue->tail = new_node;
	}

	queue->tail->next = NULL;
	queue->length++;

	return 0;
}

int queue_dequeue(queue_t queue, void **data)
{
	if (queue == NULL || data == NULL || queue->length == 0)
		return -1;
	
	// assigns data to passed in pointer
	*data = queue->head->data;

	// increments head and frees old head
	node_t temp = queue->head;
	queue->head = queue->head->next;
	free(temp);

	queue->length--;

	return 0;
}

/*
 *	Searches queue for data and deletes it if found
 */
int queue_delete(queue_t queue, void *data)
{
	if (queue == NULL || data == NULL)
		return -1;

	// Pointers to track current and previous node
	node_t current = queue->head;
	node_t prev = NULL;

	// Iterates through queue until data is found
	while(current != NULL) {
		if (current->data == data) {
			// Differentiates between first item, last item, and all other items 
			if(current == queue->head) {
				queue->head = current->next;
			} else if (current == queue->tail) {
				queue->tail = prev;
				prev->next = NULL;
			} else {
				prev->next = current->next;	
			}

			free(current->data);
			free(current);

			queue->length--;
			
			return 0;
		}

		// Increment pointers
		prev = current;
		current = current->next;
	}

	return -1;
}


/*
	Iterates through queue and calls user specified function on each item
*/
int queue_iterate(queue_t queue, queue_func_t func)
{
	if (queue == NULL || func == NULL)
		return -1;

	node_t current = queue->head;
	while (current != NULL) {
		func(queue, current->data);
		current = current->next;
	}

	return 0;
}

/*
 *	Returns the length of the queue
*/
int queue_length(queue_t queue)
{
	if (queue == NULL)
		return -1;

	return queue->length;
}