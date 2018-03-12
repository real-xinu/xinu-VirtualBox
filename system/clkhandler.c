/* clkhandler.c - clkhandler */

#include <xinu.h>

/*------------------------------------------------------------------------
 * clkhandler - high level clock interrupt handler
 *------------------------------------------------------------------------
 */
void	clkhandler()
{
	static	uint32	count1000 = 1000;	/* Count to 1000 ms	*/
	struct cpuent* cpuptr;			/* pointer to cpu entry		*/

	cpuptr = &cputab[getcid()];

	/* cpu 0 handles global timer state and sleep queue */

	if(getcid() == 0){

		/* Decrement the ms counter, and see if a second has passed */

		if((--count1000) <= 0) {

			/* One second has passed, so increment seconds count */

			clktime++;

			/* make new clktime visible to other cores */

			__sync_synchronize();

			/* Reset the local ms counter for the next second */

			count1000 = 1000;
		}

		/* Handle sleeping processes if any exist */
		resched_cntl(DEFER_START);
		lock(sleepqlock);

		if(!isempty(sleepq)) {

			/* Decrement the delay for the first process on the	*/
			/*   sleep queue, and awaken if the count reaches zero	*/

			if((--queuetab[firstid(sleepq)].qkey) <= 0) {
				wakeup();
			}
		}

		unlock(sleepqlock);
		resched_cntl(DEFER_STOP);
	}

	/* Decrement the preemption counter, and reschedule when the */
	/*   remaining time reaches zero			     */

	if((--(cpuptr->preempt)) <= 0) {
		cpuptr->preempt = QUANTUM;
		resched();
	}
}
