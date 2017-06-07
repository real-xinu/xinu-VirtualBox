/* usbdinit.c  -  usbdinit */

#include <xinu.h>

struct	usbdcblk usbdtab[NUSBD];

/*------------------------------------------------------------------------
 * usbdinit  -  Initialize the USBD device
 *------------------------------------------------------------------------
 */
devcall	usbdinit (
		struct	dentry *devptr	/* Entry in device switch table	*/
		)
{
	struct	usbdcblk *usbdptr;	/* USB device control block	*/

	usbdptr = &usbdtab[devptr->dvminor];

	memset(usbdptr, 0, sizeof(*usbdptr));
	usbdptr->devid = devptr->dvnum;
	usbdptr->state = USBD_STATE_FREE;

	return OK;
}
