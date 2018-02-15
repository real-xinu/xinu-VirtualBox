/* prstart.c - prstart */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  prstart  -  first function executed by a new process
 *------------------------------------------------------------------------
 */
void	prstart(void){

    /* perform duties required after return from context switch */
    ctxsw_ret();

    /* start running with interrupts enabled */
    enable();

	return;
}