/* usbepread.c - usbepread */

#include <xinu.h>

/*------------------------------------------------------------------------
 * usbepread  -  Read from USB endpoint device
 *------------------------------------------------------------------------
 */
devcall	usbepread (
		struct	dentry *devptr,	/* Entry in device switch table	*/
		char	*buf,		/* Buffer to read data into	*/
		uint32	count		/* No. of bytes to be read	*/
		)
{
	struct	usbepcblk *epcptr;	/* Endpoint control block	*/
	struct	usbtransfer utfr;	/* USB transfer information	*/

	epcptr = &usbeptab[devptr->dvminor];

	if(epcptr->state == USBEP_STATE_FREE) {
		return SYSERR;
	}

	if(epcptr->epptr->dir != USBEP_DIR_IN) {
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
	utfr.dirin = TRUE;
	utfr.dvrq = NULL;
	utfr.buffer = buf;
	utfr.size = count;

	control(USB, USB_CTRL_TRANSFER, (int32)&utfr, 0);

	//TODO
	return count;
}
