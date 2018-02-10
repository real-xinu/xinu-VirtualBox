/* receive.c - receive */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  receive  -  Wait for a message and return the message to the caller
 *------------------------------------------------------------------------
 */
umsg32	receive(void)
{
	intmask	mask;			/* Saved interrupt mask		*/
	struct	procent *prptr;		/* Ptr to process's table entry	*/
	umsg32	msg;			/* Message to return		*/

	prptr = &proctab[currpid];

	mask = xsec_beg(prptr->prlock);

	/* Check for state changed by another processor */
	if(prptr->prstate != PR_CURR){ 
		xsec_end(mask, prptr->prlock);
		return SYSERR;
	}

	if (prptr->prhasmsg == FALSE) {
		prptr->prstate = PR_RECV;
		resched();		/* Block until message arrives	*/
	}

	msg = prptr->prmsg;		/* Retrieve message		*/
	prptr->prhasmsg = FALSE;	/* Reset message flag		*/
	
	xsec_end(mask, prptr->prlock);
	return msg;
}