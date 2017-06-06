/* ehci.c - ehci_scan */

#include <xinu.h>

/*------------------------------------------------------------------------
 * ehci_port_reset  -  Reset an EHCI port
 *------------------------------------------------------------------------
 */
void	ehci_port_reset (
		struct	ehcicblk *ehciptr,	/* EHCI control block	*/
		int32	port			/* Port number		*/
		)
{
	uint32	portsc;		/* Register value	*/

	portsc = ehciptr->opptr->portsc[port];

	portsc &= ~EHCI_PSC_PED;
	portsc |= EHCI_PSC_PR;

	ehciptr->opptr->portsc[port]= portsc;

	sleepms(50);

	ehciptr->opptr->portsc[port] &= ~EHCI_PSC_PR;

	while(ehciptr->opptr->portsc[port] & EHCI_PSC_PR);
}

/*------------------------------------------------------------------------
 * ehci_scan  -  Scan all EHCI controller for connected devices
 *------------------------------------------------------------------------
 */
void	ehci_scan (void) {

	struct	ehcicblk *ehciptr;	/* EHCI control block	*/
	int32	nports;			/* No. of ports		*/
	int32	i, j;			/* Loop index variables	*/

	for(i = 0; i < NEHCI; i++) {

		ehciptr = &ehcitab[i];

		nports = ehciptr->cpptr->hcsp & 0x0000000F;

		for(j = 0; j < nports; j++) {

			if(ehciptr->opptr->portsc[j] & EHCI_PSC_CCS) {

				ehci_port_reset(ehciptr, j);

				if(ehciptr->opptr->portsc[j] & EHCI_PSC_PED) {

					//High speed USB device
					kprintf("ehci_scan: high speed device connected to port %d\n", j);
					usb_new_device(USBD_HCI_EHCI, ehciptr->devid, j);
				}
			}
		}
	}
}
