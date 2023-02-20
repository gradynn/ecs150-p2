#include <signal.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "private.h"
#include "uthread.h"

/*
 * Frequency of preemption
 * 100Hz is 100 times per second
 */
#define HZ 100

struct sigaction sa2;

void signal_handler(int signum)
{
	if (signum) 
		// do nothing

	uthread_yield();
}

void preempt_disable(void)
{
	/* Block signal SIGALRM */
	sigset_t mask;
	sigemptyset(&mask);
	sigaddset(&mask, SIGVTALRM);

	sigprocmask(SIG_BLOCK, &mask, NULL);
}

void preempt_enable(void)
{
	/* Unblock signal SIGALRM */
	sigset_t mask;
	sigemptyset(&mask);
	sigaddset(&mask, SIGVTALRM);

	sigprocmask(SIG_UNBLOCK, &mask, NULL);
}

void preempt_start(bool preempt)
{
	if (preempt == true) {
		/* Set up signal handler*/
		struct sigaction sa;
		sa.sa_handler = signal_handler;
		sigemptyset(&sa.sa_mask);
		sa.sa_flags = 0;

		/* Set up sig action to hold old config */
		sa2.sa_handler = NULL;
		sigemptyset(&sa2.sa_mask);
		sa2.sa_flags = 0;

		sigaction(SIGVTALRM, &sa, &sa2);
		
		/* Alarm */
		struct itimerval timer;
		timer.it_value.tv_sec = 1 / HZ;
		setitimer(ITIMER_VIRTUAL, &timer, 0);
	}
}

void preempt_stop(void)
{
	/* Restore timer configuration (no timer) */
	setitimer(ITIMER_VIRTUAL, 0, 0);

	/* Restore signal handler */
	sigaction(SIGVTALRM, &sa2, NULL);
}

