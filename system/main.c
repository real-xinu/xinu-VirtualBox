/*  main.c  - main */

#include <xinu.h>

struct	usbdcblk usbdtab[2];

#pragma pack(1)
struct	umscbw {
	uint32	sig;
	uint32	tag;
	uint32	tlen;
	byte	flag;
	byte	lun;
	byte	cblen;
	byte	cb[16];
};

struct	umscbs {
	uint32	sig;
	uint32	tag;
	uint32	datares;
	byte	status;
};
#pragma pack()

process	main(void)
{
	int32	i;

	for(i = 0; i < 2; i++) {
		kprintf("portsc[%d]: %08x\n", i, ehcitab[0].opptr->portsc[i]);
	}

	for(i = 0; i < sizeof(struct usb_devdesc); i++) {
		kprintf("%02x ", *((byte *)usbdtab[0].devdesc + i));
	}
	kprintf("\n");

	for(i = 0; i < sizeof(struct usb_devdesc); i++) {
		kprintf("%02x ", *((byte *)usbdtab[1].devdesc + i));
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
