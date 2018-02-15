/* cpu.h - Definitions related to CPU */

#define	MAX_CPU	8

#define	CPU_STATE_DOWN	0
#define	CPU_STATE_UP	1

#define NCPU 4

struct	cpuent {
	/* startup/initialization variables */
	int32	state;	/* State of the CPU	*/
	byte	apicid;	/* APIC ID of the CPU	*/
	bool8	bsp;	/* Bootstrap processor	*/
	void	(*func)(void);
			/* Function to be called*/
			/* on interrupt		*/
	/* steady/run state variables, only accesed by owner */
	pid32 cpid;					/* ID of currently executing process */
	pid32 ppid;					/* ID of previously executing process */
	struct deferent defer;		/* Deferred scheduling for cpu */
	uint32 preempt;				/* count 1000 ms for cpu */
};

extern	int32	ncpu;
extern	struct	cpuent 	cputab[];
extern  lid32	cpubootlock;

#define CPU_NONE	-1
#define CPU_ALL		-2

#define isbadcid(x)	(x < 0 || x >= NCPU)

#define IPI_RESCHED 40	/* inter-processor interrupt to cause resched */
#define IPI_SUSPEND 49  /* inter-processor interrupt to cause suspend */
