/* prnull.c	- prnull */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  prnull  -  Null process 
 *------------------------------------------------------------------------
 */
void	prnull(void){
	if(getcid() != 0){
		lock(cpubootlock);
		unlock(cpubootlock);
		resched();
		enable();
	}
	while(TRUE);
}
