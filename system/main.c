/*  main.c  - main */

#include <xinu.h>

void	cpufunc1 (void) {

	kprintf("Message 1 on processor: %d\n", lapic->lapic_id >> 24);
}

void	cpufunc2 (void) {

	kprintf("Message 2 on processor: %d\n", lapic->lapic_id >> 24);
}

process	main(void)
{
//	cpu_run(1, cpufunc1);

	sleep(2);

//	cpu_run(2, cpufunc1);

//	sleep(2);

//	cpu_run(1, cpufunc2);

//	sleep(2);

//	cpu_run(2, cpufunc2);

	sleep(10);

	kprintf("lcount for 204 = %d\n", locktab[204].lcount);

	/* Run the Xinu shell */

	recvclr();
	resume(create(shell, 8192, 50, "shell", 1, CONSOLE));

	/* Wait for shell to exit and recreate it */

	while (TRUE) {
		receive();
		sleepms(200);
		kprintf("\n\nMain process recreating shell\n\n");
		resume(create(shell, 4096, 20, "shell", 1, CONSOLE));
	}
	return OK;
    
}
