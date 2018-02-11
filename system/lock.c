/* lock.c - lock, unlock, newlock */

#include <xinu.h>

struct	lentry	locktab[NSLK];	/* Lock Table				*/

/*------------------------------------------------------------------------
 *  lock  -  lock a spinlock
 *------------------------------------------------------------------------
 */
status lock(
		lid32	lid		/* id of lock to lock */
	){
	struct lentry* lockptr;	/* Ptr to lock table entry */
//	if(!getcid() && lid == 204){kprintf("cpu 0 locking %d\n", lid);}

	if(getcid()){kprintf("locking %d\n", lid);}
	if (isbadlid(lid)) {
		if(getcid()){kprintf("bad lid, returning\n");}
		return SYSERR;
	}

	lockptr = &locktab[lid];

	if(lockptr->lowner == getcid()){
		lockptr->lcount++;
		return OK;
	}
	
	if(getcid()){kprintf("lock owned by %d, spinning\n", lockptr->lowner);}

	spin_lock(&(lockptr->lock));
	lockptr->lowner = getcid();
	lockptr->lcount++;

	return OK;
}

/*------------------------------------------------------------------------
 *  unlock  -  unlock a spinlock
 *------------------------------------------------------------------------
 */
status unlock(
		lid32	lid		/* id of lock to unlock */
	){
//	if(!getcid() && lid == 204){kprintf("cpu 0 unlocking %d\n", lid);}

	struct lentry* lockptr;	/* Ptr to lock table entry */

	if (isbadlid(lid)) {
		return SYSERR;
	}

	lockptr = &locktab[lid];

	if(lockptr->lowner != getcid()){
		return SYSERR;
	}

	if(--lockptr->lcount == 0){
		lockptr->lowner = SLK_NONE;
		spin_unlock(&(lockptr->lock));
	}

	return OK;
}

/*------------------------------------------------------------------------
 *  newlock  -  Allocate and initialize a lock in the lock table
 *------------------------------------------------------------------------
 */
lid32	newlock(void)
{
	static lid32	nextlid = 0;	/* Next lock in locktab to use	*/
	lid32		l;		            /* ID of allocated lock 	*/

	l = nextlid++;

	if (l >= NSLK) {		        /* Check for table overflow	*/
		return SYSERR;
	}

	/* Initialize lock */

	locktab[l].lock = UNLOCKED;
	locktab[l].lowner = SLK_NONE;
	locktab[l].lcount = 0;

	return l;
}
