/*  main.c  - main */

#include <xinu.h>

struct	usbdcblk usbdtab[1];

process	main(void)
{
	struct	ehcicblk *ehciptr;
	struct	ehcitransfer tfr;
	struct	usb_devreq req;
	struct	usb_devdesc desc;

	ehciptr = &ehcitab[0];

	ehciptr->opptr->portsc[0] |= EHCI_PSC_PR;
	sleepms(100);
	ehciptr->opptr->portsc[0] &= ~EHCI_PSC_PR;
	sleepms(100);

	kprintf("portsc[0]: %08x\n", ehciptr->opptr->portsc[0]);
	kprintf("portsc[1]: %08x\n", ehciptr->opptr->portsc[1]);
/*
	req.reqtype = 0x80;
	req.request = USB_DVRQ_GET_DESC;
	req.value = 0x0100;
	req.index = 0;
	req.length = sizeof(desc);

	tfr.devaddr = 0;
	tfr.ep = 0;
	tfr.eptype = 0;
	tfr.dirin = 1;
	tfr.ctrlbuffer = &req;
	tfr.buffer = &desc;
	tfr.size = sizeof(desc);

	memset(&desc, 0, sizeof(desc));

	control(EHCI, EHCI_CTRL_TRANSFER, (int32)&tfr, 0);

	int32	i;
	for(i = 0; i < sizeof(desc); i++) {
		kprintf("%02x ", *((byte *)&desc + i));
	}
	kprintf("\n");

	req.reqtype = 0;
	req.request = 5;
	req.value = 10;
	req.index = 0;
	req.length = 0;

	struct	ehci_qhd *qhd;
	struct	ehci_qtd *qtd1, *qtd2;

	qhd = getmem(32  +sizeof(*qhd));
	qhd = (struct ehci_qhd *)(((uint32)qhd + 31) & (~31));

	qtd1 = getmem(32 + sizeof(*qtd1));
	qtd1 = (struct ehci_qtd *)(((uint32)qtd1 + 31) & (~31));

	qtd2 = getmem(32 + sizeof(*qtd2));
	qtd2 = (struct ehci_qtd *)(((uint32)qtd2 + 31) & (~31));

	memset(qhd, 0, sizeof(*qhd));
	qhd->qhlp = (uint32)qhd | EHCI_QHD_QHLP_TYP_QH;
	qhd->devaddr = 0;
	qhd->eps = EHCI_QHD_EPS_HIGH;
	qhd->dtc = 1;
	qhd->H = 1;
	qhd->maxlen = 64;
	qhd->qtd.next = (uint32)qtd1;

	memset(qtd1, 0, sizeof(*qtd1));
	qtd1->next = (uint32)qtd2;
	qtd1->status = 0x80;
	qtd1->pid = 2;
	qtd1->cpage = 0;
	qtd1->size = sizeof(req);
	qtd1->dt = 0;
	qtd1->buffer[0]= (uint32)&req;
	qtd1->buffer[1] = ((uint32)&req & 0xFFFFF000) + 0x1000;

	memset(qtd2, 0, sizeof(*qtd2));
	qtd2->next = 1;
	qtd2->status = 0x80;
	qtd2->pid = 1;
	qtd2->dt = 1;
	qtd2->ioc = 1;
	qtd2->size = 0;

	qhd->_next = qtd1;
	qtd1->_next = qtd2;
	qtd2->_next = NULL;
*/
/*
	sleep(1);

	ehciptr->nused = 1;
	ehciptr->lastqh = qhd;

	ehciptr->opptr->asyncbase = (uint32)qhd;

	ehciptr->opptr->usbcmd |= EHCI_USBCMD_ASE;

	sleep(1);

	kprintf("qtd1 sts: %x\n", qtd1->status);
	kprintf("qtd2 sts: %x\n", qtd2->status);
*/

	//struct	usb_cfgdesc cdesc;
/*
	req.reqtype = 0;
	req.request = USB_DVRQ_SET_ADDR;
	req.value = 10;
	req.index = 0;
	req.length = 0;

	tfr.devaddr = 0;
	tfr.ep = 0;
	tfr.eptype = 0;
	tfr.dirin = 1;
	tfr.ctrlbuffer = &req;
	tfr.buffer = NULL;
	tfr.size = 0;

	//memset(&cdesc, 0, sizeof(cdesc));

	control(EHCI, EHCI_CTRL_TRANSFER, (int32)&tfr, 0);
*/
/*
	sleep(1);

	req.reqtype = 0x80;
	req.request = USB_DVRQ_GET_DESC;
	req.value = 0x0100;
	req.index = 0;
	req.length = sizeof(desc);

	tfr.devaddr = 10;
	tfr.ep = 0;
	tfr.eptype = 0;
	tfr.dirin = 1;
	tfr.ctrlbuffer = &req;
	tfr.buffer = &desc;
	tfr.size = sizeof(desc);

	memset(&desc, 0, sizeof(desc));

	control(EHCI, EHCI_CTRL_TRANSFER, (int32)&tfr, 0);

	for(i = 0; i < sizeof(desc); i++) {
		kprintf("%02x ", *((byte *)&desc + i));
	};
	kprintf("\n");
*/

	usbdtab[0].hcitype = 0;
	usbdtab[0].hcidev = EHCI;
	usbdtab[0].address = 0;
	usbdtab[0].state = USBD_STATE_DFLT;

	devtab[USBD0].dvcsr = &usbdtab[0];

	usb_get_dev_desc(USBD0, &desc, sizeof(desc));
	kprintf("Vendor ID: %04x\n", desc.vendid);
	kprintf("Product ID: %04x\n", desc.prodid);

	usb_set_address(USBD0);
	kprintf("Address assigned: %d\n", usbdtab[0].address);
/*
	memset(&desc, 0, sizeof(desc));

	kprintf("calling usb_get_dev_desc..\n");
	usb_get_dev_desc(USBD0, &desc, sizeof(desc));

	for(i = 0; i < sizeof(desc); i++) {
		kprintf("%02x ", *((byte *)&desc + i));
	}
	kprintf("\n");
*/
	char	buf[100];
	struct	usb_cfgdesc *cdesc;

	usb_get_cfg_desc(USBD0, 0, buf, sizeof(*cdesc));

	cdesc = (struct usb_cfgdesc *)buf;
	kprintf("Cfg desc: total length %d\n", cdesc->totallen);

	usb_get_cfg_desc(USBD0, 0, buf, cdesc->totallen);

	byte	*ptr;

	ptr = (byte *)(cdesc+1);

	while(ptr < (byte *)cdesc + cdesc->totallen) {

		if(*(ptr+1) == 4) {
			kprintf("Interface:\n");
			kprintf("\tnum: %d\n", ((struct usb_intfdesc *)ptr)->intfnum);
			kprintf("\t#ep: %d\n", ((struct usb_intfdesc *)ptr)->numep);
		}
		else if(*(ptr+1) == 5) {
			kprintf("Endpoint:\n");
			kprintf("\tep addr: %02x\n", ((struct usb_epdesc *)ptr)->epaddr);
			kprintf("\tattr: %02x\n", ((struct usb_epdesc *)ptr)->attr);
			kprintf("\tmaxpktsize: %d\n", ((struct usb_epdesc *)ptr)->maxpktsize);
		}

		ptr += *ptr;
	}

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
