#include <xinu.h>

/*------------------------------------------------------------------------
 * mdemod - multicore demo daemon
 *------------------------------------------------------------------------
 */
void mdemod(
        int deltat  /* time to sleep after running (s) */
) {
    while(TRUE){
        printf("Hello from demo daemon! pid: %d, core %d\n",
                          getpid(), getcid());
        MDELAY(1000*deltat);
    }
}

/*------------------------------------------------------------------------
 * xsh_demo - run multicore demo daemon process from command line
 *------------------------------------------------------------------------
 */
shellcmd xsh_demo(int nargs, char *args[])
{
    int sleep_time = 1; /* default sleep time */

    /* sanity check */
    switch (nargs) {
        case 2:
            sleep_time = (int)(args[1] - 0x30);
            /* fall through */
        case 1:
            break;
        default:
            printf("Use: %s <sleep_time>\n", args[0]);
    }

    /* start multicore demo daemon */
    resume(create(mdemod, 1024, 20, "mdemod", 1, sleep_time));

    return 0;
}
