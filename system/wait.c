/* wait.c - wait */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  wait  -  Cause current process to wait on a semaphore
 *------------------------------------------------------------------------
 */
syscall	wait(
	  sid32		sem		/* Semaphore on which to wait  */
	)
{
	intmask mask;				/* Saved interrupt mask			*/
	struct	procent *prptr;		/* Ptr to process's table entry	*/
	struct	sentry *semptr;		/* Ptr to sempahore table entry	*/

	if (isbadsem(sem)) {
		return SYSERR;
	}
	semptr = &semtab[sem];
	prptr = &proctab[currpid];

	mask = xsec_begn(2, semptr->slock, prptr->prlock);

	if (semptr->sstate == S_FREE || prptr->prstate != PR_CURR) {
		xsec_endn(mask, 2, semptr->slock, prptr->prlock);
		return SYSERR;
	}

	if (--(semptr->scount) < 0) {		/* If caller must block	*/
		prptr->prstate = PR_WAIT;		/* Set state to waiting	*/
		prptr->prsem = sem;				/* Record semaphore ID	*/
		enqueue(currpid,semptr->squeue);/* Enqueue on semaphore	*/
		resched();						/*   and reschedule	*/
	}

	xsec_endn(mask, 2, semptr->slock, prptr->prlock);
	return OK;
}

