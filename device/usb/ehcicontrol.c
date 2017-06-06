/* ehcicontrol.c - ehcicontrol */

#include <xinu.h>

/*------------------------------------------------------------------------
 * ehcicontrol  -  Control functions for EHCI device
 *------------------------------------------------------------------------
 */
devcall	ehcicontrol (
		struct	dentry *devptr,	/* Entry in device table	*/
		int32	func,		/* Control function		*/
		int32	arg1,		/* Argument 1 for the function	*/
		int32	arg2		/* Argument 2 for the function	*/
		)
{
	struct	ehcicblk *ehciptr;	/* EHCI control block	*/
	struct	ehcitransfer *tfrptr;	/* EHCI transfer info	*/

	ehciptr = &ehcitab[devptr->dvminor];

	switch(func) {

	  case EHCI_CTRL_TRANSFER:

	    tfrptr = (struct ehcitransfer *)arg1;
	    ehcitransfer(ehciptr, tfrptr);
	    break;

	  default:

	    return SYSERR;
	}

	return OK;
}

/*------------------------------------------------------------------------
 * ehcitransfer  -  Handle a USB transfer over the EHCI controller
 *------------------------------------------------------------------------
 */
status	ehcitransfer (
		struct	ehcicblk *ehciptr,	/* EHCI control block	*/
		struct	ehcitransfer *tfrptr	/* EHCI transfer info	*/
		)
{
	struct	ehci_qhd *qhd;			/* Queue head		*/
	struct	ehci_qtd *qtd1, *qtd2, *qtd3;	/* Queue transfer desc.	*/
	void	*tmp;				/* Temporary ptr	*/
	int32	retval;				/* Return value		*/
	int32	i;				/* Loop index variable	*/

	/* Allocate memory for queue head and transfer desc. */

	tmp = getmem(32 + sizeof(*qhd));
	qhd = (struct ehci_qhd *)(((uint32)tmp + 31) & (~31));
	qhd->_start = tmp;

	tmp = getmem(32 + sizeof(*qtd1));
	qtd1 = (struct ehci_qtd *)(((uint32)tmp + 31) & (~31));
	qtd1->_start = tmp;

	tmp = getmem(32 + sizeof(*qtd2));
	qtd2 = (struct ehci_qtd *)(((uint32)tmp + 31) & (~31));
	qtd2->_start = tmp;

	/* Initialize the queue head */

	memset(qhd, 0, sizeof(*qhd));
	qhd->devaddr = tfrptr->devaddr;
	qhd->endpt = tfrptr->ep;
	qhd->eps = EHCI_QHD_EPS_HIGH;
	qhd->dtc = 1;
	qhd->maxlen = 64;
	qhd->mult = 1;
	qhd->qtd.next = (uint32)qtd1;

	memset(qtd1, 0, sizeof(*qtd1));
	qtd1->status = EHCI_QTD_STS_ACT;

	qtd2 = qtd3 = NULL;

	if(tfrptr->eptype == EHCI_TFR_EP_CTRL) {

		qtd1->next = (uint32)qtd2;
		qtd1->pid = EHCI_QTD_PID_SETUP;
		qtd1->cpage = 0;
		qtd1->size = sizeof(struct usb_devreq);
		qtd1->dt = 0;
		qtd1->buffer[0] = (uint32)tfrptr->ctrlbuffer;
		for(i = 0; i < 4; i++) {
			qtd1->buffer[i+1] = (qtd1->buffer[i] & 0xFFFFF000) +
								0x1000;
		}

		memset(qtd2, 0, sizeof(*qtd2));
		qtd2->next = 0x00000001;
		qtd2->status = EHCI_QTD_STS_ACT;
		qtd2->pid = tfrptr->dirin ? EHCI_QTD_PID_IN :
					    EHCI_QTD_PID_OUT;
		qtd2->cpage = 0;
		qtd2->size = tfrptr->size;
		qtd2->dt = 1;
		qtd2->buffer[0] = (uint32)tfrptr->buffer;
		for(i = 0; i < 4; i++) {
			qtd2->buffer[i+1] = (qtd2->buffer[i] & 0xFFFFF000) +
								0x1000;
		}

		//kprintf("ehcicontrol: setup size: %d\n", qtd1->size);
		//kprintf("ehcicontrol: data/status size: %d\n", qtd2->size);

		qhd->_next = qtd1;
		qtd1->_next = qtd2;
		qtd2->_next = NULL;

		if(tfrptr->size > 0) {

			tmp = (struct ehci_qtd *)getmem(32 + sizeof(*qtd3));
			qtd3 = (struct ehci_qtd *)(((uint32)tmp + 31) & (~31));
			qtd3->_start = tmp;

			qtd2->next = (uint32)qtd3;
			memset(qtd3, 0, sizeof(*qtd3));
			qtd3->next = 0x00000001;
			qtd3->status = EHCI_QTD_STS_ACT;
			qtd3->pid = tfrptr->dirin ? EHCI_QTD_PID_OUT :
						    EHCI_QTD_PID_IN;
			qtd3->cpage = 0;
			qtd3->ioc = 1;
			qtd3->size = 0;
			qtd3->dt = 1;

			//kprintf("ehcicontrol: status size: 0\n");
			qtd2->_next = qtd3;
			qtd3->_next = NULL;
		}
		else {
			qtd2->ioc = 1;
			qtd2->pid = EHCI_QTD_PID_IN;
			qtd2->size = 0;
		}
	}
	else if(tfrptr->eptype == EHCI_TFR_EP_BULK) {

		//kprintf("ehcicontrol: bulk transfer..\n");
		qtd1->next = 0x1;
		qtd1->altnext = 0x1;
		qtd1->pid = tfrptr->dirin ? EHCI_QTD_PID_IN :
					    EHCI_QTD_PID_OUT;
		qtd1->ioc = 1;
		qtd1->size = tfrptr->size;
		//TODO
		qtd1->dt = 1;
		qtd1->buffer[0] = (uint32)tfrptr->buffer;
		for(i = 0; i < 4; i++) {
			qtd1->buffer[i+1] = ((uint32)qtd1->buffer[i] & 0xFFFFF000) + 0x1000;
		}
	}

	qhd->_pid = getpid();

	recvclr();

	ehciasync_add(ehciptr, qhd);

	retval = recvtime(EHCI_TRANSFER_TIMEOUT);

	freemem((char *)qhd, 32 + sizeof(*qhd));
	freemem((char *)qtd1, 32 + sizeof(*qtd1));

	if(qtd2) {
		freemem((char *)qtd2, 32 + sizeof(*qtd2));
	}

	if(qtd3) {
		freemem((char *)qtd3, 32 + sizeof(*qtd3));
	}

	return retval;
}

