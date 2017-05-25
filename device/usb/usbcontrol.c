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
	      case USB_TFR_EP_BULK:
		etfr.eptype = EHCI_TFR_EP_BULK;
	      case USB_TFR_EP_INTR:
	        etfr.eptype = EHCI_TFR_EP_INTR;
	      case USB_TFR_EP_ISO:
		etfr.eptype = EHCI_TFR_EP_ISO;
	      default:
		return SYSERR;
	    }

	    if(utfr->eptype == USB_TFR_EP_CTRL) {
		    etfr.ctrlbuffer = (char *)utfr->dvrq;
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
