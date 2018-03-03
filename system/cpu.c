/* cpu.c */

#include <xinu.h>

struct	cpuent cputab[MAX_CPU];
int32	ncpu = 0;
lid32	cpubootlock;

/* cpu_start1: 16-bit code that runs when a CPU is started:
   (gcc-4 cannot generate 16-bit code. This code is generated
    using gcc-5)

.code16gcc
	.text
	.globl cpu_start1

cpu_start1:
	cli
	mov	$0x40, %eax	# GDTR is located at 0x40
	mov	%cr0, %ebx	# Read CR0 into ebx
	or	$1, %ebx	# Set the Protected Mode bit
	lgdt	(%eax)		# Load the GDT
	mov	%ebx, %cr0	# Write CR0 (enter Protected Mode)
	ljmp	$0x8, $0x2000	# Long-jump to 0x2000
				# This loads the Code segment
*/
byte	cpu_start1[] = { 0xfa, 0x66, 0xb8, 0x40, 0x00, 0x00, 0x00, 0x0f,
			 0x20, 0xc3, 0x66, 0x83, 0xcb, 0x01, 0x67, 0x0f,
			 0x01, 0x10, 0x0f, 0x22, 0xc3, 0xea, 0x00, 0x20,
			 0x08, 0x00, 0xb8, 0x10 };

void	cpu_start2();
void	cpu_start2_end();
extern	struct	idt idt[];

/* ipi dispatchers */
void	cpudisp(void);
void	resched_disp(void);
void	suspend_disp(void);

/*------------------------------------------------------------------------
 *  cpuinit  -  Initialize cpu entry information
 *------------------------------------------------------------------------
 */
void cpuinit(void){
	int32 i;				/* iterator over cores */
	struct cpuent* cpuptr;	/* pointer to cpu entry */

	/* set resched and suspend ipi handlers for cpu 0 */
	set_ipi_handler(IPI_RESCHED, (uint32)resched_disp);
	set_ipi_handler(IPI_SUSPEND, (uint32)suspend_disp);

	for(i = 1; i < NCPU; i++){
		cpuptr = &cputab[i];

		/* Scheduling is not currently blocked */
		cpuptr->defer.ndefers = 0;
		cpuptr->defer.attempt = FALSE;

		/* Initialize current and previous processes */
		cpuptr->cpid = i;
		cpuptr->ppid = i;

		/* Set initial preemption time */
		cpuptr->preempt = QUANTUM;

		/* Set auxiliary cores running null process */
		cpu_run(i, prnull);
		sleepms(20);
	}
}

/*------------------------------------------------------------------------
 * cpu_run  -  Execute a function on the specified CPU
 *------------------------------------------------------------------------
 */
void	cpu_run (
		int32	apicid,		/* CPU APIC ID		*/
		void	(*func)(void)	/* Function pointer	*/
		)
{
	struct	cpuent *cpuptr;	/* CPU entry pointer	*/
	byte	*ptr;		/* Byte pointer		*/
	int32	i;

	if(apicid == lapic->lapic_id) {	/* Target CPU is this CPU! */
		func();
		return;
	}

	for(i = 0; i < ncpu; i++) {
		if(cputab[i].apicid == apicid) {
			break;
		}
	}

	if(i >= ncpu) {
		return;
	}

	cpuptr = &cputab[i];
	cpuptr->func = func;

	if(cpuptr->state == CPU_STATE_DOWN) {

		/* We need to bootstrap this CPU */

		/* Copy the GDT at address 0 */

		memcpy(0, gdt_copy, 8*8);

		ptr = (byte *)(64);

		*((uint16 *)ptr) = 63;
		*((uint32 *)(ptr + 2)) = 0;

		/* Copy cpu_start1 code at address 4096 */

		memcpy((char *)0x1000, cpu_start1, sizeof(cpu_start1));

		/* Copy cpu_start2 code at address 8192 */

		memcpy((char *)0x2000, cpu_start2,
				(char *)cpu_start2_end - (char *)cpu_start2);

		/* Send INIT to the target CPU */

		lapic->icr_high = (apicid << 24) & 0xFF000000;
		lapic->icr_low = 0x00004500;

		sleepms(20);

		/* Send SIPI with vector 0x01 (i.e. address 4096) */

		lapic->icr_high = (apicid << 24) & 0xFF000000;
		lapic->icr_low = 0x00004601;
	}
	else {
		/* Send an interrupt (50) to the target processor */

		lapic->icr_high = (apicid << 24) & 0xFF000000;
		lapic->icr_low = 0x00004032;
	}
}

/*------------------------------------------------------------------------
 * cpu_init  -  Initialize data structures for a CPU
 *------------------------------------------------------------------------
 */
