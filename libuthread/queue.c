#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

typedef struct node* node_t;

struct node {
	void* data;
	struct node* next;
};

struct queue {
	node_t head;
	node_t tail;
	int length;
};

queue_t queue_create(void)
{
	/* TODO Phase 1 */
	queue_t queue = (queue_t) malloc(sizeof(struct queue));
	queue->length = 0;
	return queue;
}

int queue_destroy(queue_t queue)
{
	/* TODO Phase 1 */
	if (queue == NULL || queue->length != 0)
		return -1;

	free(queue);

	return 0;
}

int queue_enqueue(queue_t queue, void *data)
{
	/* TODO Phase 1 */
	node_t new_node = (node_t) malloc(sizeof(struct node));
	new_node->data = data;

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
	/* TODO Phase 1 */
	*data = queue->tail->data;

	node_t temp = queue->head;
	queue->head = queue->head->next;
	free(temp);

	queue->length--;

	return 0;
}

int queue_delete(queue_t queue, void *data)
{
	if (queue == NULL || data == NULL)
		return -1;

	node_t current = queue->head;

	while(current != NULL) {
		if (current->data == data) {
			free(current);
			return 0;
		}
		current = current->next;
	}

	return -1;
}



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

int queue_length(queue_t queue)
{
	return queue->length;
}