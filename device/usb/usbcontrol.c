/* usbcontrol.c - usbcontrol */

#include <xinu.h>

/*------------------------------------------------------------------------
 * usbcontrol  -  Control functions for device "USB"
 *------------------------------------------------------------------------
 */
devcall	usbcontrol (
		struct	dentry *devptr,	/* Entry in device table	*/
		int32	func,		/* Control function to perform	*/
		int32	arg1,		/* Argument 1 for request	*/
		int32	arg2		/* Argument 2 for request	*/
		)
{
	struct	usbtransfer *utfr;	/* USB transfer information	*/
	struct	ehcitransfer etfr;	/* EHCI transfer information	*/

	switch(func) {

	  case USB_CTRL_TRANSFER:

	    utfr = (struct usbtransfer *)arg1;

	    etfr.devaddr = utfr->usbdptr->address;
	    etfr.ep = utfr->ep;
	    etfr.dirin = utfr->dirin;
	    switch(utfr->eptype) {
	      case USB_TFR_EP_CTRL:
	        etfr.eptype = EHCI_TFR_EP_CTRL;
		etfr.ctrlbuffer = (char *)utfr->dvrq;
		break;
	      case USB_TFR_EP_BULK:
		etfr.eptype = EHCI_TFR_EP_BULK;
		break;
	      case USB_TFR_EP_INTR:
	        etfr.eptype = EHCI_TFR_EP_INTR;
		break;
	      case USB_TFR_EP_ISO:
		etfr.eptype = EHCI_TFR_EP_ISO;
		break;
	      default:
		return SYSERR;
	    }

	    etfr.buffer = utfr->buffer;
	    etfr.size = utfr->size;

	    control(utfr->usbdptr->hcidev, EHCI_CTRL_TRANSFER,
			    			(int32)&etfr, 0);
	    break;

	  default:
	    return SYSERR;
	}

	return OK;
}

/*------------------------------------------------------------------------
 * usb_get_dev_desc  -  Get device descriptor
 *------------------------------------------------------------------------
 */
int32	usb_get_dev_desc (
		did32	devid,	/* Index in device switch table	*/
		char	*buf,	/* Address of buffer		*/
		int32	size	/* Size of buffer		*/
		)
{
	struct	usbtransfer utfr;	/* USB transfer information	*/
	struct	usb_devreq *dvrq;	/* USB device request		*/

	dvrq = (struct usb_devreq *)getmem(sizeof(*dvrq));
	dvrq->reqtype = 0x80;
	dvrq->request = USB_DVRQ_GET_DESC;
	dvrq->value = 0x0100;
	dvrq->index = 0;
	dvrq->length = sizeof(struct usb_devdesc);

	utfr.usbdptr = (struct usbdcblk *)devtab[devid].dvcsr;
	utfr.eptype = USB_TFR_EP_CTRL;
	utfr.ep = 0;
	utfr.dirin = TRUE;
	utfr.dvrq = dvrq;
	utfr.buffer = buf;
	utfr.size = size;

	control(USB, USB_CTRL_TRANSFER, (int32)&utfr, 0);

	freemem((char *)dvrq, sizeof(*dvrq));

	return sizeof(struct usb_devdesc);
}

/*------------------------------------------------------------------------
 * usb_set_address  -  Set address of a USB device
 *------------------------------------------------------------------------
 */
status	usb_set_address (
		did32	devid	/* Index in device switch table	*/
		)
{
	struct	usbdcblk *usbdptr;	/* USB device control block	*/
	struct	usbtransfer utfr;	/* USB transfer information	*/
	struct	usb_devreq *dvrq;	/* USB device request		*/
	static	uint16 nextaddr = 1;	/* Next address to be assigned	*/

	usbdptr = (struct usbdcblk *)devtab[devid].dvcsr;

	if(usbdptr->state > USBD_STATE_DFLT) {
		return SYSERR;
	}

	dvrq = (struct usb_devreq *)getmem(sizeof(*dvrq));

	dvrq->reqtype = 0;
	dvrq->request = USB_DVRQ_SET_ADDR;
	dvrq->value = nextaddr;
	dvrq->index = 0;
	dvrq->length = 0;

	utfr.usbdptr = usbdptr;
	utfr.eptype = USB_TFR_EP_CTRL;
	utfr.ep = 0;
	utfr.dirin = TRUE;
	utfr.dvrq = dvrq;
	utfr.buffer = NULL;
	utfr.size = 0;

	control(USB, USB_CTRL_TRANSFER, (int32)&utfr, 0);

	usbdptr->address = nextaddr++;
}
