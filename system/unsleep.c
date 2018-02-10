/* unsleep.c - unsleep */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  unsleep  -  Internal function to remove a process from the sleep
 *		    queue prematurely.  The caller must adjust the delay
 *		    of successive processes.
 *------------------------------------------------------------------------
 */
status	unsleep(
	  pid32		pid		/* ID of process to remove	*/
        )
{
	intmask	mask;			/* Saved interrupt mask		*/
    struct	procent	*prptr;		/* Ptr to process's table entry	*/

    pid32	pidnext;		/* ID of process on sleep queue	*/
					/*   that follows the process	*/
					/*   which is being removed	*/

	if (isbadpid(pid)) {
		return SYSERR;
	}
	prptr = &proctab[pid];

	mask = xsec_begn(2, sleepqlock, prptr->prlock);

	/* Verify that candidate process is on the sleep queue */

	if ((prptr->prstate!=PR_SLEEP) && (prptr->prstate!=PR_RECTIM)) {
		xsec_endn(mask, 2, sleepqlock, prptr->prlock);
		return SYSERR;
	}

	/* Increment delay of next process if such a process exists */

	pidnext = queuetab[pid].qnext;
	if (pidnext < NPROC) {
		queuetab[pidnext].qkey += queuetab[pid].qkey;
	}

	getitem(pid);			/* Unlink process from queue */

	xsec_endn(mask, 2, sleepqlock, prptr->prlock);
	return OK;
}
