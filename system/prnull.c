/* prnull.c	- prnull */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  prnull  -  Null process 
 *------------------------------------------------------------------------
 */
void	prnull(void){
	if(getcid() != 0){
		// kprintf("cpu %d prnull\n", getcid());
		// resched();
		enable();
		// kprintf("cpu %d prnull: enabled interrupts\n", getcid());
	}
	while(TRUE){
	    // if(getcid()==1){kprintf("cpu %d in loop\n", getcid()); MDELAY(1000);}
		// resched();
	}
}
