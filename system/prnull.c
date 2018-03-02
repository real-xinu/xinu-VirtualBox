/* prnull.c	- prnull */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  prnull  -  Null process 
 *------------------------------------------------------------------------
 */
void	prnull(void){
	if(getcid() != 0){
		printf("cpu %d prnull\n", getcid());
		resched();
		enable();
	}
	while(TRUE){
		// resched();
	}
}
