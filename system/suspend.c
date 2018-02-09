/* suspend.c - suspend */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  suspend  -  Suspend a process, placing it in hibernation
 *------------------------------------------------------------------------
 */
syscall suspend(
	pid32 pid /* ID of process to suspend	*/
)
{
	intmask mask;		   /* Saved interrupt mask		*/
	struct procent *prptr; /* Ptr to process's table entry	*/
	pri16 prio;			   /* Priority to return		*/

	if (isbadpid(pid) || isnullpid(pid)){
		return SYSERR;
	}
	prptr = &proctab[pid];

	mask = xsec_begn(2, readylock, prptr->prlock);

	/* Only suspend a process that is current or ready */
	if ((prptr->prstate != PR_CURR) && (prptr->prstate != PR_READY)){
		xsec_endn(mask, 2, readylock, prptr->prlock);
		return SYSERR;
	}

	if (prptr->prstate == PR_READY){ 
		getitem(pid); /* Remove a ready process	*/
					  /*   from the ready list	*/
		prptr->prstate = PR_SUSP;
	} else {
		prptr->prstate = PR_SUSP; /* Mark the current process	*/
		if(pid == currpid){ /* process currently on this core */
			resched();				  /*   suspended and resched.	*/
		} else { /* process on a different core */
			sendipi(IPI_RESCHED, prptr->prcpu);
		}
	}

	prio = prptr->prprio;

	xsec_endn(mask, 2, readylock, prptr->prlock);
	return prio;
}
