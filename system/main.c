/*  main.c  - main */

#include <xinu.h>

process	main(void)
{
	struct	ehcicblk *ehciptr;
	struct	usb_devreq req;
	struct	ehcitransfer tfr;
	char	buffer[100] = {0};
	int32	nports;
	int32	i;

	req.reqtype = 0x80;
	req.request = 6;
	req.value = 0x0100;
	req.index = 0;
	req.length = sizeof(struct usb_devdesc);

	tfr.devaddr = 0;
	tfr.ep = 0;
	tfr.eptype = 0;
	tfr.dirin = 1;
	tfr.ctrlbuffer = (char *)&req;
	tfr.buffer = buffer;
	tfr.size = sizeof(struct usb_devdesc);

	ehciptr = &ehcitab[0];

	kprintf("EHCI cpptr %08x opptr %08x\n", ehciptr->cpptr, ehciptr->opptr);

	nports = ehciptr->cpptr->hcsp & EHCI_HCSP_NPORTS;

	int32	found = -1;
	for(i = 0; i < nports; i++) {
		kprintf("portsc[%d]: %08x\n", i, ehciptr->opptr->portsc[i]);
		if(ehciptr->opptr->portsc[i] & 1) {
			found = i;
		}
	}

	if(found == -1) {
		return OK;
	}

	ehciptr->opptr->portsc[found] |= EHCI_PSC_PR;

	sleepms(100);

	ehciptr->opptr->portsc[found] &= ~EHCI_PSC_PR;

	sleepms(10);

	kprintf("portsc: %08x\n", ehciptr->opptr->portsc[found]);

	control(EHCI, EHCI_CTRL_TRANSFER, (int32)&tfr, 0);

	struct	ehcitransfer tfr1;
	char	buffer1[100] = {0};

	tfr1.devaddr = 0;
	tfr1.ep = 0;
	tfr1.eptype = 0;
	tfr1.dirin = 1;
	tfr1.ctrlbuffer = (char *)&req;
	tfr1.buffer = buffer1;
	tfr1.size = sizeof(struct usb_devdesc);

	for(i = 0; i < 100; i++) {

		control(EHCI, EHCI_CTRL_TRANSFER, (int32)&tfr1, 0);
	}

	ehciptr->opptr->usbcmd |= EHCI_USBCMD_ASE;

	sleep(1);

	struct	ehci_qhd *qh;
	struct	ehci_qtd *qt;

	qh = ehciptr->opptr->asyncbase;

	qt = qh->_next;

	kprintf("QT status %02x\n", qt->status);

	for(i = 0; i < sizeof(struct usb_devdesc); i++) {
		kprintf("%02x ", *((byte *)buffer + i));
	}
	kprintf("\n");

	/* Run the Xinu shell */

	recvclr();
	resume(create(shell, 8192, 50, "shell", 1, CONSOLE));

	/* Wait for shell to exit and recreate it */

	while (TRUE) {
		receive();
		sleepms(200);
		kprintf("\n\nMain process recreating shell\n\n");
		resume(create(shell, 4096, 20, "shell", 1, CONSOLE));
	}
	return OK;
    
}
