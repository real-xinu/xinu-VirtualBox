/* recvtime.c - recvtime */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  recvtime  -  Wait specified time to receive a message and return
 *------------------------------------------------------------------------
 */
umsg32	recvtime(
	  int32		maxwait		/* Ticks to wait before timeout */
        )
{
	intmask	mask;			/* Saved interrupt mask		*/
	struct	procent	*prptr;		/* Tbl entry of current process	*/
	umsg32	msg;			/* Message to return		*/

	if (maxwait < 0) {
		return SYSERR;
	}

	prptr = &proctab[currpid];

	mask = xsec_begn(2, sleepqlock, prptr->prlock);

	/* Check for state changed by another processor */
	if(prptr->prstate != PR_CURR){ 
		xsec_endn(mask, 2, sleepqlock, prptr->prlock);
		return SYSERR;
	}

	/* Schedule wakeup and place process in timed-receive state */

	if (prptr->prhasmsg == FALSE) {	/* Delay if no message waiting	*/
		if (insertd(currpid,sleepq,maxwait) == SYSERR) {
			xsec_endn(mask, 2, sleepqlock, prptr->prlock);
			return SYSERR;
		}
		prptr->prstate = PR_RECTIM;
		resched();
	}

	/* Either message arrived or timer expired */

	if (prptr->prhasmsg) {
		msg = prptr->prmsg;	/* Retrieve message		*/
		prptr->prhasmsg = FALSE;/* Reset message indicator	*/
	} else {
		msg = TIMEOUT;
	}

	xsec_endn(mask, 2, sleepqlock, prptr->prlock);
	return msg;
}
