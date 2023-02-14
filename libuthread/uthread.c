#include <assert.h>
#include <signal.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <string.h>

#include "private.h"
#include "uthread.h"
#include "queue.h"

#define UNUSED(x) (void)(x)

queue_t ready_queue;
queue_t destroy_queue;

struct uthread_tcb {
	/* TODO Phase 2 */
	int status; // 0 = ready, 1 = zombie
	void* stack_ptr;
	uthread_ctx_t *context;
};

struct uthread_tcb *current_tcb;

struct uthread_tcb *uthread_current(void)
{
	/* TODO Phase 2/3 */
	return current_tcb;
}

void uthread_yield(void)
{
    /* Create new space to save a copy of the current tcb */
    struct uthread_tcb *save_tcb = malloc(sizeof(struct uthread_tcb));
	save_tcb->context = malloc(sizeof(uthread_ctx_t));

	/* Copy current thread info to save tcb */
	memcpy(save_tcb, current_tcb, sizeof(struct uthread_tcb));
    
    /* Add current tcb to the back of the queue to be resumed later */
    queue_enqueue(ready_queue, save_tcb);

	/* Obtain the head of the queue */
    do {
        queue_dequeue(ready_queue, (void*)&current_tcb);
    } while (current_tcb->status != 0); // eunsures that only valid processes are dequeued

    /* Switch to the next thread */
    uthread_ctx_switch(save_tcb->context, current_tcb->context);
}

void uthread_exit(void)
{
	/* Reassign current tcb's status to zombie */
    current_tcb->status = 1;

	/* holder to pass to ctx_switch */
	struct uthread_tcb *junk_tcb = malloc(sizeof(struct uthread_tcb));
	junk_tcb->context = malloc(sizeof(uthread_ctx_t));

	queue_dequeue(ready_queue, (void*)&current_tcb);
	uthread_ctx_switch(junk_tcb->context, current_tcb->context);
}

int uthread_create(uthread_func_t func, void *arg)
{
	/* Dynamically allocate space for a new thread */
	struct uthread_tcb *new_tcb = malloc(sizeof(struct uthread_tcb));
	new_tcb->context = malloc(sizeof(uthread_ctx_t));

	/* Initialize new thread */
	new_tcb->stack_ptr = uthread_ctx_alloc_stack();
	uthread_ctx_init(new_tcb->context, new_tcb->stack_ptr, func, arg);
	new_tcb->status = 0;

	/* Enqueue new thread in ready queue */
	queue_enqueue(destroy_queue, new_tcb);
    return(queue_enqueue(ready_queue, new_tcb));
}

int uthread_run(bool preempt, uthread_func_t func, void *arg)
{

	if (preempt) {
		/* TODO Phase 3 */
	}

	/* Initialize all global vars */
    ready_queue = queue_create(); // queue of ready threads

    current_tcb = malloc(sizeof(struct uthread_tcb)); // current thread
    current_tcb->context = malloc(sizeof(uthread_ctx_t));
    current_tcb->stack_ptr = NULL;

    /* Create the initial thread */
    uthread_create(func, arg);

	while (1) {
        if (queue_length(ready_queue) == 0) {
            break;
        }

        /* Yield idle thread control */
        uthread_yield();
	}

	/* Free up memory of created threads */
	while (1) {
		if (queue_length(destroy_queue) == 0) {
			break;
		}

		queue_dequeue(destroy_queue, (void*)&current_tcb);
		uthread_ctx_destroy_stack(current_tcb->stack_ptr);
		free(current_tcb->context);
		free(current_tcb);
	}

	queue_destroy(ready_queue);
	queue_destroy(destroy_queue);
	
	return 0;
}

void uthread_block(void)
{
	/* TODO Phase 3 */

}

/*
void uthread_unblock(struct uthread_tcb *uthread)
{
	
	
}
*/