/* ehcidispatch.S - ehcidispatch */

#include <icu.s>
	.text
	.globl ehcidispatch
	.globl ehcihandler
ehcidispatch:
	pushal
	pushfl
	cli
	movb	$EOI, %al
	outb	%al, $OCW1_2
	movb	$EOI, %al
	outb	%al, $OCW2_2

	call	ehcihandler

	popfl
	popal
	iret
