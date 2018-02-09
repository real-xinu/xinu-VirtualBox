/* resched.h */

/* Constants and variables related to deferred rescheduling */

#define	DEFER_START	1	/* Start deferred rescheduling		*/
#define	DEFER_STOP	2	/* Stop  deferred rescheduling		*/

/* Structure that collects items related to deferred rescheduling	*/

struct	deferent	{
	int32	ndefers;	/* Number of outstanding defers 	*/
	int32	attempt;	/* Was resched called during the	*/
						/*   deferral period?			*/
};

#define	Defer	(cputab[getcid()].defer)
