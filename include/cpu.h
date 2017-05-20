/* cpu.h - Definitions related to CPU */

#define	MAX_CPU	8

#define	CPU_STATE_DOWN	0
#define	CPU_STATE_UP	1

struct	cpuent {
	int32	state;	/* State of the CPU	*/
	byte	apicid;	/* APIC ID of the CPU	*/
	bool8	bsp;	/* Bootstrap processor	*/
	void	(*func)(void);
			/* Function to be called*/
			/* on interrupt		*/
};

extern	int32	ncpu;
extern	struct	cpuent 	cputab[];
