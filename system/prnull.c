/* prnull.c	- prnull */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  prnull  -  Null process 
 *------------------------------------------------------------------------
 */
void	prnull(void){
	if(getcid() != 0){
		kprintf("cpu %d prnull\n");
		resched();
		enable();
	}
	while(TRUE);
}
