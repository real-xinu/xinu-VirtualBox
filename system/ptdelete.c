/* ptdelete.c - ptdelete */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  ptdelete  -  Delete a port, freeing waiting processes and messages
 *------------------------------------------------------------------------
 */
syscall	ptdelete(
	  int32		portid,		/* ID of port to delete		*/
	  int32		(*disp)(int32)	/* Function to call to dispose	*/
	)				/*    of waiting messages	*/
{
	intmask	mask;			/* Saved interrupt mask		*/
	struct	ptentry	*ptptr;		/* Pointer to port table entry	*/

	if (isbadport(portid)){
		return SYSERR;
	}
	ptptr = &porttab[portid];

	mask = xsec_beg(portlock);

	if(ptptr->ptstate != PT_ALLOC){
		xsec_end(mask, portlock);
		return SYSERR;
	}

	_ptclear(ptptr, PT_FREE, disp);
	ptnextid = portid;

	xsec_end(mask, portlock);
	return OK;
}
