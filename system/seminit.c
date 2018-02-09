/* seminit.c - seminit */

#include <xinu.h>

lid32	semtablock;				/* Lock on semaphore table */

/*------------------------------------------------------------------------
 *  seminit  - Initialize semaphores
 *------------------------------------------------------------------------
 */
status seminit(void){

	uint32 i;	/* iterator over semtab */
	struct	sentry	*semptr;	/* Ptr to semaphore table entry	*/

	/* Initialize semaphores */

	for (i = 0; i < NSEM; i++) {
		semptr = &semtab[i];
		semptr->sstate = S_FREE;
		semptr->scount = 0;
		semptr->squeue = newqueue();
		semptr->slock = newlock();
	}

	semtablock = newlock();

	return OK;
}
