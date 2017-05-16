/* ethhandler.c - ethhandler */

#include <xinu.h>


/*------------------------------------------------------------------------
 * ethhandler - interrupt handler for VirtIO Network device
 *------------------------------------------------------------------------
 */
interrupt ethhandler(void)
{
	struct  dentry  *devptr;        /* Address of device control blk*/
	struct 	ethcblk	*ethptr;	/* Ptr to control block		*/
	struct	virtio_cblk *csrptr;	/* Ptr to virtio control block	*/
	volatile struct	virtq_desc *descptr;/* Virtual queue descriptor	*/
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

		/* No. of packets arrived */

		count = csrptr->queue[0].used->idx -
				csrptr->queue[0].used_idx;

		/* First descriptor to be processed */

		j = csrptr->queue[0].used_idx %
			csrptr->queue[0].queue_size;

		scount = 0;
		for(i = 0; i < count; i++) {

			/* Get the descritor index */

			d = csrptr->queue[0].used->ring[j++].id;
			if(j >= csrptr->queue[0].queue_size) {
				j = 0;
			}

			if(semcount(ethptr->isem) <= 0 && i == 0) {
					/* Arrived packet queue empty */

				ethptr->rxHead = ethptr->rxTail = d;
			}
			else if(semcount(ethptr->isem) < 
					(int32)ethptr->rxRingSize/2) {

				/* Insert desc. into queue */

				csrptr->queue[0].desc[ethptr->txTail].next = d;
				ethptr->txTail = d;
			}
			else if(semcount(ethptr->isem) >=
					(int32)ethptr->rxRingSize/2) {
						/* Queue full */

				/* Put the desc. in the avail. ring */

				descptr = &csrptr->queue[0].desc[d];
				descptr->len = 10;
				descptr->flags = VIRTQ_DESC_F_WRITE |
						 VIRTQ_DESC_F_NEXT;
				descptr->next = d + 1;

				descptr = &csrptr->queue[0].desc[d+1];
				descptr->len = 1514;
				descptr->flags = VIRTQ_DESC_F_WRITE;

				next_idx = csrptr->queue[0].avail->idx %
					   csrptr->queue[0].queue_size;
				csrptr->queue[0].avail->ring[next_idx] = d;
				csrptr->queue[0].avail->idx += 1;
				continue;
			}

			scount++;
		}

		csrptr->queue[0].used_idx = csrptr->queue[0].used->idx;

		signaln(ethptr->isem, scount);
	}

	if(csrptr->queue[1].used->idx != csrptr->queue[1].used_idx) {
						/* TX Queue interrupt	*/

		/* No. of desc. chains to be processed */

		count = csrptr->queue[1].used->idx -
				csrptr->queue[1].used_idx;

		/* First chain to be processed */

		j = csrptr->queue[1].used_idx %
			csrptr->queue[1].queue_size;

		for(i = 0; i < count; i++) {

			/* Get the descriptor indexx */

			d = csrptr->queue[1].used->ring[j++].id;
			if(j >= csrptr->queue[1].queue_size) {
				j = 0;
			}

			if(semcount(ethptr->osem) <= 0 && i == 0) {
					/* Unused desc queue empty */

				ethptr->txHead = ethptr->txTail = d;
			}
			else {
				/* Insert desc. into unused queue */
				csrptr->queue[1].desc[ethptr->txTail].next = d;
				ethptr->txTail = d;
			}
		}

		csrptr->queue[1].used_idx = csrptr->queue[1].used->idx;

		signaln(ethptr->osem, count);
	}
}
