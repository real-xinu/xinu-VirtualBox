/* semdelete.c - semdelete */

#include <xinu.h>

/*------------------------------------------------------------------------
 * semdelete  -  Delete a semaphore by releasing its table entry
 *------------------------------------------------------------------------
 */
syscall	semdelete(
	  sid32		sem		/* ID of semaphore to delete	*/
	)
{
	intmask mask;			/* Saved interrupt mask		*/
	struct	sentry *semptr;		/* Ptr to semaphore table entry	*/

	if (isbadsem(sem)) {
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

	semptr->sstate = S_FREE;

	while (semptr->scount++ < 0) {	/* Free all waiting processes	*/
		ready(getfirst(semptr->squeue));
	}

	xsec_endn(mask, 2, semptr->slock, readylock);
	return OK;
}
