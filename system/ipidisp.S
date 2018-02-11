/* ipidisp.s - inter-processor interrupt dispatchers(x86): 
*    resched_disp, suspend_disp  
*/

#include <icu.s>
.text
.globl resched_disp     
.globl suspend_disp

/*------------------------------------------------------------------------
 * resched_disp  -  Interrupt dispatcher for resched interrupts 
 *------------------------------------------------------------------------
 */
resched_disp:
		pushal			/* Save general-purpose regs.	*/
		pushfl			/* Save the flags register	*/
		cli			/* Prevent further interrupts	*/
		movb	$EOI,%al	/* Clear the interrupt		*/
		outb	%al,$OCW1_2
		call	resched_handler	/* Call the handler		*/
		popfl			/* Restore the flags register	*/
		popal			/* Restore general-purpose regs.*/
		iret			/* Return from interrupt	*/


/*------------------------------------------------------------------------
 * suspend_disp	 -  Interrupt dispatcher for suspend interrupts 
 *------------------------------------------------------------------------
 */
suspend_disp:
		pushal			/* Save general-purpose regs.	*/
		pushfl			/* Save the flags register	*/
		cli			/* Prevent further interrupts	*/
		movb	$EOI,%al	/* Clear the interrupt		*/
		outb	%al,$OCW1_2
		call	suspend_handler	/* Call the handler		*/
		popfl			/* Restore the flags register	*/
		popal			/* Restore general-purpose regs.*/
		iret			/* Return from interrupt	*/