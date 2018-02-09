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
	pushal			# Save registers
	cli			# Disable further interrupts
	movb	$EOI,%al	# Reset interrupt
	outb	%al,$OCW1_2

	call	resched	# Call high level handler

	sti			# Restore interrupt status
	popal			# Restore registers
	iret			# Return from interrupt


/*------------------------------------------------------------------------
 * suspend_disp	 -  Interrupt dispatcher for suspend interrupts 
 *------------------------------------------------------------------------
 */
suspend_disp:
	pushal			# Save registers
	cli			# Disable further interrupts
	movb	$EOI,%al	# Reset interrupt
	outb	%al,$OCW1_2

	call	suspend	# Call high level handler

	sti			# Restore interrupt status
	popal			# Restore registers
	iret			# Return from interrupt