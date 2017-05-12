/* ethread.c - ethread */

#include <xinu.h>

/*------------------------------------------------------------------------
 * ethread - read a packet from a VirtIO Network device
 *------------------------------------------------------------------------
 */
devcall	ethread(
	struct	dentry	*devptr,	/* entry in device switch table	*/
	void	*buf,			/* buffer to hold packet	*/
	uint32	len			/* length of buffer		*/
	)
{
	struct 	ethcblk	*ethptr; 	/* ptr to entry in ethertab	*/
	struct	virtio_cblk *csrptr;	/* ptr to Virtio control block	*/
	int32	next_id;		/* next index in avail. ring	*/
	int32	d;			/* descriptor index		*/

	ethptr = &ethertab[devptr->dvminor];

	csrptr = (struct virtio_cblk *)ethptr->csr;

	if ((ETH_STATE_UP != ethptr->state)
			|| (len < ETH_HDR_LEN)) {
		return SYSERR;
	}

	/* Wait for a packet to arrive */

	wait(ethptr->isem);

	/* Get the next descriptor index from RX ring */

	d = ((uint16 *)ethptr->rxRing)[ethptr->rxHead++];
	if(ethptr->rxHead >= ethptr->rxRingSize) {
		ethptr->rxHead = 0;
	}

	/* Compute the length to copy */

	if(len > csrptr->queue[0].desc[d].len) {
		len = csrptr->queue[0].desc[d].len;
	}

	/* Copy the packet in the provided buffer */

	memcpy(buf, (byte *)csrptr->queue[0].desc[d].addr +
				sizeof(struct virtio_net_hdr), len);

	/* Compute next index in available ring */

	next_id = csrptr->queue[0].avail->idx %
				csrptr->queue[0].queue_size;

	/* Add the descriptor to the available ring */

	csrptr->queue[0].desc[d].flags = VIRTQ_DESC_F_WRITE;
	csrptr->queue[0].avail->ring[next_id] = d;
	csrptr->queue[0].avail->idx += 1;

	outw((int32)&(csrptr->csr->qnotify), 0);

	return len;
}
