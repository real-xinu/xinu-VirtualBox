	.text
	.globl cpu_start2
	.globl cpu_start2_end
cpu_start2:
	movl	$0x10, %eax
	mov	%ax, %ds
	mov	%ax, %es
	mov	%ax, %fs
	mov	%ax, %gs
	movl	$0x18, %eax
	mov	%ax, %ss
	movl	$0x7000000, %esp
	movl	$0x7000000, %ebp
	sti
	lcall	$0x8, $cpu_init
cpu_loop:
	hlt
	jmp	cpu_loop
cpu_start2_end:
