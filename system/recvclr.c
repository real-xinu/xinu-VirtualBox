/* recvclr.c - recvclr */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  recvclr  -  Clear incoming message, and return message if one waiting
 *------------------------------------------------------------------------
 */
umsg32	recvclr(void)
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

	if (prptr->prhasmsg == TRUE) {
		msg = prptr->prmsg;	/* Retrieve message		*/
		prptr->prhasmsg = FALSE;/* Reset message flag		*/
	} else {
		msg = OK;
	}
	
	xsec_end(mask, prptr->prlock);
	return msg;
}
