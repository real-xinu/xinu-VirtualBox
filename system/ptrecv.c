/* ptrecv.c - ptrecv */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  ptrecv  -  Receive a message from a port, blocking if port empty
 *------------------------------------------------------------------------
 */
uint32	ptrecv(
	  int32		portid		/* ID of port to use		*/
	)
{
	intmask	mask;			/* Saved interrupt mask		*/
	struct	ptentry	*ptptr;		/* Pointer to table entry	*/
	int32	seq;			/* Local copy of sequence num.	*/
	umsg32	msg;			/* Message to return		*/
	struct	ptnode	*msgnode;	/* First node on message list	*/

	if (isbadport(portid)){
		return SYSERR;
	}
	ptptr = &porttab[portid];

	mask = disable();
	lock(portlock);
	if(ptptr->ptstate != PT_ALLOC){
		unlock(portlock);
		restore(mask);
		return (uint32)SYSERR;
	}

	/* Wait for message and verify that the port is still allocated */

	seq = ptptr->ptseq;		/* Record orignal sequence	*/
	unlock(portlock);		/* Can't call wait() while holding lock */

	if (wait(ptptr->ptrsem) == SYSERR){
		restore(mask);
		return (uint32)SYSERR;
	}

	lock(portlock);

	if(ptptr->ptstate != PT_ALLOC || ptptr->ptseq != seq){
		unlock(portlock);
		restore(mask);
		return (uint32)SYSERR;
	}

	/* Dequeue first message that is waiting in the port */

	msgnode = ptptr->pthead;
	msg = msgnode->ptmsg;
	if (ptptr->pthead == ptptr->pttail)	/* Delete last item	*/
		ptptr->pthead = ptptr->pttail = NULL;
	else
		ptptr->pthead = msgnode->ptnext;
	msgnode->ptnext = ptfree;		/* Return to free list	*/
	ptfree = msgnode;
	
	resched_cntl(DEFER_START); /* can't resched while holding lock */
	signal(ptptr->ptssem);
	unlock(portlock);
	resched_cntl(DEFER_STOP);
	restore(mask);
	return msg;
}
