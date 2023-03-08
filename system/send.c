/* send.c - send */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  send  -  Pass a message to a process and start recipient if waiting
 *------------------------------------------------------------------------
 */
syscall	send(
	  pid32		pid,		/* ID of recipient process	*/
	  umsg32	msg		/* Contents of message		*/
	)
{
	intmask	mask;			/* Saved interrupt mask		*/
	struct	procent *prptr;		/* Ptr to process's table entry	*/

	if (isbadpid(pid)) {
		return SYSERR;
	}
	prptr = &proctab[pid];

	mask = xsec_begn(3, sleepqlock, readylock, prptr->prlock);

	if (prptr->prstate == PR_FREE || prptr->prstate == PR_DEAD || prptr->prhasmsg) {
		xsec_endn(mask, 3, sleepqlock, readylock, prptr->prlock);
		return SYSERR;
	}

	prptr->prmsg = msg;		/* Deliver message		*/
	prptr->prhasmsg = TRUE;		/* Indicate message is waiting	*/

	/* Check to make sure receiver is no longer running on a core */
	while (prptr->prcpu != CPU_NONE);

	/* If recipient waiting or in timed-wait make it ready */

	if (prptr->prstate == PR_RECV) {
		ready(pid);
	} else if (prptr->prstate == PR_RECTIM) {
		unsleep(pid);
		ready(pid);
	}

	xsec_endn(mask, 3, sleepqlock, readylock, prptr->prlock);
	return OK;
}
