#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

#include "private.h"
#include "queue.h"
#include "sem.h"

struct semaphore {
	int count;
	queue_t waiting_queue;
};

enum stat {
	READY,
	RUNNING,
	BLOCKED,
	ZOMBIE
};

struct uthread_tcb {
	enum stat status;
	void* stack_ptr;
	uthread_ctx_t *context;
};

sem_t sem_create(size_t count)
{
	// Allocate space and initialize sem
	sem_t sem = malloc(sizeof(struct semaphore));
	
	if (sem == NULL)
		return NULL;

	// initialize semaphore members
	sem->count = count;
	sem->waiting_queue = queue_create();

	return sem;
}

int sem_destroy(sem_t sem)
{
	// Check error conditions
	if (sem == NULL || sem->count == 0) {
		return -1;
	}

	free(sem);

	return 0;
}

int sem_down(sem_t sem)
{
	// Check error conditions
	if (sem == NULL)
		return -1;

	// Decrease count or block thread
	while(1) {
		if (sem->count > 0) {
			sem->count--;

			return 0;
		} else {
			// Preserve in semaphore's waiting queue
			queue_enqueue(sem->waiting_queue, uthread_current());
			uthread_block();
		}
	}
}

int sem_up(sem_t sem)
{
	// Check error conditions
	if (sem == NULL)
		return -1;

	struct uthread_tcb *current_tcb = malloc(sizeof(struct uthread_tcb));
	current_tcb->context = malloc(sizeof(uthread_ctx_t));
	current_tcb->stack_ptr = NULL;

	// Unblock a waiting thread if there is one
	if (queue_length(sem->waiting_queue) > 0) {
		queue_dequeue(sem->waiting_queue, (void*)&current_tcb);
		uthread_unblock(current_tcb);
	}
 
	sem->count++; // Add to semaphore count

	return 0;
}



