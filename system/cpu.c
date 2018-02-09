/* cpu.c */

#include <xinu.h>

struct	cpuent cputab[MAX_CPU];
int32	ncpu = 0;

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

/*------------------------------------------------------------------------
 *  cpuinit  -  Initialize cpu entry information
 *------------------------------------------------------------------------
 */
void cpuinit(void){
	int32 i;				/* iterator over cores */
	struct cpuent* cpuptr;	/* pointer to cpu entry */

	/* set resched and suspend ipi handlers */
	set_evec(IPI_RESCHED, (uint32)resched_disp);
	set_evec(IPI_SUSPEND, (uint32)suspend_disp);

	for(i = 0; i < NCPU; i++){
		cpuptr = &cputab[i];

		/* Scheduling is not currently blocked */
		cpuptr->defer.ndefers = 0;
		cpuptr->defer.attempt = FALSE;

		/* Initialize current and previous processes */
		cpuptr->cpid = i;
		cpuptr->ppid = i;

		/* Set initial preemption time */
		cpuptr->preempt = 1000;

		/* Set auxiliary cores running null process */
		if (i > 0) {
			cpu_run(i, prnull);
		}
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

void	cpudisp(void);

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

	/* Load the IDT */

	lidt();

	apicid = lapic->lapic_id >> 24;

	//kprintf("cpu_init: %d\n", apicid);

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

	//kprintf("cpuhandler: cpu %d\n", apicid);

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
 *  getcid  -  Return the ID of the currently executing core
 *------------------------------------------------------------------------
 */
cid32 getcid(void){
	return (lapic->lapic_id >> 24);
}

/*------------------------------------------------------------------------
 * set_irq_target - set gic distributor to forward irq to target cpu
 *------------------------------------------------------------------------
 */ // TODO:
// status set_irq_target(uint32 xnum, cid32 cpu)
// {
// 	struct gic_distreg* gicdist = (struct gic_distreg*)GIC_DIST_BASE;
// 	int32 pctgt = 0;	/* target mask */
// 	int32 i;		/* iterator over target mask */

// 	if(xnum > GIC_IRQ_MAX) {
// 		return SYSERR;
// 	}

// 	if(cpu == CPU_ALL){	/* forward irq to all cores */
// 		for (i = 0; i < NCPU; i++){
// 			pctgt |= (1 << i);
// 		}
// 	} else if (isbadcid(cpu)) {
// 		return SYSERR;
// 	} else {
// 		pctgt = (1 << cpu);
// 	}

// 	gicdist->pctgt[xnum] = pctgt;

// 	return OK;
// }

/*------------------------------------------------------------------------
 *  sendsgi  -  Generate inter-processor interrupt on given core
 *------------------------------------------------------------------------
 */
status sendipi(
    int32 ipi,	/* interrupt number to generate */
    cid32 core	/* core on which to generate interrupt */
    )
{
	
	/* Send an interrupt (50) to the target processor */
	lapic->icr_high = (core << 24) & 0xFF000000;
	lapic->icr_low = 0x00004000 & ipi;

    return OK;
}

/*------------------------------------------------------------------------
 *  bcastsgi  -  broadcast inter-processor interrupt on all cores 
 * 					 except caller
 *------------------------------------------------------------------------
 */
status bcastipi(
    int32 ipi	/* interrupt number to generate */
    )
{
	cid32 core;
	cid32 thiscore = getcid();
	for(core = 0; core < NCPU; core++){
		if(core != thiscore){
			sendipi(ipi, core);
		}
	}
	return OK;
}