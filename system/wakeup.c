/* wakeup.c - wakeup */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  wakeup  -  Called by clock interrupt handler to awaken processes
 *------------------------------------------------------------------------
 */
void	wakeup(void)
{
	intmask mask;	/* saved interrupt mask */

	/* Awaken all processes that have no more time to sleep */

	/* 	We are adding multiple processes to ready queue, so temporarily 
	*   stop other cores from rescheduling until all processes are added 
	*	by locking the readylock.
	*/ 
	mask = xsec_begn(2, sleepqlock, readylock);
	while (nonempty(sleepq) && (firstkey(sleepq) <= 0)) {
		ready(dequeue(sleepq));
	}
	xsec_endn(mask, 2, sleepqlock, readylock);

	return;
}
