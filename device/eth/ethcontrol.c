/* ethcontrol.c - ethcontrol */

#include <xinu.h>

/*------------------------------------------------------------------------
 * ethcontrol - implement control function for an eth device
 *------------------------------------------------------------------------
 */
devcall	ethcontrol(
	struct	dentry	*devptr,	/* Entry in device switch table	*/
	int32	func,			/* Control function		*/
	int32	arg1,			/* Argument 1, if needed 	*/
	int32	arg2			/* Argument 2, if needed	*/
	)
{
	struct	ethcblk	*ethptr; 	/* Pointer to control block	*/

	ethptr = &ethertab[devptr->dvminor];

	switch (func) {

		/* Get MAC address */

		case ETH_CTRL_GET_MAC:
			memcpy((byte *)arg1, ethptr->devAddress, 
					ETH_ADDR_LEN);
			break;

		default:
			return SYSERR;
	}

	return OK;
}
