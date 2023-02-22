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

bool PREEMPT = true;

/*
 * Signal handler for SIGALRM
 */
void signal_handler(int signum)
{
	(void)signum;
	uthread_yield();
}

void preempt_disable(void)
{
	if (PREEMPT) {
		// Build mask that excludes SIGVTALRM
		sigset_t mask;
		sigemptyset(&mask);
		sigaddset(&mask, SIGVTALRM);

		sigprocmask(SIG_BLOCK, &mask, NULL);
	}
}

void preempt_enable(void)
{
	if (PREEMPT){
		// Build mask that includes SIGVTALRM
		sigset_t mask;
		sigemptyset(&mask);
		sigaddset(&mask, SIGVTALRM);

		// Unblock SIGVTALRM
		sigprocmask(SIG_UNBLOCK, &mask, NULL);
	}
}

void preempt_start(bool preempt)
{
	if (!preempt) {
		PREEMPT = false;
		return;
	}

	// Set up and link signal handler
	struct sigaction sa;
	sa.sa_handler = signal_handler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sigaction(SIGVTALRM, &sa, NULL);
	
	// Alarm set up
	struct itimerval timer;
	timer.it_value.tv_sec = 1 / HZ;
  	timer.it_value.tv_usec = (HZ * 100) % 1000000;
	timer.it_interval = timer.it_value;
	setitimer(ITIMER_VIRTUAL, &timer, NULL);
}

void preempt_stop(void)
{
	if (PREEMPT) {
		// Restore timer configuration (no timer)
		setitimer(ITIMER_VIRTUAL, 0, 0);
	}
}

