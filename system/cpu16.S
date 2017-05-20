#.code16gcc
#	.text

#	.globl cpu_start1
#	.globl cpu_start1_end
#cpu_start1:
#	cli
#	mov	$0x40, %eax
#	mov	%cr0, %ebx
#	or	$1, %ebx
#	lgdt	(%eax)
#	mov	%ebx, %cr0
#	ljmp	$0x8, $0x2000
#cpu_start1_end:
