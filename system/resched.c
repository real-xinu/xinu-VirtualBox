/* resched.c - resched, resched_cntl */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  resched  -  Reschedule processor to highest priority eligible process
 *------------------------------------------------------------------------
 */
void	resched(void)		/* Assumes interrupts are disabled	*/
{
	struct procent *ptold;		/* Ptr to table entry for old process	*/
	struct procent *ptnew;		/* Ptr to table entry for new process	*/
	// struct deferent *dfrptr;	/* Ptr to defer entry for this core		*/
	struct cpuent *cpuptr;		/* Ptr to cpu entry						*/
	cpuptr = &cputab[getcid()];

	kprintf("process %d enter resched()\n", getpid());
	// dfrptr = &cpuptr->defer;

	/* If rescheduling is deferred, record attempt and return */

	if (Defer.ndefers > 0) {
		Defer.attempt = TRUE;
		kprintf("process %d resched() deferred, return\n", getpid());
		return;
	}

	/* Point to process table entry for the current (old) process */
	ptold = &proctab[currpid];

	lock(readylock);
	lock(ptold->prlock);

	if (ptold->prstate == PR_CURR) {  /* Process remains eligible */
		if (ptold->prprio > firstkey(readylist)) {
			unlock(ptold->prlock);
			unlock(readylock);
			kprintf("process %d still top priority, return\n", getpid());
			return;
		}

		/* Old process will no longer remain current */

		ptold->prstate = PR_READY;
		insert(currpid, readylist, ptold->prprio);
	}

	/* Force context switch to highest priority ready process */
	prevpid = currpid;				/* record previous process		*/
	currpid = dequeue(readylist);	/* get and record new process	*/
	ptnew = &proctab[currpid];		
	lock(ptnew->prlock);
	ptnew->prstate = PR_CURR;		/* set new process as current	*/
	cpuptr->preempt = QUANTUM;		/* Reset time slice for process	*/

	/* hand over locks to new process before ctxsw */
	locktab[ptnew->prlock].lowner = currpid;
	locktab[ptold->prlock].lowner = currpid;
	locktab[readylock].lowner = currpid;

	/* record where processes are running in their table entries	*/
	ptnew->prcpu = getcid();
	ptold->prcpu = CPU_NONE;

	kprintf("process %d, core %d: enter ctxsw\n", cpuptr->ppid, getcid());
	ctxsw(&ptold->prstkptr, &ptnew->prstkptr);

	kprintf("process %d, core %d: exit ctxsw\n", getpid(), getcid());
	/* Old process returns here when resumed */
	ctxsw_ret();

	kprintf("process %d done switching, return\n", getpid());
	return;
}

/*------------------------------------------------------------------------
 *  resched_cntl  -  Control whether rescheduling is deferred or allowed
 *------------------------------------------------------------------------
 */
status	resched_cntl(		/* Assumes interrupts are disabled	*/
	  int32	defer		/* Either DEFER_START or DEFER_STOP	*/
	)
{
	struct deferent *dfrptr;	/* Ptr to defer entry for this core		*/

	dfrptr = &cputab[getcid()].defer;

	switch (defer) {

	    case DEFER_START:	/* Handle a deferral request */

		if (dfrptr->ndefers++ == 0) {
			dfrptr->attempt = FALSE;
		}
		return OK;

	    case DEFER_STOP:	/* Handle end of deferral */
		if (dfrptr->ndefers <= 0) {
			return SYSERR;
		}
		if ( (--dfrptr->ndefers == 0) && dfrptr->attempt ) {
			resched();
		}
		return OK;

	    default:
		return SYSERR;
	}
}
