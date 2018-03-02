/* spinlock.c - spin_lock, spin_unlock */

#include <xinu.h>

/*------------------------------------------------------------------------
 * spin_lock  -  Lock a spinlock using test and set 
 *------------------------------------------------------------------------
 */
void spin_lock(volatile int* lockaddr){
    while (__sync_lock_test_and_set(lockaddr, 1)){
        while(*lockaddr);
    }
}

/*------------------------------------------------------------------------
 * spin_unlock  -  Unlock a spinlock
 *------------------------------------------------------------------------
 */
void spin_unlock(volatile int* lockaddr) {
    __sync_lock_release(lockaddr);
}

/*------------------------------------------------------------------------
 * spin_lock  -  Lock a spinlock using compare and swap
 *------------------------------------------------------------------------
 */
// void spin_lock(volatile int* lockaddr){
//     while (!__sync_bool_compare_and_swap(lockaddr, 0, 1)){
//         while(*lockaddr);
//     }
// }
