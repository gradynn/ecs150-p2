#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

#include "queue.h"
#include "sem.h"
#include "private.h"

struct semaphore {
	/* TODO Phase 3 */
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
	enum stat status; // 0 = ready, 1 = zombie
	void* stack_ptr;
	uthread_ctx_t *context;
};

sem_t sem_create(size_t count)
{
	/* TODO Phase 3 */
	sem_t sem = malloc(sizeof(struct semaphore));
	sem->count = count;

	sem->waiting_queue = queue_create();

	return sem;
}

int sem_destroy(sem_t sem)
{
	/* TODO Phase 3 */
	if (sem == NULL || sem->count == 0) {
		return -1;
	}

	free(sem);

	return 0;
}

int sem_down(sem_t sem)
{
	/* TODO Phase 3 */
	if (sem == NULL)
		return -1;

	while(1) {
		if (sem->count > 0) {
			sem->count--;
			return 0;
		} else {
			uthread_block();
		}
	}
}

int sem_up(sem_t sem)
{
	printf("sem_up\n");
	/* TODO Phase 3 */
	if (sem == NULL)
		return -1;

	printf("Made it here 0\n");

	struct uthread_tcb *current_tcb = malloc(sizeof(struct uthread_tcb));
	current_tcb->context = malloc(sizeof(uthread_ctx_t));
	current_tcb->stack_ptr = NULL;

	printf("Made it here 1\n");

	queue_dequeue(sem->waiting_queue, (void*)&current_tcb);

	printf("Made it here 2\n");

	uthread_unblock(current_tcb);

	printf("Made it here 3\n");
 
	sem->count++;

	return 0;
}



