/* ethread.c - ethread */

#include <xinu.h>

/*------------------------------------------------------------------------
 * ethread - read a packet from a VirtIO Network device
 *------------------------------------------------------------------------
 */
devcall	ethread(
	struct	dentry	*devptr,	/* Entry in device switch table	*/
	void	*buf,			/* Buffer to hold packet	*/
	uint32	len			/* Length of buffer		*/
	)
{
	struct 	ethcblk	*ethptr; 	/* Ptr to entry in ethertab	*/
	struct	virtio_cblk *csrptr;	/* Ptr to Virtio control block	*/
	int32	next_id;		/* Next index in avail. ring	*/
	int32	d;			/* Descriptor index		*/

	ethptr = &ethertab[devptr->dvminor];

	csrptr = (struct virtio_cblk *)ethptr->csr;

	if ((ETH_STATE_UP != ethptr->state)
			|| (len < ETH_HDR_LEN)) {
		return SYSERR;
	}

	/* Wait for a packet to arrive */

	wait(ethptr->isem);

	/* Get the next descriptor index from RX Used ring */

	d = ethptr->rxHead;
	ethptr->rxHead = csrptr->queue[0].desc[d].next;

	/* Compute the length to copy */

	if(len > csrptr->queue[0].desc[d+1].len) {
		len = csrptr->queue[0].desc[d+1].len;
	}

	/* Copy the packet in the provided buffer */

	memcpy(buf, (char *)
			((uint32)csrptr->queue[0].desc[d+1].addr), len);

	/* Compute next index in available ring */

	next_id = csrptr->queue[0].avail->idx %
				csrptr->queue[0].queue_size;

	/* Add the descriptor to the available ring */

	csrptr->queue[0].desc[d].flags = VIRTQ_DESC_F_WRITE |
					 VIRTQ_DESC_F_NEXT;
	csrptr->queue[0].desc[d].next = d + 1;

	csrptr->queue[0].desc[d+1].len = 1514;
	csrptr->queue[0].desc[d+1].flags = VIRTQ_DESC_F_WRITE;

	csrptr->queue[0].avail->ring[next_id] = d;

	__sync_synchronize();

	csrptr->queue[0].avail->idx += 1;

	outw((int32)&(csrptr->csr->qnotify), 0);

	return len;
}
