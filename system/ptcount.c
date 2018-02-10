/* ptcount.c - ptcount */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  ptcount  -  Return the count of messages currently waiting in a port
 *		  (a non-negative count K means the port contains K
 *		   K messages,  including messages from senders that
 *		   are blocked waiting to send a message; a count of
 *		   negative K means a port is empty and K processes
 *		   are blocked waiting to receive a message)
 *------------------------------------------------------------------------
 */
int32	ptcount(
	  int32		portid		/* ID of a port to use		*/
	)
{
	intmask	mask;			/* Saved interrupt mask		*/
	int32	count;			/* Count of messages available	*/
	int32	sndcnt;			/* Count of sender semaphore	*/
	struct	ptentry	*ptptr;		/* Pointer to port table entry	*/

	if (isbadport(portid)){
		return SYSERR;
	}
	ptptr= &porttab[portid];

	mask = xsec_beg(portlock);

	if (ptptr->ptstate != PT_ALLOC){
		xsec_end(mask, portlock);
		return SYSERR;
	}

	/* Get count of messages available */
	lock(semtab[ptptr->ptrsem].slock); /* prevent count change */
	count = semcount(ptptr->ptrsem);

	/* If messages are waiting, check for blocked senders */

	if (count >= 0) {
		lock(semtab[ptptr->ptssem].slock); /* prevent sndcnt change */
		sndcnt = semcount(ptptr->ptssem);
		if (sndcnt < 0) {	/* -sndcnt senders blocked */
			count += -sndcnt;
		}
		unlock(semtab[ptptr->ptssem].slock);
	}

	unlock(semtab[ptptr->ptrsem].slock);

	xsec_end(mask, portlock);
	return count;
}
