/* sleep.c - sleep sleepms */

#include <xinu.h>

#define	MAXSECONDS	2147483		/* Max seconds per 32-bit msec	*/

/*------------------------------------------------------------------------
 *  sleep  -  Delay the calling process n seconds
 *------------------------------------------------------------------------
 */
syscall	sleep(
	  int32	delay		/* Time to delay in seconds	*/
	)
{
	if ( (delay < 0) || (delay > MAXSECONDS) ) {
		return SYSERR;
	}
	sleepms(1000*delay);
	return OK;
}

/*------------------------------------------------------------------------
 *  sleepms  -  Delay the calling process n milliseconds
 *------------------------------------------------------------------------
 */
syscall	sleepms(
	  int32	delay				/* Time to delay in msec.	*/
	)
{
	intmask	mask;				/* Saved interrupt mask		*/
	struct	procent *prptr;		/* Ptr to process's table entry	*/


	if (delay < 0) {
		return SYSERR;
	}

	if (delay == 0) {
		yield();
		return OK;
	}

	prptr = &proctab[currpid];

	mask = xsec_begn(2, sleepqlock, prptr->prlock);

	/* check for state changed by other cpu */
	if (prptr->prstate != PR_CURR){
		xsec_endn(mask, 2, sleepqlock, prptr->prlock);
	}

	/* Delay calling process */

	if (insertd(currpid, sleepq, delay) == SYSERR) {
		xsec_endn(mask, 2, sleepqlock, prptr->prlock);
		return SYSERR;
	}

	proctab[currpid].prstate = PR_SLEEP;
	resched();
	
	xsec_endn(mask, 2, sleepqlock, prptr->prlock);
	return OK;
}
