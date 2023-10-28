/* ttycontrol.c - ttycontrol */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  ttycontrol  -  Control a tty device by setting modes
 *------------------------------------------------------------------------
 */
devcall	ttycontrol(
	  struct dentry	*devptr,	/* Entry in device switch table	*/
	  int32	 func,			/* Function to perform		*/
	  int32	 arg1,			/* Argument 1 for request	*/
	  int32	 arg2			/* Argument 2 for request	*/
	)
{
	struct	ttycblk	*typtr;		/* Pointer to tty control block	*/
	char	ch;			/* Character for lookahead	*/
	intmask mask;

	typtr = &ttytab[devptr->dvminor];

	/* Process the request */

	switch ( func )	{

	case TC_NEXTC:
		wait(typtr->tyisem);
		mask = xsec_beg(typtr->tylock);
		ch = *typtr->tyitail;
		xsec_end(mask, typtr->tylock);
		signal(typtr->tyisem);
		return (devcall)ch;

	case TC_MODER:
		mask = xsec_beg(typtr->tylock);
		typtr->tyimode = TY_IMRAW;
		xsec_end(mask, typtr->tylock);
		return (devcall)OK;

	case TC_MODEC:
		mask = xsec_beg(typtr->tylock);
		typtr->tyimode = TY_IMCOOKED;
		xsec_end(mask, typtr->tylock);
		return (devcall)OK;

	case TC_MODEK:
		mask = xsec_beg(typtr->tylock);
		typtr->tyimode = TY_IMCBREAK;
		xsec_end(mask, typtr->tylock);
		return (devcall)OK;

	case TC_ICHARS:
		return(semcount(typtr->tyisem));

	case TC_ECHO:
		mask = xsec_beg(typtr->tylock);
		typtr->tyiecho = TRUE;
		xsec_end(mask, typtr->tylock);
		return (devcall)OK;

	case TC_NOECHO:
		mask = xsec_beg(typtr->tylock);
		typtr->tyiecho = FALSE;
		xsec_end(mask, typtr->tylock);
		return (devcall)OK;

	default:
		return (devcall)SYSERR;
	}
}
