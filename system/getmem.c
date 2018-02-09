/* getmem.c - getmem */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  getmem  -  Allocate heap storage, returning lowest word address
 *------------------------------------------------------------------------
 */
char  	*getmem(
	  uint32	nbytes		/* Size of memory requested	*/
	)
{
	intmask	mask;			/* Saved interrupt mask		*/
	struct	memblk	*prev, *curr, *leftover;

	if (nbytes == 0) {
		return (char *)SYSERR;
	}

	nbytes = (uint32) roundmb(nbytes);	/* Use memblk multiples	*/

	mask = xsec_beg(memlock);

	prev = &memlist;
	curr = memlist.mnext;
	while (curr != NULL) {			/* Search free list	*/
		if (curr->mlength == nbytes) {	/* Block is exact match	*/
			prev->mnext = curr->mnext;
			memlist.mlength -= nbytes;
			xsec_end(mask, memlock);
			return (char *)(curr);

		} else if (curr->mlength > nbytes) { /* Split big block	*/
			leftover = (struct memblk *)((uint32) curr +
					nbytes);
			prev->mnext = leftover;
			leftover->mnext = curr->mnext;
			leftover->mlength = curr->mlength - nbytes;
			memlist.mlength -= nbytes;
			xsec_end(mask, memlock);
			return (char *)(curr);
		} else {			/* Move to next block	*/
			prev = curr;
			curr = curr->mnext;
		}
	}

	xsec_end(mask, memlock);
	return (char *)SYSERR;
}
