/* ethwrite.c - ethwrite */

#include <xinu.h>

/*------------------------------------------------------------------------
 * ethwrite - write a packet to a VirtIO Network device
 *------------------------------------------------------------------------
 */
devcall	ethwrite(
	struct	dentry	*devptr, 	/* Entry in device switch table	*/
	void	*buf,			/* Buffer that holds a packet	*/
	uint32	len			/* Length of buffer		*/
	)
{
	struct	ethcblk	*ethptr; 	/* Ptr to entry in ethertab 	*/
	struct	virtio_cblk *csrptr;	/* VirtIO control block		*/
	uint16	next_idx;		/* Next index in avail. ring	*/
	int32	i;			/* Descriptor index		*/

	ethptr = &ethertab[devptr->dvminor];

	csrptr = (struct virtio_cblk *)ethptr->csr;

	/* Verify Ethernet interface is up and arguments are valid */

	if ((ETH_STATE_UP != ethptr->state)
			|| (len < ETH_HDR_LEN)
			|| (len > ETH_MAX_PKT_LEN) ) {
		return SYSERR;
	}

	/* Wait until there is an available descriptor */

	wait(ethptr->osem);

	/* Get the index of next descriptor */

	i = ethptr->txHead;
	ethptr->txHead = csrptr->queue[1].desc[i].next;

	/* Copy the packet into the descriptor buffer */

	memset((char *)((uint32)csrptr->queue[1].desc[i].addr), 0, 10);
	csrptr->queue[1].desc[i].len = 10;
	memcpy((char *)((uint32)csrptr->queue[1].desc[i+1].addr), 
							(char *)buf, len);
	csrptr->queue[1].desc[i+1].len = len;

	csrptr->queue[1].desc[i].flags = VIRTQ_DESC_F_NEXT;
	csrptr->queue[1].desc[i].next = i + 1;

	/* Insert the descriptor in the available ring */

	next_idx = csrptr->queue[1].avail->idx %
				csrptr->queue[1].queue_size;
	csrptr->queue[1].avail->ring[next_idx] = i;

	/* Make sure all memory operations are completed */

	__sync_synchronize();

	/* Make the descriptor available to the device */

	csrptr->queue[1].avail_idx = csrptr->queue[1].avail->idx;
	csrptr->queue[1].avail->idx += 1;

	/* Notify the device about change in the queue */

	outw((int32)&(csrptr->csr->qnotify), 1);

	return len;
}
