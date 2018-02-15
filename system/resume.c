/* resume.c - resume */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  resume  -  Unsuspend a process, making it ready
 *------------------------------------------------------------------------
 */
pri16	resume(
	  pid32		pid		/* ID of process to unsuspend	*/
	)
{
	intmask	mask;			/* Saved interrupt mask		*/
	struct	procent *prptr;		/* Ptr to process's table entry	*/
	pri16	prio;			/* Priority to return		*/
	kprintf("enter resume\n");

	if (isbadpid(pid)) {
		return (pri16)SYSERR;
	}
	prptr = &proctab[pid];

	mask = xsec_begn(2, readylock, prptr->prlock);

	if (prptr->prstate != PR_SUSP) {
		xsec_endn(mask, 2, readylock, prptr->prlock);
		return (pri16)SYSERR;
	}

	prio = prptr->prprio;		/* Record priority to return	*/
	ready(pid);

	xsec_endn(mask, 2, readylock, prptr->prlock);
	kprintf("exit resume\n");
	return prio;
}
