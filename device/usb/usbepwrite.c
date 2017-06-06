/* usbepwrite.c - usbepwrite */

#include <xinu.h>

/*------------------------------------------------------------------------
 * usbepwrite  -  Write to a USB endpoint
 *------------------------------------------------------------------------
 */
devcall	usbepwrite (
		struct	dentry *devptr,	/* Entry in device switch table	*/
		char	*buf,		/* Buffer to write from		*/
		uint32	count		/* No. of bytes to write	*/
		)
{
	struct	usbepcblk *epcptr;	/* Endpoint control block	*/
	struct	usbtransfer utfr;	/* USB transfer information	*/

	epcptr = &usbeptab[devptr->dvnum];

	if(epcptr->state == USBEP_STATE_FREE) {
		return SYSERR;
	}

	if(epcptr->epptr->dir != USBEP_DIR_OUT) {
		return SYSERR;
	}

	utfr.usbdptr = epcptr->usbdptr;

	switch(epcptr->epptr->type) {

		case USBEP_TYPE_ISO:  //TODO
		case USBEP_TYPE_INTR: //TODO
		case USBEP_TYPE_CTRL:
			return SYSERR;

		case USBEP_TYPE_BULK:
			utfr.eptype = USB_TFR_EP_BULK;
			break;

		default:
			return SYSERR;
	}

	utfr.ep = epcptr->epptr->addr;
	utfr.dirin = FALSE;
	utfr.dvrq = NULL;
	utfr.buffer = buf;
	utfr.size = count;

	control(USB, USB_CTRL_TRANSFER, (int32)&utfr, 0);

	//TODO
	return count;
}
