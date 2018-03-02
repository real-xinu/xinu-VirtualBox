/*  main.c  - main */

#include <xinu.h>

int32 slock;
lid32 mylock;
sid32 mutex;
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

void	cpufunc2 (int id) {
	int i;
	for(i = 0; i < 10000000; i++){
		// spin_lock(&slock);
		// lock(mylock);
		wait(mutex);
		kprintf("\nhello from process %d, core %d. var = %d\n", currpid, getcid(), var);
		var--;
		// spin_unlock(&slock);
		// unlock(mylock);
		xsh_ps(0, NULL);
		signal(mutex);
		MDELAY(3000);
	}
}

process	main(void)
{
	var = 0;
	// slock = 0;
	mylock = newlock();
	int i;
	// cpu_run(1, cpufunc1);
	mutex = semcreate(1);
	
	resched_cntl(DEFER_START);
	resume(create(cpufunc2, 4096, 19, "cpudaemon1", 1, 1));
	// sleep(1);
	resume(create(cpufunc2, 4096, 19, "cpudaemon2", 1, 2));
	// sleep(1);
	resume(create(cpufunc2, 4096, 19, "cpudaemon3", 1, 3));
	resched_cntl(DEFER_STOP);


	/* Run the Xinu shell */

	recvclr();
	wait(mutex);
	resume(create(shell, 8192, 50, "shell", 1, CONSOLE));
	signal(mutex);

	/* Wait for shell to exit and recreate it */

	while (TRUE) {
		receive();
		sleepms(200);
		kprintf("\n\nMain process recreating shell\n\n");
		resume(create(shell, 4096, 20, "shell", 1, CONSOLE));
	}
	return OK;
    
}
