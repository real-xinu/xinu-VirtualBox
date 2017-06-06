/* usbepinit.c - usbepinit */

#include <xinu.h>

struct	usbepcblk usbeptab[NUSBEP];

/*------------------------------------------------------------------------
 * usbepinit  -  Initializa a USB endpoint device
 *------------------------------------------------------------------------
 */
devcall	usbepinit (
		struct	dentry *devptr	/* Entry in device switch table	*/
		)
{
	struct	usbepcblk *epcptr;	/* Endpoint control block	*/

	epcptr = &usbeptab[devptr->dvminor];

	epcptr->state = USBEP_STATE_FREE;
	epcptr->devid = devptr->dvnum;
	epcptr->usbdptr = NULL;
	epcptr->epptr = NULL;

	return OK;
}
