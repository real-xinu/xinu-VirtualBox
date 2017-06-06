/* ehciinit.c - ehciinit */

#include <xinu.h>

struct	ehcicblk ehcitab[NEHCI];

/*------------------------------------------------------------------------
 * ehciinit  -  Initialize the EHCI device
 *------------------------------------------------------------------------
 */
devcall	ehciinit (
		struct	dentry *devptr	/* Device entry pointer	*/
		)
{
	struct	ehcicblk *ehciptr;	/* EHCI control block	*/
	uint32	value;			/* Value read from PCI	*/
	uint32	*perbase;		/* Periodic list address*/
	int32	i;			/* Loop index variable	*/

	ehciptr = (struct ehcicblk *)&ehcitab[devptr->dvminor];

	ehciptr->devid = devptr->dvnum;

	ehciptr->pcidev = find_pci_device(0x24CD, 0x8086, 0);

	if(ehciptr->pcidev == SYSERR) {
		return SYSERR;
	}

	kprintf("Found EHCI controller\n");
	pci_bios_read_config_dword(ehciptr->pcidev, 0x10, &value);

	ehciptr->cpptr = (struct ehci_capreg *)(value & 0xFFFFFF00);
	ehciptr->opptr = (struct ehci_opreg *)((byte *)ehciptr->cpptr +
						ehciptr->cpptr->caplen);

	/* Reset the EHCI device */

	ehciptr->opptr->usbcmd |= EHCI_USBCMD_HCR;
	while(ehciptr->opptr->usbcmd & EHCI_USBCMD_HCR);

	/* Set the data segment selecter to 0 */

	ehciptr->opptr->ctrldsseg = 0;

	/* Disable all interrupts for now */

	ehciptr->opptr->usbintr = EHCI_USBINTR_IE |
				  EHCI_USBINTR_IAAE |
				  EHCI_USBINTR_EIE;

	/* Allocate memory for periodic list */

	perbase = (uint32 *)getmem(4096 + (sizeof(uint32) * 1024));
	ehciptr->opptr->perbase = (((uint32)perbase + 4096) & (~4096));

	for(i = 0; i < 1024; i++) {
		perbase[i] = 0x00000001;
	}

	value = 0;
	pci_bios_read_config_byte(ehciptr->pcidev, 0x3C, (byte *)&value);

	set_evec(IRQBASE + value, ehcidispatch);

	/* Start the EHCI controller */

	ehciptr->opptr->usbcmd |= EHCI_USBCMD_RS;

	/* Route all ports to EHCI */

	ehciptr->opptr->configflag = 1;

	return OK;
}
