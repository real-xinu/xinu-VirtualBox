/* ptreset.c - ptreset */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  ptreset  -  Reset a port, freeing waiting processes and messages and
			leaving the port ready for further use
 *------------------------------------------------------------------------
 */
syscall	ptreset(
	  int32		portid,		/* ID of port to reset		*/
	  int32		(*disp)(int32)	/* Function to call to dispose	*/
	)				/*   of waiting messages	*/
{
	intmask	mask;			/* Saved interrupt mask		*/
	struct	ptentry	*ptptr;		/* Pointer to port table entry	*/

	if (isbadport(portid)){
		return SYSERR;
	}
	ptptr= &porttab[portid];

	mask = xsec_beg(portlock);

	if(ptptr->ptstate != PT_ALLOC){
		xsec_end(mask, portlock);
		return SYSERR;
	}

	_ptclear(ptptr, PT_ALLOC, disp);
	
	xsec_end(mask, portlock);
	return OK;
}
