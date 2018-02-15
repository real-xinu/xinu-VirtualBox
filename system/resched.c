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
	struct deferent *dfrptr;	/* Ptr to defer entry for this core		*/
	struct cpuent *cpuptr;		/* Ptr to cpu entry						*/

	cpuptr = &cputab[getcid()];
	dfrptr = &cpuptr->defer;

	/* If rescheduling is deferred, record attempt and return */

	if(getcid()){kprintf("cpu %d in resched\n", getcid());}
	if (dfrptr->ndefers > 0) {
		dfrptr->attempt = TRUE;
		if(getcid()){kprintf("resched deffered, returning\n");}
		return;
	}
	if(getcid()){kprintf("cpu %d resched not deferred\n", getcid());}
	if(getcid()){kprintf("currpid = %d\n", currpid);}

	/* Point to process table entry for the current (old) process */
	ptold = &proctab[cpuptr->cpid];

	lock(readylock);
	if(getcid()){kprintf("locked readylock\n");}
	lock(ptold->prlock);
	if(getcid()){kprintf("locked old prlock\n");}

	if (ptold->prstate == PR_CURR) {  /* Process remains eligible */
		if (ptold->prprio > firstkey(readylist)) {
			unlock(ptold->prlock);
			unlock(readylock);
			if(getcid()){kprintf("ptold->prprio < firstkey(readylist) (%d < %d), returning\n", ptold->prprio, firstkey(readylist));}
			return;
		}

		/* Old process will no longer remain current */

		ptold->prstate = PR_READY;
		insert(cpuptr->cpid, readylist, ptold->prprio);
	}

	/* Force context switch to highest priority ready process */
	cpuptr->ppid = cpuptr->cpid;		/* record previous process		*/
	cpuptr->cpid = dequeue(readylist);	/* get and record new process	*/
	ptnew = &proctab[cpuptr->cpid];		
	lock(ptnew->prlock);
	ptnew->prstate = PR_CURR;			/* set new process as current	*/
	cpuptr->preempt = QUANTUM;			/* Reset time slice for process	*/

	ctxsw(&ptold->prstkptr, &ptnew->prstkptr);

	/* Old process returns here when resumed */

	/* update pointers on new process stack */			
	cpuptr = &cputab[getcid()];
	ptnew = &proctab[cpuptr->cpid];
	ptold = &proctab[cpuptr->ppid];

	/* record where processes are running in their table entries	*/
	ptnew->prcpu = getcid();			
	ptold->prcpu = CPU_NONE;

	/* handle dying process	*/
	if (ptold->prstate == PR_DEAD){
		freestk(ptold->prstkbase, ptold->prstklen);
		ptold->prstate = PR_FREE;
	}

	/* unlock locks locked by previous process still held by this cpu */
	unlock(ptnew->prlock);
	unlock(ptold->prlock);
	unlock(readylock);


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