void	cpu_init (void) {

	struct	idt *pidt;
	int32	apicid;

	/* Install an interrupt handler for int 50 */

	pidt = &idt[50];
	pidt->igd_loffset = (uint32)cpudisp;
	pidt->igd_segsel = 0x8;
	pidt->igd_mbz = 0;
	pidt->igd_type = IGDT_TRAPG;
	pidt->igd_dpl = 0;
	pidt->igd_present = 1;
	pidt->igd_hoffset = (uint32)cpudisp >> 16;

	/* set resched and suspend ipi handlers */
	set_ipi_handler(IPI_RESCHED, (uint32)resched_disp);
	set_ipi_handler(IPI_SUSPEND, (uint32)suspend_disp);
	set_ipi_handler(IRQBASE, (uint32)clkdisp);

	/* Load the IDT */

	lidt();

	apicid = lapic->lapic_id >> 24;

//	kprintf("cpu_init: %d\n", apicid);

	/* Enable the Local APIC */

	lapic->sivr |= 0x00000100;

	cputab[apicid].state = CPU_STATE_UP;

	if(cputab[apicid].func) {
		(cputab[apicid].func)();
	}
}

/*------------------------------------------------------------------------
 * cpuhandler  -  Handle interrupt sent by another processor
 *------------------------------------------------------------------------
 */
void	cpuhandler (void) {

	int32	apicid;
	int32	i;

	apicid = lapic->lapic_id >> 24;

//	kprintf("cpuhandler: cpu %d\n", apicid);

	for(i = 0; i < ncpu; i++) {
		if(cputab[i].apicid == apicid) {
			break;
		}
	}

	if(i >= ncpu) {
		return;
	}

	if(cputab[i].func) {
		(cputab[i].func)();
	}
}

/*------------------------------------------------------------------------
 * resched_handler  -  Handle resched IPI
 *------------------------------------------------------------------------
 */
void resched_handler (void) {
	lapic->eoi = 0;
	resched();
	return;
}

/*------------------------------------------------------------------------
 * suspend_handler  -  Handle suspend IPI
 *------------------------------------------------------------------------
 */
void suspend_handler (void) {
	lapic->eoi = 0;
	suspend(currpid);
	return;
}

/*------------------------------------------------------------------------
 * cpudisp  -  Dispatcher function for inter-processor interrupt
 *------------------------------------------------------------------------
 */
asm (

	".globl	cpudisp\n\t"
	"cpudisp:\n\t"
	"call cpuhandler\n\t"
	"iret\n\t"
);

/*------------------------------------------------------------------------
 * resched_disp  -  Dispatcher function for resched ipi
 *------------------------------------------------------------------------
 */
asm (

	".globl	resched_disp\n\t"
	"resched_disp:\n\t"
	"cli\n\t"
	"call resched_handler\n\t"
	"iret\n\t"
);

/*------------------------------------------------------------------------
 * suspend_disp  -  Dispatcher function for suspend ipi
 *------------------------------------------------------------------------
 */
asm (

	".globl	suspend_disp\n\t"
	"suspend_disp:\n\t"
	"cli\n\t"
	"call suspend_handler\n\t"
	"iret\n\t"
);

/*------------------------------------------------------------------------
 *  getcid  -  Return the ID of the currently executing core
 *------------------------------------------------------------------------
 */
cid32 getcid(void){
	return (lapic->lapic_id >> 24);
}

/*------------------------------------------------------------------------
 *  sendipi  -  Generate inter-processor interrupt on given core
 *------------------------------------------------------------------------
 */
status sendipi(
    int32 ipi,	/* interrupt number to generate */
    cid32 core	/* core on which to generate interrupt */
    )
{
	lapic->icr_high = (core << 24) & 0xFF000000;
	lapic->icr_low = 0x00004000 | ipi;
    return OK;
}

/*------------------------------------------------------------------------
 *  bcastipi  -  broadcast inter-processor interrupt on all cores 
 * 					 except caller
 *------------------------------------------------------------------------
 */
status bcastipi(
    int32 ipi	/* interrupt number to generate */
    )
{
	lapic->icr_low = 0x000C4000 | ipi;
    return OK;
}

/*------------------------------------------------------------------------
 *  set_ipi_handler  -  Set ipi handler for inter-processor interrupt
 *------------------------------------------------------------------------
 */
status set_ipi_handler(
    uint32 ipi,		/* ipi number */
    uint32 handler	/* handler for ipi */
    )
{
	struct	idt	*pidt;
	pidt = &idt[ipi];
	pidt->igd_loffset = handler;
	pidt->igd_segsel = 0x8;
	pidt->igd_mbz = 0;
	pidt->igd_type = IGDT_TRAPG;
	pidt->igd_dpl = 0;
	pidt->igd_present = 1;
	pidt->igd_hoffset = handler >> 16;
    return OK;
}