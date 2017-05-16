/* ethinit.c - ethinit */

#include <xinu.h>

#ifndef Neth
#define	Neth	1
#endif
struct	ethcblk	ethertab[Neth];		/* Ethernet control blocks 	*/

/*------------------------------------------------------------------------
 * ethinit - Initialize Ethernet device structures
 *------------------------------------------------------------------------
 */
devcall	ethinit (
	  struct dentry *devptr
	)
{
	struct	ethcblk *ethptr;	/* Ethernet control block	*/
	struct	virtio_cblk *csrptr;	/* VirtIO control block		*/
	volatile struct	virtq_desc *descptr;/* Virtq descriptor		*/
	int32	dinfo;			/* Device information		*/
	uint32	pci_value;		/* PCI config. space field value*/
	int32	memsize;		/* Allocated memory size	*/
	int32	qsize;			/* Virtual queue size		*/
	void	*qaddr;			/* Virtual queue address	*/
	byte	dstat;			/* Device status byte		*/
	byte	irqline;		/* Interrupt line		*/
	int32	i, j;			/* Index variables		*/

	/* Initialize structure pointers */

	ethptr = &ethertab[devptr->dvminor];
	
	memset(ethptr, '\0', sizeof(struct ethcblk));
	ethptr->dev = devptr;
	ethptr->state = ETH_STATE_DOWN;
	ethptr->mtu = ETH_MTU;
	ethptr->errors = 0;
	ethptr->addrLen = ETH_ADDR_LEN;
	ethptr->rxHead = ethptr->rxTail = 0;
	ethptr->txHead = ethptr->txTail = 0;

	dinfo = find_pci_device(VIRTIO_NET_PCI_DID,
				     VIRTIO_NET_PCI_VID, 0);
	if (dinfo == SYSERR) {
		kprintf("Could not find a VirtIO Ethernet device\n");
		return SYSERR;
	}
	kprintf("Found VirtIO Ethernet NIC\n");

	ethptr->pcidev = dinfo;

	ethptr->csr = getmem(sizeof(struct virtio_cblk) +
			(VIRTIO_NET_NQUEUE*sizeof(struct virtq)));

	csrptr = (struct virtio_cblk *)ethptr->csr;

	pci_bios_read_config_dword(ethptr->pcidev, 0x10, &pci_value);

	if(!(pci_value & 1)) {
		//TODO: For now, fail if BAR0 is memory mapped
		kprintf("VirtIO Net error: BAR0 is memory mapped\n");
		return SYSERR;
	}

	csrptr->csr = (struct virtio_csr *)(pci_value & 0xFFF0);

	outb((int32)&(csrptr->csr->dev_status), 0);

	/* Acknowledge that we know about the device and can drive it */

	outb((int32)&(csrptr->csr->dev_status), VIRTIO_DEV_ST_ACK);
	outb((int32)&(csrptr->csr->dev_status),
			(VIRTIO_DEV_ST_ACK | VIRTIO_DEV_ST_DRV));

	/* Read the device features */

	csrptr->dev_features = inl((int32)&(csrptr->csr->dev_features));

	/* Check if the device can provide a MAC Address */

	if(!(csrptr->dev_features & VIRTIO_NET_F_MAC)) {
		kprintf("VirtIO Net error: no MAC provided\n");
		return SYSERR;
	}

	/* Read the MAC Address */

	kprintf("MAC Address: ");
	for(i = 0; i < ETH_ADDR_LEN; i++) {
		ethptr->devAddress[i] =
				inb((int32)&(csrptr->csr->mac[i]));
		kprintf("%02X", ethptr->devAddress[i]);
		if(i < (ETH_ADDR_LEN-1)) {
			kprintf(":");
		}
	}
	kprintf("\n");

	/* Negotiate features with the device */

	csrptr->neg_features = csrptr->dev_features & VIRTIO_NET_F_MAC;

	outl((int32)&(csrptr->csr->drv_features), csrptr->neg_features);
	dstat = inb((int32)&(csrptr->csr->dev_status));
	outb((int32)&(csrptr->csr->dev_status), dstat | 0x08);

	/* Allocate memory for virtual queues */

	csrptr->nqueues = VIRTIO_NET_NQUEUE;

	for(i = 0; i < csrptr->nqueues; i++) {

		/* Select the queue # i */

		outw((int32)&(csrptr->csr->qselect), i);

		/* Read the queue size */

		qsize = inw((int32)&(csrptr->csr->qsize));
		csrptr->queue[i].queue_size = qsize;

		/* Compute required memory for the virtual queue */

		memsize = VIRTIO_ALIGN((qsize*sizeof(struct virtq_desc)) +
				       (3 * sizeof(uint16)) +
				       (qsize * sizeof(uint16))) +
			  VIRTIO_ALIGN((3 * sizeof(uint16)) +
				 (qsize*sizeof(struct virtq_used_elem))) +
			  4096;

		qaddr = getmem(memsize);
		memset(qaddr, 0, memsize);

		/* Align the virtual queue on a page boundary */

		qaddr = (void *)(((uint32)qaddr + 4095) & (~4095));

		csrptr->queue[i].desc = (struct virtq_desc *)qaddr;
		csrptr->queue[i].avail = (struct virtq_avail *)
					 ((uint32)qaddr +
				(qsize * sizeof(struct virtq_desc)));
		csrptr->queue[i].used = (struct virtq_used *)
					((uint32)csrptr->queue[i].avail +
					((3 + qsize) * sizeof(uint16)));
		csrptr->queue[i].used = (struct virtq_used *)
			(((uint32)csrptr->queue[i].used + 4095)&(~4095));

		/* Set the queue address in the device register */

		outl((int32)&(csrptr->csr->qaddr), ((uint32)qaddr/4096));

		csrptr->queue[i].used->flags = 0;

		if(i == 0) { /* This is the RX Queue */


			ethptr->rxRingSize = qsize;

			for(j = 0; j < ethptr->rxRingSize; j += 2) {

				/* Each even desc. is a packet header */

				descptr = &csrptr->queue[i].desc[j];
				descptr->addr = (uint64)((uint32)
				  getmem(sizeof(struct virtio_net_hdr)));
				descptr->len = 10;
				descptr->flags = VIRTQ_DESC_F_WRITE |
						 VIRTQ_DESC_F_NEXT;
				descptr->next = j + 1;

				/* Each odd desc. is a packet buffer */

				descptr = &csrptr->queue[i].desc[j+1];
				descptr->addr = (uint64)((uint32)
					getmem(sizeof(struct netpacket)));
				descptr->len = 1514;
				descptr->flags = VIRTQ_DESC_F_WRITE;
				descptr->next = 0;

				csrptr->queue[i].avail->ring[j/2] = j;
			}

			/* Create input semaphore */

			ethptr->isem = semcreate(0);

		}
		else if(i == 1) { /* This is the TX Queue */

			ethptr->txRingSize = qsize;

			for(j = 0; j < ethptr->txRingSize; j += 2) {

				/* Each even desc. is a packet header */

				descptr = &csrptr->queue[i].desc[j];
				descptr->addr = (uint64)((uint32)
				  getmem(sizeof(struct virtio_net_hdr)));
				descptr->len = 10;
				descptr->flags = 0;
				descptr->next = j + 2;

				/* Each odd desc. is a packet buffer */

				descptr = &csrptr->queue[i].desc[j+1];
				descptr->addr = (uint64)((uint32)
					getmem(sizeof(struct netpacket)));
				descptr->flags = 0;
			}

			/* All descriptors are unused */

			ethptr->txHead = 0;
			ethptr->txTail = qsize - 2;

			/* Create output semaphore */

			ethptr->osem = semcreate(qsize/2);
		}
	}

	/* Notify the device that we are ready to drive it */

	dstat = inb((int32)&(csrptr->csr->dev_status));
	outb((int32)&(csrptr->csr->dev_status),
			dstat | VIRTIO_DEV_ST_DRVOK);
	dstat = inb((int32)&(csrptr->csr->dev_status));

	/* Make all the RX descriptors available */

	__sync_synchronize();

	csrptr->queue[0].avail->idx += ethptr->rxRingSize/2;
	outw((int32)&(csrptr->csr->qnotify), 0);

	/* Set the interrupt handler */

	pci_bios_read_config_byte(ethptr->pcidev, 0x3C, &irqline);
	set_evec(IRQBASE+irqline, (uint32)ethdispatch);

	/* Mark Ethernet interface active */

	ethptr->state = ETH_STATE_UP;

	return OK;
}
