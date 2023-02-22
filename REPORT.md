# ECS 150 - Project 2

## February 21, 2023

## Prof. Porquet-Lupine

## Authors: Gradyn Nagle, Chase Keighley

This report covers the implemenation of the following four features. These 
features are compiled in the library `libuthread.a`.

## Table of Contents

1. [Makefile](#makefile)
2. [Queue](#queue)
3. [uthread](#uthread)
4. [Semaphores](#semaphores)
5. [Preemption](#preemption)
6. [Sources](#sources)

## Makefile

The makefile we created compiles the source code into object files which are
compiled into the library `libuthread.a`. A generalized rule creates most
objects from their source and header files. Two objects, `preempt.o` and
`context.o`, require separate rules as the source and header have different
names. A conditional statement allows for the -g flag for debugging. 

## Queue

The file `queue.c` contains the implementation of the queue. It is implemented
through a linked list. This consists of two structures: `node`, which contains a
data member and a pointer to the next node, and `queue`, which contains pointers
to the head and tail nodes, as well as the length of the queue. Most queue
functions contain a conditional statement that handles the error management if
the argument(s) it has been provided are null or otherwise invalid, causing the
function to return -1. The following functions are defined in `queue.c`:

### `queue_create()`

This function simply dynamically allocates memory for a new queue, initializes
its length to be 0, and returns the newly created queue.

### `queue_destroy()`

This function frees the space allocated to the queue passed as its argument,
returning 0 if successful. This

### `queue_enqueue()`

This function allocates a new node containing the data specified by the argument
`data` and enqueues it in `queue`. If the queue's length is 0, it makes the
queue's head and tail members the new node. Otherwise, the tail of the linked
list becomes new node and the previous tail becomes its next node. The queue's
length is increased. The linked list implementation allows this function to
maintain O(1) time complexity, as it only has to reassign the tail pointer when
adding data to the queue.

### `queue_dequeue()`

This function dequeues data from the argument `queue` into argument `data`.
`temp` is allocated to hold the address of the queue's head node. The head node
is then reassigned to be its own next node (the next node in the queue), and
`temp` is deallocated. The length of the queue is decremented by 1.

### `queue_delete()`

This function uses a loop and two temporary nodes `current` and `prev` to
iterate through the queue's linked list. If the current node's data equals the
`data` argument, the previous node's `next` becomes the current node's `next`,
or (in the case of deletion at the queue's start) the head's `next` becomes the
current node's `next`. This essentially relinks the gap in the queue made by
deleting the current node. The length of the queue is decremented by 1.

### `queue_iterate()`

This function uses a loop and a temporary node `current` similar to
queue_delete(), however in each iteration of the loop, it calls a function
specified by `func` that takes as arguments the given queue and the current
node's data. Returns 0 on success.

### `queue_length()`

This function returns the given queue's length.

#### Queue Testing

In order to test our queue implementation we wrote multiple functions in the 
`queue_tester.c` file. These functions test the functionality of the queue
individually and user `ASSERT()` to check if the functions are working as
intended. Each function is called in the `main()` function.

### uthread

The file `uthread.c` contains the implementation of functions realting to the
user level threading library as well as a structure representing a thread
control block. We use three queues, declared globally, to keep track of all open
threads. The `ready_queue` contains all threads that are ready to be executed,
the `blocked_queue` contains all threads that are blocked on a semaphore, and
the `destroy_queue` contains each thread that needs to be destroyed. It contains 
the following four functions:

### `uthread_current()`

This function returns a pointer to the thread control block  of the thread that 
is currently being executed. We stored this pointer as a global variable and
updated it whenever a context switch occurs in other functions.

### `uthread_yield()`

This function is used to yield the CPU to another thread. It is first allocates
a new space on the heap for to save a pointer to the current thread, before
calling `swapcontext` to switch to the next thread in the queue. It then
enqueues this pointer in the `ready_queue`. The function then dequeues the next
thread in the `ready_queue` and calls `uthread_ctx_switch` to switch to that 
thread.

### `uthread_exit()`

This function handles the exiting of a thread at the end of its execution. It
is similar to `uthread_yield` in that it dequeues the next thread in the
`ready_queue` and calls `uthread_ctx_switch` to switch to that thread. However, 
it, does not reenqueue the current thread in the `ready_queue`. Instead, it 
changes its status to `ZOMBIE`.

### `uthread_create()`

This function handles the creation of a new thread. It first allocates a new
space on the heap for the thread control block. After that it uses
helper functions `uthread_ctx_alloc_stack()` and `uthread_ctx_init()` to
initialize the stack and context of the thread. It then enqueues the thread in
the `ready_queue` and the `destroy_queue` (so we can deallocate it later).

### `uthread_run()`

This is the main function of the library and is called by the user to start the
execution of threads. It first passes boolean argument `preempt` to preempt 
start to set the users specified preemption mode. It the initializes all three
queues and `current_thread`. It then calls `uthread_create` to create the
initial thread, passing the users specified function `func` and argument `arg`.
A while loop continuously checks if the `ready_queue` is empty. If it is not,
then `uthread_yield` is called to switch to the next thread in the queue. If it
is empty, then the loop breaks and the program exits. Another while loop
moves through the `destroy_queue` and deallocates each thread in the queue.
Finally, the three queues are deallocated.

### `uthread_block()`

This function is called when a thread needs to be blocked. It first allocates 
new space in memory to save the current thread to and copies the current thread 
to it using `memcpy()`. It then changes the status of the current thread to 
`BLOCKED` and enqueues it in the `blocked_queue`. It then dequeues the next 
thread in the `ready_queue` and calls `uthread_ctx_switch` to switch to that 
thread. The dequeue call is contained within a do-while loop to ensure that the 
next thread in the queue has status the correct status.

### `uthread_unblock()`

This function handles the unblocking of a thread. It deletes the specified
thread, passed as an argument by the user, from the `blocked_queue` and changes
its status to `READY`. It then enqueues the thread in the `ready_queue`.

### uthread Testing

To test uthread we utilized Professor Porque-Lupine's provided test cases.

## Semaphores

Our semaphore implementation in `sem.c` uses the structure `semaphore` which
contains an integer `count` and queue pointer `waiting_queue`. The structure
`uthread_tcb` is redefined in `sem.c` so it can be utilized within the semaphore
functions. Most functions contain a conditional statement that handles the error
management if the argument(s) it has been provided are null or otherwise
invalid, causing the function to return -1. The functions are as follows:

### `sem_create()`

This function allocates space for a new semaphore, initializes its `count` given
by the function argument, and initializes its waiting queue using
`queue_create`. It returns the newly created semaphore.

### `sem_destroy()`

This function frees memory allocated for the given semaphore and returns 0 on
success.

### `sem_down`

This function utilizes an infinite while loop to continually enqueue the current
thread into the waiting queue and block it using `uthread_block` if a
semaphore's resources are not available. If `count` is not 0, it is decremented
by 1 and the function returns 0.

### `sem_up`

This function dynamically allocates a `uthread_tcb` pointer called
`current_tcb`. If the semaphore's waiting queue is not empty, the next thread is
dequeued and assigned to `current_tcb`. The semahpore's `count` is incremented.

### Semaphore Testing

To test uthread we utilized Professor Porque-Lupine's provided test cases.

## Preemption

The file `preempt.c` contains the implementation of optional preemption used
from within the user level threading library. It contains the following five
functions:

### `preempt_start()`

`preempt_start()` is called by `uthread_run()` to set the preemption mode. It
is the main function in the library. It then binds the function
`signal_handler()` to the `SIGVTALRM` signal. `signal_handler` contains a single
call to `uthread_yield`. This ensures that, even if the thread itself does not 
yield control it will be forced to yield. It then sets a virtual timer to go
off *x* times a second, effectively yielding *x* times per second.

### `preempt_stop()`

This function stops all preemption by setting the virtual timer to 0.

### `preempt_disable()`

This function disables preemption (temporarily until the user enables it) by
blocking the `SIGVTALRM` signal. It accomplishes this using a mask.

### `preempt_enable()`

This function enables preemption (assuming it was previously disabled) by
unblocking the `SIGVTALRM` signal. It also accomplishes this using a mask.

### Preemption Testing

In order to test preemption, we wrote a simple C program that creates two 
threads. In the first thread, after creating the second thread, an infinite
while loop runs. Each iteration it prints out "thread1". The second thread is
the same but prints "thread2". We then call `uthread_run` with the argument
first true argument as `true` specifing preemption should be enabled. Without
preemption, we would expect only the first thread, which never invokes 
`uthread_yield()`, to hold control forever and print an endless stream of 
"thread1". However, with preemption enabled, we expect the second thread to
periodically interrupt the first thread and print "thread2".

## Sources

1. Porquet-Lupine, Joel. “System Calls.” ECS 150. University of California, 
    Davis, 2023. Lecture.
    + Consulted heavily for the signal implementation.

2. Porquet-Lupine, Joel. “Concurrency Threads.” ECS 150. University of 
    California, Davis, 2023. Lecture.
    + Consulted for thread implementation, conceptual.

3. Porquet-Lupine, Joel. “Synchronization.” ECS 150. University of California, 
    Davis, 2023. Lecture.
    + Consulted for semaphore implementation.

4. Porquet-Lupine, Joel. “System Calls.” ECS 150. University of California, 
    Davis, 2023. Lecture.
    + Consulted heavily for the signal implementation.

5. Kerrisk, Micheal. “man7.Org.” Michael Kerrisk, https://man7.org/index.html. 
    + Consulted for details of signals.

6. “Setitimer(2) - Linux Man Page.” Setitimer(2): Get/Set Value of Interval 
    Timer - Linux Man Page, https://linux.die.net/man/2/setitimer. 
    + Consulted for details of setitimer.