/*------------------------------------------------------------------------
 * ehciasync_add  -  Add a queue head to the Async. list
 *------------------------------------------------------------------------
 */
status	ehciasync_add (
		struct	ehcicblk *ehciptr,	/* EHCI control block	*/
		struct	ehci_qhd *qhd		/* Queue to add to list	*/
		)
{
	struct	ehci_qhd *first, *last;

	//if(!(ehciptr->opptr->usbsts & EHCI_USBSTS_ASS)) {
	if(ehciptr->nused <= 0) {

		qhd->qhlp = (uint32)qhd | EHCI_QHD_QHLP_TYP_QH;

		qhd->H = 1;

		ehciptr->lastqh = qhd;

		ehciptr->nused = 1;

		ehciptr->opptr->asyncbase = (uint32)qhd;

		ehciptr->opptr->usbcmd |= EHCI_USBCMD_ASE;

		return OK;
	}

	last = (struct ehci_qhd *)ehciptr->lastqh;
	first = (struct ehci_qhd *)((uint32)last->qhlp & 0xFFFFFFE0);

	qhd->qhlp = (uint32)first;
	qhd->qhlp |= EHCI_QHD_QHLP_TYP_QH;

	last->qhlp = (uint32)qhd | EHCI_QHD_QHLP_TYP_QH;

	ehciptr->nused++;
	ehciptr->lastqh = qhd;

	return OK;
}
