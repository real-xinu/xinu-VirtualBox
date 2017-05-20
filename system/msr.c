/* msr.c - msr_read, msr_write */

#include <xinu.h>

/*------------------------------------------------------------------------
 * msr_read  -  Read a model specific register
 *------------------------------------------------------------------------
 */
uint64	msr_read (
		uint32	reg	/* Register to read	*/
		)
{
	uint32	val_low, val_high;

	asm volatile (
		"rdmsr\t\n"
		"movl %%eax,%0\t\n"
		: "=a" (val_low), "=d" (val_high)
		: "c" (reg)
		:
	);

	return ((uint64)val_high << 32 | val_low);
}

/*------------------------------------------------------------------------
 * msr_write  -  Write to a model specific register
 *------------------------------------------------------------------------
 */
void	msr_write (
		uint32	reg,	/* Register to write to	*/
		uint64	value	/* Value to be written	*/
		)
{
	uint32	val_low, val_high;

	val_low = value;
	val_high = value >> 32;

	asm volatile (
		"wrmsr\n\t"
		:
		: "d" (val_high), "a" (val_low), "c" (reg)
		:
		);
}
