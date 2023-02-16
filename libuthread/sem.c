#include <stddef.h>
#include <stdlib.h>

#include "queue.h"
#include "sem.h"
#include "private.h"

queue_t waiting_queue;

struct semaphore {
	/* TODO Phase 3 */
	int count;
};

sem_t sem_create(size_t count)
{
	/* TODO Phase 3 */
	sem_t sem = malloc(sizeof(struct semaphore));
	sem->count = count;

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
	/* TODO Phase 3 */
	if (sem == NULL)
		return -1;

	struct uthread_tcb *current_tcb;

	queue_dequeue(waiting_queue, (void*)&current_tcb);
	uthread_unblock(current_tcb);
 
	sem->count++;
}



