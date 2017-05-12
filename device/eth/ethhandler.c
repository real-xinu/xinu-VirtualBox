/* ethhandler.c - ethhandler */

#include <xinu.h>


/*------------------------------------------------------------------------
 * ethhandler - interrupt handler for VirtIO Network device
 *------------------------------------------------------------------------
 */
interrupt ethhandler(void)
{
	uint32	status;
	struct  dentry  *devptr;        /* address of device control blk*/
	struct 	ethcblk	*ethptr;	/* ptr to control block		*/
	struct	virtio_cblk *csrptr;	/* ptr to virtio control block	*/
	int32	next_idx;		/* Next index in avail. ring	*/
	int32	count;			/* Descriptor count		*/
	int32	scount;			/* Signal count for RX interrupt*/
	int32	d;			/* Descriptor index		*/
	byte	isr;			/* Interrupt status		*/
	int32	i, j;			/* Index variables		*/

	/* Initialize structure pointers */

	devptr = (struct dentry *) &devtab[ETHER0];

	/* Obtain a pointer to the Ethernet control block */

	ethptr = &ethertab[devptr->dvminor];

	/* Obtain a pointer to the VirtIO control block */

	csrptr = (struct virtio_cblk *)ethptr->csr;

	/* Read the interrupt status register */

	isr = inb((int32)&(csrptr->csr->isr));

	//kprintf("ethhandler: isr %02x\n", isr);

	if(!(isr & 0x01)) { /* There is no queue interrupt, ignore */
		return;
	}

	if(csrptr->queue[0].used->idx != csrptr->queue[0].used_idx) {
						/* RX Queue interrupt 	*/
		//kprintf("ethhandler: RX Q interrupt\n");

		count = csrptr->queue[0].used->idx -
				csrptr->queue[0].used_idx;

		j = csrptr->queue[0].used_idx % csrptr->queue[0].queue_size;

		scount = 0;
		for(i = 0; i < count; i++) {

			d = csrptr->queue[0].used->ring[j++].id;
			if(j >= csrptr->queue[0].queue_size) {
				j = 0;
			}

			//kprintf("ethhandler: RX desc %d\n", d);

			//kprintf("ethhandler RX semcount %d ring size %d\n", semcount(ethptr->isem), ethptr->rxRingSize);
			if(semcount(ethptr->isem) >= (int32)ethptr->rxRingSize) {
				csrptr->queue[0].desc[d].len = sizeof(struct virtio_net_hdr) +
							       sizeof(struct netpacket);

				next_idx = csrptr->queue[0].avail->idx %
							csrptr->queue[0].queue_size;
				csrptr->queue[0].avail->ring[next_idx] = d;
				csrptr->queue[0].avail->idx += 1;
				continue;
			}

			((uint16 *)ethptr->rxRing)[ethptr->rxTail++] = d;
			if(ethptr->rxTail >= ethptr->rxRingSize) {
				ethptr->rxTail = 0;
			}
			scount++;
		}

		csrptr->queue[0].used_idx = csrptr->queue[0].used->idx;

		//kprintf("ethhandler: rx signal %d\n", scount);
		signaln(ethptr->isem, scount);
	}

	if(csrptr->queue[1].used->idx != csrptr->queue[1].used_idx) {
						/* TX Queue interrupt	*/
		//kprintf("ethhandler: TX Q interrupt\n");

		count = csrptr->queue[1].used->idx -
				csrptr->queue[1].used_idx;

		j = csrptr->queue[1].used_idx % csrptr->queue[1].queue_size;

		for(i = 0; i < count; i++) {

			d = csrptr->queue[1].used->ring[j++].id;
			if(j >= csrptr->queue[1].queue_size) {
				j = 0;
			}

			//kprintf("ethhandler: used descriptor %d\n", d);

			((uint16 *)ethptr->txRing)[ethptr->txTail++] = d;
			if(ethptr->txTail >= ethptr->txRingSize) {
				ethptr->txTail = 0;
			}
		}

		csrptr->queue[1].used_idx = csrptr->queue[1].used->idx;

		signaln(ethptr->osem, count);
	}
}
