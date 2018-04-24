/* ethhandler.c - ethhandler */

#include <xinu.h>

/*------------------------------------------------------------------------
 * eth_rxPackets - handler for receiver interrupts
 *------------------------------------------------------------------------
 */
local 	void 	eth_rxPackets(
	struct 	ethcblk	*ethptr 	/* ptr to control block		*/
	)
{
	struct	eth_rx_desc *descptr;/* ptr to ring descriptor 	*/
	uint32	tail;			/* pos to insert next packet	*/
	uint32	status;			/* status of ring descriptor 	*/
	int numdesc; 			/* num. of descriptor reclaimed	*/

	intmask mask = xsec_beg(ethptr->ethlock);	/* multicore protection */

	for (numdesc = 0; numdesc < ethptr->rxRingSize; numdesc++) {

		/* Insert new arrived packet to the tail */

		tail = ethptr->rxTail;
		descptr = (struct eth_rx_desc *)ethptr->rxRing + tail;
		status = descptr->status;

		if (status == 0) {
			break;
		}

		ethptr->rxTail 
			= (ethptr->rxTail + 1) % ethptr->rxRingSize;
	}

	signaln(ethptr->isem, numdesc);

	xsec_end(mask, ethptr->ethlock);

	return;
}

/*------------------------------------------------------------------------
 * eth_txPackets - handler for transmitter interrupts
 *------------------------------------------------------------------------
 */
local 	void 	eth_txPackets(
	struct	ethcblk	*ethptr		/* ptr to control block		*/
	)
{
	struct	eth_tx_desc *descptr;/* ptr to ring descriptor 	*/
	uint32 	head; 			/* pos to reclaim descriptor	*/
	char 	*pktptr; 		/* ptr used during packet copy  */
	int 	numdesc; 		/* num. of descriptor reclaimed	*/

	/* protect parallel access to eth control block */
	intmask mask = xsec_beg(ethptr->ethlock);	

	for (numdesc = 0; numdesc < ethptr->txRingSize; numdesc++) {
		head = ethptr->txHead;
		descptr = (struct eth_tx_desc *)ethptr->txRing + head;

		if (!(descptr->upper.data & E1000_TXD_STAT_DD))
			break;

		/* Clear the write-back descriptor and buffer */

		descptr->lower.data = 0;
		descptr->upper.data = 0;
		pktptr = (char *)((uint32)(descptr->buffer_addr &
					   ADDR_BIT_MASK));
		memset(pktptr, '\0', ETH_BUF_SIZE);

		ethptr->txHead 
			= (ethptr->txHead + 1) % ethptr->txRingSize;
	}

	signaln(ethptr->osem, numdesc);

	xsec_end(mask, ethptr->ethlock);
	return;
}


/*------------------------------------------------------------------------
 * ethhandler - decode and handle interrupt from an E1000 device
 *------------------------------------------------------------------------
 */
interrupt ethhandler(void)
{
	uint32	status;
	struct  dentry  *devptr;        /* address of device control blk*/
	struct 	ethcblk	*ethptr;	/* ptr to control block		*/
	intmask mask;

	/* Initialize structure pointers */

	devptr = (struct dentry *) &devtab[ETHER0];
	
	/* Obtain a pointer to the tty control block */

	ethptr = &ethertab[devptr->dvminor];

	/* protect parallel access to eth control block */
	
	mask = xsec_beg(ethptr->ethlock);

	/* Invoke the device-specific interrupt handler */

	/* Disable device interrupt */

	ethIrqDisable(ethptr);
	
	/* Obtain status bits from device */

	status = eth_io_readl(ethptr->iobase, E1000_ICR);

	/* Not our interrupt */

	if (status == 0) {
		ethIrqEnable(ethptr);
		return;
	}


	if (status & E1000_ICR_LSC) {
	}

	if (status & E1000_ICR_RXT0) {
		ethptr->rxIrq++;
		eth_rxPackets(ethptr);
	}

	if (status & E1000_ICR_TXDW) {
		ethptr->txIrq++;
		eth_txPackets(ethptr);
	}

	/* Enable device interrupt */

	ethIrqEnable(ethptr);
	
	xsec_end(mask, ethptr->ethlock);

	return;
}
