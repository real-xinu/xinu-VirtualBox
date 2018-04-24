/* xsec.c	-	xsec_beg, xsec_end */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  xsec_beg  -  Begin XINU kernel critical section
 *------------------------------------------------------------------------
 */
intmask xsec_beg(
		lid32 lid	/* id of spinlock to lock */
		){
	intmask mask;	/* Saved interrupt mask	*/

	/* Disable interrupts to prevent handler deadlocks */
	mask = disable();

	/* Defer rescheduling so the process doesn't yield the processor
	 * while holding a spinlock.
	 */
	resched_cntl(DEFER_START);

	/* Lock the spinlock necessary to protect XINU critical section */
 	lock(lid);

	return mask;
}

/*------------------------------------------------------------------------
 *  xsec_end  -  End XINU kernel critical section
 *------------------------------------------------------------------------
 */
 status xsec_end(
  		intmask mask,	/* saved interrupt mask to restore */
 		lid32 lid		/* id of spinlock to unlock */
  		){

	/* Undo operations in reverse order of xsec_beg */
	unlock(lid);

	resched_cntl(DEFER_STOP);

	restore(mask);

	return OK;
}

/*------------------------------------------------------------------------
 *  xsec_begn  -  Begin XINU kernel critical section with multiple locks
 *------------------------------------------------------------------------
 */
intmask xsec_begn(
		uint32 nlocks,	/* number of spinlocks to lock 	*/
		...				/* list of spinlocks to lock 	*/
		){
	intmask mask;	/* Saved interrupt mask	*/
	lid32* locks;	/* Point to lock list 	*/
	int32 i;		/* Iterate over locks 	*/

	/* Disable interrupts to prevent handler deadlocks */
	mask = disable();

	/* Defer rescheduling so the process doesn't yield the processor
	 * while holding a spinlock.
	 */
	resched_cntl(DEFER_START);

	/* Lock the spinlock necessary to protect XINU critical section */
	locks = (lid32*)(&nlocks + 1);
	for(i = 0; i < nlocks; i++){
 		lock(locks[i]);
	}

	return mask;
}

/*------------------------------------------------------------------------
 *  xsec_endn  -  End XINU kernel critical section with multiple locks
 *------------------------------------------------------------------------
 */
 status xsec_endn(
  		intmask mask,	/* saved interrupt mask to restore	*/
 		uint32 nlocks,	/* number of spinlocks to unlock 	*/
		...				/* list of spinlocks to unlock 		*/
  		){
	lid32* locks;	/* Point to lock list 	*/
	int32 i;		/* Iterate over locks 	*/

	/* Undo operations in reverse order of xsec_begn */

	locks = (lid32*)(&nlocks + 1);
	for(i = (nlocks-1); i >= 0; i--){
 		unlock(locks[i]);
	}

	resched_cntl(DEFER_STOP);

	restore(mask);

	return OK;
}
