/* usb.c - usb_init, usb_new_device */

#include <xinu.h>

/*------------------------------------------------------------------------
 * usb_init  -  Initialize the USB system
 *------------------------------------------------------------------------
 */
void	usb_init (void) {

	ehci_scan();
}

/*------------------------------------------------------------------------
 * usb_new_device  -  Handle a new USB device attachment
 *------------------------------------------------------------------------
 */
void	usb_new_device (
		byte	hcitype,	/* Host controller type		*/
		did32	hcidev,		/* Host controller device	*/
		int32	port		/* Port number			*/
		)
{
	struct	usbdcblk *usbdptr;	/* USB device control block	*/
	int32	desclen;		/* Descriptor length		*/
	int32	i;			/* Loop index variable		*/

	for(i = 0; i < NUSBD; i++) {

		if(usbdtab[i].state == USBD_STATE_FREE) {
			break;
		}
	}
	if(i >= NUSBD) {
		kprintf("usb_new_device: no free 'USBD' devices\n");
		return;
	}

	usbdptr = &usbdtab[i];
	usbdptr->hcitype = hcitype;
	usbdptr->hcidev = hcidev;
	usbdptr->address = 0;
	usbdptr->state = USBD_STATE_PWRD;
	usbdptr->devdesc = NULL;
	usbdptr->cfgdesc = NULL;
	usbdptr->nep = 0;

	switch(hcitype) {

	  case USBD_HCI_EHCI:
		  ehci_port_reset(&ehcitab[devtab[hcidev].dvminor], port);
		  break;

	  default:
		  return;
	}

	sleepms(50);

	desclen = sizeof(struct usb_devdesc);
	usbdptr->devdesc = (struct usb_devdesc *)getmem(desclen);
	usb_get_dev_desc(usbdptr->devid, (char *)usbdptr->devdesc, desclen);

	usb_set_address(usbdptr->devid);
	kprintf("usb_new_device: address set to %d\n", usbdptr->address);

}
