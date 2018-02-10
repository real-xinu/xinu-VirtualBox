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

	typtr = &ttytab[devptr->dvminor];

	/* Process the request */

	switch ( func )	{

	case TC_NEXTC:
		wait(typtr->tyisem);
		lock(typtr->tylock);
		ch = *typtr->tyitail;
		unlock(typtr->tylock);
		signal(typtr->tyisem);
		return (devcall)ch;

	case TC_MODER:
		lock(typtr->tylock);
		typtr->tyimode = TY_IMRAW;
		unlock(typtr->tylock);
		return (devcall)OK;

	case TC_MODEC:
		lock(typtr->tylock);
		typtr->tyimode = TY_IMCOOKED;
		unlock(typtr->tylock);
		return (devcall)OK;

	case TC_MODEK:
		lock(typtr->tylock);
		typtr->tyimode = TY_IMCBREAK;
		unlock(typtr->tylock);
		return (devcall)OK;

	case TC_ICHARS:
		return(semcount(typtr->tyisem));

	case TC_ECHO:
		lock(typtr->tylock);
		typtr->tyiecho = TRUE;
		unlock(typtr->tylock);
		return (devcall)OK;

	case TC_NOECHO:
		lock(typtr->tylock);
		typtr->tyiecho = FALSE;
		unlock(typtr->tylock);
		return (devcall)OK;

	default:
		return (devcall)SYSERR;
	}
}
