/* signaln.c - signaln */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  signaln  -  Signal a semaphore n times, releasing n waiting processes
 *------------------------------------------------------------------------
 */
syscall	signaln(
	  sid32		sem,		/* ID of semaphore to signal	*/
	  int32		count		/* Number of times to signal	*/
	)
{
	intmask	mask;			/* Saved interrupt mask		*/
	struct	sentry	*semptr;	/* Ptr to sempahore table entry */

	if (isbadsem(sem) || (count < 0)) {
		return SYSERR;
	}
	semptr = &semtab[sem];

	/* 	We are adding multiple processes to ready queue, so temporarily 
	*   stop other cores from rescheduling until all processes are added 
	*	by locking the readylock.
	*/ 	
	mask = xsec_begn(2, semptr->slock, readylock); 

	if (semptr->sstate == S_FREE) {
		xsec_endn(mask, 2, semptr->slock, readylock);
		return SYSERR;
	}

	for (; count > 0; count--) {
		if ((semptr->scount++) < 0) {
			ready(dequeue(semptr->squeue));
		}
	}

	xsec_endn(mask, 2, semptr->slock, readylock);
	return OK;
}
