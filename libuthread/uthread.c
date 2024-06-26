#include <assert.h>
#include <signal.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#include "preempt.c"
#include "private.h"
#include "queue.h"
#include "uthread.h"

#define UNUSED(x) (void)(x)

// Representation of a threads status
enum stat {
	READY,
	RUNNING,
	BLOCKED,
	ZOMBIE
};

queue_t ready_queue;
queue_t blocked_queue;
queue_t destroy_queue;

struct uthread_tcb {
	enum stat status;
	void* stack_ptr;
	uthread_ctx_t *context;
};

struct uthread_tcb *current_tcb;

struct uthread_tcb *uthread_current(void)
{
	return current_tcb;
}

void uthread_yield(void)
{
    // Create new space to save a copy of the current tcb
    struct uthread_tcb *save_tcb = malloc(sizeof(struct uthread_tcb));
	save_tcb->context = malloc(sizeof(uthread_ctx_t));

	// Copy current thread info to save tcb
	memcpy(save_tcb, current_tcb, sizeof(struct uthread_tcb));

    // Add current tcb to the back of the queue to be resumed later
    queue_enqueue(ready_queue, save_tcb);

	// Obtain the head of the queue
    do {
        queue_dequeue(ready_queue, (void*)&current_tcb);
    } while (current_tcb->status != READY); // eunsures that only valid processes are dequeued

    // Switch to the next thread
    uthread_ctx_switch(save_tcb->context, current_tcb->context);
}

void uthread_exit(void)
{
	// Reassign current tcb's status to zombie
    current_tcb->status = ZOMBIE;

	// holder to pass to ctx_switch
	struct uthread_tcb *junk_tcb = malloc(sizeof(struct uthread_tcb));
	junk_tcb->context = malloc(sizeof(uthread_ctx_t));

	queue_dequeue(ready_queue, (void*)&current_tcb);
	uthread_ctx_switch(junk_tcb->context, current_tcb->context);
}

int uthread_create(uthread_func_t func, void *arg)
{
	// Dynamically allocate space for a new thread
	struct uthread_tcb *new_tcb = malloc(sizeof(struct uthread_tcb));

	if (new_tcb == NULL)
		return -1;

	new_tcb->context = malloc(sizeof(uthread_ctx_t));

	// Initialize new thread
	new_tcb->stack_ptr = uthread_ctx_alloc_stack();
	uthread_ctx_init(new_tcb->context, new_tcb->stack_ptr, func, arg);
	new_tcb->status = READY;

	// Enqueue new thread in ready queue
	queue_enqueue(destroy_queue, new_tcb);
    return(queue_enqueue(ready_queue, new_tcb));
}

int uthread_run(bool preempt, uthread_func_t func, void *arg)
{
	preempt_start(preempt);

	// Initialize all global vars
    ready_queue = queue_create(); // queue of ready threads
	blocked_queue = queue_create(); // queue of blocked threads
	destroy_queue = queue_create(); // queue of threads to be destroyed

	if (ready_queue == NULL || blocked_queue == NULL || destroy_queue == NULL)
		return -1;

    current_tcb = malloc(sizeof(struct uthread_tcb)); // current thread

	if(current_tcb == NULL)
		return -1;

    current_tcb->context = malloc(sizeof(uthread_ctx_t));
    current_tcb->stack_ptr = NULL;

    // Create the initial thread
    if (uthread_create(func, arg) == -1)
		return -1;

	while (1) {
        if (queue_length(ready_queue) == 0) {
            break;
        }

        // Yield idle thread control
        uthread_yield();
	}

	preempt_stop();

	// Free up memory of created threads
	while (1) {
		if (queue_length(destroy_queue) == 0) {
			break;
		}

		queue_dequeue(destroy_queue, (void*)&current_tcb);
		uthread_ctx_destroy_stack(current_tcb->stack_ptr);
		free(current_tcb->context);
		free(current_tcb);
	}

	if (queue_destroy(ready_queue) != 0)
		return -1;

	if (queue_destroy(destroy_queue) != 0)
		return -1;

	if (queue_destroy(blocked_queue) != 0)
		return -1;

	return 0;
}

void uthread_block(void)
{
	// Create new space to save a copy of the current tcb
    struct uthread_tcb *save_tcb = malloc(sizeof(struct uthread_tcb));
	save_tcb->context = malloc(sizeof(uthread_ctx_t));

	// Copy current thread info to save tcb
	memcpy(save_tcb, current_tcb, sizeof(struct uthread_tcb));
    
    // Add current tcb to the blocked queue
	save_tcb->status = BLOCKED;
    queue_enqueue(blocked_queue, save_tcb);

	// Obtain the head of the queue
    do {
        queue_dequeue(ready_queue, (void*)&current_tcb);
    } while (current_tcb->status != READY); // limit to only valid processes

    // Switch to the next thread
    uthread_ctx_switch(save_tcb->context, current_tcb->context);
}


void uthread_unblock(struct uthread_tcb *uthread)
{
	// Remove from blocked queue
	queue_delete(blocked_queue, uthread);

	// Enqueue in ready queue
	uthread->status = READY;
	queue_enqueue(ready_queue, uthread);
}
