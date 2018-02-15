/* ctxsw_ret.c - ctxsw_ret */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  ctxsw_ret  -  called after process returns from a context switch
 *------------------------------------------------------------------------
 */
void	ctxsw_ret(void){

    /* handle dying process	*/
	if (proctab[prevpid].prstate == PR_DEAD){
		freestk(proctab[prevpid].prstkbase, proctab[prevpid].prstklen);
		proctab[prevpid].prstate = PR_FREE;
	}

    /* Unlock locks used in rescheduling */
    unlock(proctab[currpid].prlock); /* current process */
    unlock(proctab[prevpid].prlock); /* previous process */
    unlock(readylock);

	return;
}
