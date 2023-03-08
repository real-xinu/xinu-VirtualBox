/* ready.c - ready */

#include <xinu.h>

qid16	readylist;			/* Index of ready list		*/
lid32	readylock;			/* Index of ready list spinlock	*/

/*------------------------------------------------------------------------
 *  ready  -  Make a process eligible for CPU service
 *------------------------------------------------------------------------
 */
status	ready(
	  pid32		pid		/* ID of process to make ready	*/
	)
{
	register struct procent *prptr;
	intmask mask;	

	if (isbadpid(pid)) {
		return SYSERR;
	}
	prptr = &proctab[pid];

	mask = xsec_begn(2, readylock, prptr->prlock);

	if (prptr->prstate == PR_FREE || prptr->prstate == PR_DEAD){
		xsec_endn(mask, 2, readylock, prptr->prlock);
		return SYSERR;
	}

	/* Set process state to indicate ready and add to ready list */

	prptr->prstate = PR_READY;
	insert(pid, readylist, prptr->prprio);

	/* Enforce scheduling invariant	*/
	cid32 new_core = get_resched_core(pid);
	if (new_core != CPU_NONE) {
		sendipi(IPI_RESCHED, new_core);
	}
	//We no longer broadcast to all CPUS - just send to the one with the lowest priority process (if any is available) or do nothing (which will happen if all CPUs are executing processes with higher priorities than this one)

	xsec_endn(mask, 2, readylock, prptr->prlock);
	return OK;
}
