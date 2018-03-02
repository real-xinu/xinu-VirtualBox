/*  main.c  - main */

#include <xinu.h>

int32 slock;
lid32 mylock;
int32 var;

void	cpufunc1 (void) {

	// kprintf("Message 1 on processor: %d\n", lapic->lapic_id >> 24);
	int i;
	for(i = 0; i < 10000000; i++){
		// spin_lock(&slock);
		lock(mylock);
		if (i % 100000 == 0){
			kprintf("%d", getcid());
		}
		var--;
		// spin_unlock(&slock);
		unlock(mylock);
	}
}

void	cpufunc2 (void) {

	// kprintf("Message 2 on processor: %d\n", lapic->lapic_id >> 24);
	int i;
	for(i = 0; i < 100000; i++){
		spin_lock(&slock);
		if (i % 100000 == 0){
			kprintf("%d", getcid());
		}
		var--;
		spin_unlock(&slock);
	}
}

process	main(void)
{
	// var = 0;
	// slock = 0;
	// mylock = newlock();
	// int i;
	// cpu_run(1, cpufunc1);
	
	// for(i = 0; i < 10000000; i++){
	// 	// spin_lock(&slock);
	// 	lock(mylock);
	// 	if (i % 100000 == 0){
	// 		kprintf("%d", getcid());
	// 	}
	// 	var++;
	// 	// spin_unlock(&slock);
	// 	unlock(mylock);
	// }

	// sleep(1);

	// kprintf("\nvar = %d\n", var);

	// sleep(1);

	// cpu_run(1, prnull);

	// sleep(1);

	// cpu_run(1, cpufunc2);

	// sleep(2);

	// cpu_run(2, cpufunc2);

	// sleep(1);
	// sleep(1000);
	// cpu_run(1, prnull);
	// sleep(1);

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
