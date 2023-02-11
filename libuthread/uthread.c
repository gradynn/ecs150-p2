#include <assert.h>
#include <signal.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "private.h"
#include "uthread.h"
#include "queue.h"

queue_t ready_queue;
queue_t blocked_queue;

struct uthread_tcb {
	/* TODO Phase 2 */
	uthread_ctx_t *context;
};

struct uthread_tcb *current_thread;
struct uthread_tcb *idle_thread;

struct uthread_tcb *uthread_current(void)
{
	/* TODO Phase 2/3 */
	return current_thread;
}

void uthread_yield(void)
{
	/* TODO Phase 2 */
	queue_enqueue(ready_queue, current_thread);
	uthread_ctx_switch(current_thread->context, idle_thread->context);
}

void uthread_exit(void)
{
	/* TODO Phase 2 */
	uthread_ctx_destroy_stack(uthread_current()->context->uc_stack.ss_sp);
	free(uthread_current());
}

int uthread_create(uthread_func_t func, void *arg)
{
	/* TODO Phase 2 */
	struct uthread_tcb *new_thread = malloc(sizeof(struct uthread_tcb));
	uthread_ctx_init(new_thread->context, uthread_ctx_alloc_stack(), func, arg);
	queue_enqueue(ready_queue, new_thread);

	return 0;
}

int uthread_run(bool preempt, uthread_func_t func, void *arg)
{
	if (preempt) {
		/* TODO Phase 3 */
	}

	/* Initialize the globals */
	ready_queue = queue_create();
	blocked_queue = queue_create();
	current_thread = malloc(sizeof(struct uthread_tcb));

	/* Register execution flow of the application as the idle thread */
	idle_thread = malloc(sizeof(struct uthread_tcb));

	// Created a new (initiial) thread based on arguments
	struct uthread_tcb *initial_thread = malloc(sizeof(struct uthread_tcb));
	uthread_ctx_init(initial_thread->context, 
						uthread_ctx_alloc_stack(), 
						func, 
						arg);
	printf("Made it here 1.\n");
	queue_enqueue(ready_queue, initial_thread);

	printf("Made it here 2.\n");
	// Infinite loop to run the scheduler
	while (1) {
		// If the ready queue is empty, then we are done
		if (queue_length(ready_queue) == 0) {
			break;
		}

		// Get the next thread from the ready queue
		printf("Made it here n.\n");
		queue_dequeue(ready_queue, (void*)&current_thread);
		uthread_ctx_switch(idle_thread->context, current_thread->context);
	}

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
