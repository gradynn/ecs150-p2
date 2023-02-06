#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

#define QUEUE_SIZE 10

struct queue {
	int front;
	int rear;
	int size;
	int capacity;
	void** q;
};

queue_t queue_create(void)
{
	queue_t queue = (queue_t) malloc(sizeof(struct queue));
	queue->front = 0;
	queue->rear = 0;
	queue->size = 0;
	queue->capacity = QUEUE_SIZE;
	queue->q = (void**) malloc(sizeof(void*) * QUEUE_SIZE);

	return queue;
}

int queue_destroy(queue_t queue)
{
	if (queue == NULL) {
		return -1;
	}
	free(queue->q);
	free(queue);
	return 0;
}

int queue_enqueue(queue_t queue, void *data)
{
	if (queue == NULL || data == NULL)
		return -1;

	int i = queue->rear;
	if (i == queue->capacity) {
		queue->q = realloc(queue->q, 2 * sizeof(*queue->q));
		queue->capacity = 2 * queue->capacity;
	}
	
	queue->q[i] = data;
	queue->rear += 1;
	queue->size += 1;

	return 0;
}

int queue_dequeue(queue_t queue, void **data)
{
	if (queue == NULL || data == NULL || queue->size == 0)
		return -1;

	data = &queue->q[queue->front];
	queue->front += 1;
	queue->size -= 1;
	
	return 0;
}

int queue_delete(queue_t queue, void *data)
{
	if (queue == NULL || data == NULL)
		return -1;

	int i = queue->front;
	while (i <= queue->rear) {
		if (queue->q[i] != NULL) {
			if (memcmp(queue->q[i], data, sizeof(*data)) == 0) {
				queue->q[i] = NULL;
				queue->size -= 1;

				return 0;
			}
		}
	}

	return -1;
}

int queue_iterate(queue_t queue, queue_func_t func)
{
	if (queue == NULL || func == NULL)
		return -1;

	int i = queue->front;
	while (i <= queue->rear) {
		if (queue->q[i] != NULL)
			func(queue, queue->q[i]);
		i++;
	}

	return 0;
}

int queue_length(queue_t queue)
{
	if (queue == NULL)
		return -1;
	
	return queue->size;
}