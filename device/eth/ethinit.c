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
	struct	ethcblk *ethptr;
	struct	virtio_cblk *csrptr;
	int32	dinfo;			/* device information		*/
	uint32	pci_value;
	int32	memsize;
	int32	qsize;
	void	*qaddr;
	byte	dstat;
	int32	i, j;

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

	/* Acknowledge that we know about the device and can drive it */

	outb((int32)&(csrptr->csr->dev_status), VIRTIO_DEV_ST_ACK);
	outb((int32)&(csrptr->csr->dev_status),
			(VIRTIO_DEV_ST_ACK | VIRTIO_DEV_ST_DRV));

	/* Read the device features */

	csrptr->dev_features = inl((int32)&(csrptr->csr->dev_features));

	/* Check if the device can provide a MAC Address */

	if(!(csrptr->dev_features & VIRTIO_NET_F_MAC)) {
		kprintf("VirtIO Net error: Device does not provide a MAC\n");
		return SYSERR;
	}

	/* Read the MAC Address */

	kprintf("MAC Address: ");
	for(i = 0; i < ETH_ADDR_LEN; i++) {
		ethptr->devAddress[i] = inb((int32)&(csrptr->csr->mac[i]));
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

	csrptr->nqueues = VIRTIO_NET_NQUEUE;

	for(i = 0; i < csrptr->nqueues; i++) {

		outw(&(csrptr->csr->qselect), i);

		qsize = inw(&(csrptr->csr->qsize));

		csrptr->queue[i].queue_size = qsize;

		kprintf("ethinit: Queue %d size %d\n", i, qsize);
		memsize = VIRTIO_ALIGN((qsize * sizeof(struct virtq_desc)) +
				       (3 * sizeof(uint16)) +
				       (qsize * sizeof(uint16))) +
			  VIRTIO_ALIGN((3 * sizeof(uint16)) +
				       (qsize * sizeof(struct virtq_used_elem))) +
			  4096;

		qaddr = getmem(memsize);
		memset(qaddr, 0, memsize);

		qaddr = (void *)(((uint32)qaddr + 4095) & (~4095));

		csrptr->queue[i].desc = (struct virtq_desc *)qaddr;
		csrptr->queue[i].avail = (struct virtq_avail *)((uint32)qaddr +
					(qsize * sizeof(struct virtq_desc)));
		csrptr->queue[i].used = (struct virtq_used *)((uint32)csrptr->queue[i].avail +
				((3 + qsize) * sizeof(uint16)));
		csrptr->queue[i].used = ((uint32)csrptr->queue[i].used + 4095) & (~4095);

		kprintf("ethinit: queue %d desc %x avail %x used %x\n", i, csrptr->queue[i].desc,
									   csrptr->queue[i].avail,
									   csrptr->queue[i].used);
		outl((int32)&(csrptr->csr->qaddr), ((uint32)qaddr/4096));

		csrptr->queue[i].used->flags = 0;

		if(i == 0) { /* This is the RX Queue */

			//ethptr->rxRingSize = (VIRTIO_NET_RX_RING_SIZE > qsize) ? qsize :
			//							 VIRTIO_NET_RX_RING_SIZE;
			ethptr->rxRingSize = qsize;
			ethptr->rxRing = getmem(qsize * sizeof(uint16));
			ethptr->rxHead = ethptr->rxTail = 0;

			for(j = 0; j < ethptr->rxRingSize; j++) {
				memsize = sizeof(struct virtio_net_hdr) + sizeof(struct netpacket);
				csrptr->queue[i].desc[j].addr = (uint64)getmem(memsize);
				csrptr->queue[i].desc[j].len = memsize;
				csrptr->queue[i].desc[j].flags = VIRTQ_DESC_F_WRITE;

				csrptr->queue[i].avail->ring[j] = j;
			}

			ethptr->isem = semcreate(0);
		}
		else if(i == 1) { /* This is the TX Queue */

			//ethptr->txRingSize = (VIRTIO_NET_TX_RING_SIZE > qsize) ? qsize :
			//							 VIRTIO_NET_TX_RING_SIZE;
			ethptr->txRingSize = qsize;

			ethptr->txRing = getmem(qsize * sizeof(uint16));
			ethptr->txTail = ethptr->txHead = 0;
			ethptr->osem = semcreate(qsize);

			for(j = 0; j < ethptr->txRingSize; j++) {
				memsize = sizeof(struct virtio_net_hdr) + sizeof(struct netpacket);
				csrptr->queue[i].desc[j].addr = (uint16)getmem(memsize);
				csrptr->queue[i].desc[j].len = memsize;

				((uint16 *)ethptr->txRing)[j] = j;
			}
		}
	}

	/* Notify the device that we are ready to drive it */

	dstat = inb((int32)&(csrptr->csr->dev_status));
	outb((int32)&(csrptr->csr->dev_status), dstat | VIRTIO_DEV_ST_DRVOK);
	dstat = inb((int32)&(csrptr->csr->dev_status));
	kprintf("Device status: %08x\n", dstat);

	/* Make all the RX descriptors available */

	kprintf("ethinit: RX Queue used flags %x\n", csrptr->queue[0].used->flags);
	csrptr->queue[0].avail->idx += ethptr->rxRingSize;
	kprintf("ethinit: RX Queue used flags %x\n", csrptr->queue[0].used->flags);
	outw((int32)&(csrptr->csr->qnotify), 0);

	pci_value = 0;
	pci_bios_read_config_byte(ethptr->pcidev, 0x3C, &pci_value);
	kprintf("ethinit: Interrupt line: %d\n", pci_value);

	set_evec(IRQBASE+pci_value, ethdispatch);

	/* Mark Ethernet interface active */

	ethptr->state = ETH_STATE_UP;

	return OK;
}
