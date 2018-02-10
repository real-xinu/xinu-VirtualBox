/* signal.c - signal */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  signal  -  Signal a semaphore, releasing a process if one is waiting
 *------------------------------------------------------------------------
 */
syscall	signal(
	  sid32		sem		/* ID of semaphore to signal	*/
	)
{
	intmask mask;			/* Saved interrupt mask		*/
	struct	sentry *semptr;		/* Ptr to sempahore table entry	*/

	if (isbadsem(sem)) {
		return SYSERR;
	}
	semptr= &semtab[sem];

	mask = xsec_beg(semptr->slock);

	if (semptr->sstate == S_FREE) {
		xsec_end(mask, semptr->slock);
		return SYSERR;
	}

	if ((semptr->scount++) < 0) {	/* Release a waiting process */
		ready(dequeue(semptr->squeue));
	}

	xsec_end(mask, semptr->slock);
	return OK;
}