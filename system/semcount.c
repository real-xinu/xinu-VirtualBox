/* semcount.c - semcount */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  semcount  -  Return the count of a semaphore (because any integer is
 *		   possible, return of SYSERR may be ambiguous)
 *------------------------------------------------------------------------
 */
syscall semcount(
	  sid32		semid		/* ID of semaphore to use	*/
	)
{
	intmask	mask;			/* Saved interrupt mask		*/
	int32	count;			/* Current sempahore count	*/

	if(isbadsem(semid)){
		return SYSERR;
	}

	mask = xsec_beg(semtab[semid].slock);
	
	if (semtab[semid].sstate == S_FREE) {
		xsec_end(mask, semtab[semid].slock);
		return SYSERR;
	}

	count = semtab[semid].scount;

	xsec_end(mask, semtab[semid].slock);
	return count;
}
