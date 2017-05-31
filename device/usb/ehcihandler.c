/* ehcihandler.c - ehcihandler */

#include <xinu.h>

/*------------------------------------------------------------------------
 * ehcihandler  -  Handle interrupts for the EHCI controller
 *------------------------------------------------------------------------
 */
interrupt ehcihandler (void) {

	struct	ehcicblk *ehciptr;	/* EHCI control block	*/
	struct	ehci_qhd *curr;		/* Queue head pointer	*/
	struct	ehci_qtd *qtd;		/* Queue transfer desc	*/
	uint32	status;			/* Interrupt status	*/
	int32	i;			/* For loop index	*/
	bool8	retired;		/* Retired flag		*/

	ehciptr = &ehcitab[0];

	kprintf("EHCI interrupt: %08x\n", ehciptr->opptr->usbsts);

	status = ehciptr->opptr->usbsts;

	if(status & EHCI_USBSTS_INT || status & 0x2) {

		ehciptr->opptr->usbsts = EHCI_USBSTS_INT;

		curr = (struct ehci_qhd *)((uint32)ehciptr->lastqh->qhlp &
							0xFFFFFFE0);

		kprintf("ehcihandler: nused: %d\n", ehciptr->nused);
		ehciptr->nfree = 0;
		for(i = 0; i < ehciptr->nused; i++) {

			retired = TRUE;
			qtd = (struct ehci_qtd *)curr->_next;

			while(qtd != NULL) {

				kprintf("\tqtd sts: %x\n", qtd->status);
				if(qtd->status & EHCI_QTD_STS_ACT) {
					retired = FALSE;
					break;
				}

				qtd = (struct ehci_qtd *)qtd->_next;
			}

			if(retired == FALSE) {
				break;
			}

			//kprintf("ehcihandler: QH %08x retired\n", curr);
			ehciptr->freeq[ehciptr->nfree++] = curr;
			curr = (struct ehci_qhd *)((uint32)curr->qhlp & 0xFFFFFFE0);
		}

		kprintf("\tethhandler: completed: %d\n", ehciptr->nfree);

		ehciptr->nused -= ehciptr->nfree;

		if(ehciptr->nused <= 0) {
			ehciptr->nused = 0;
			ehciptr->opptr->usbcmd &= ~EHCI_USBCMD_ASE;
			for(i = 0; i < ehciptr->nfree; i++) {
				send(ehciptr->freeq[i]->_pid, OK);
			}
		}
		else {
			ehciptr->lastqh->qhlp = (uint32)curr | EHCI_QHD_QHLP_TYP_QH;
			for(i = 0; i < ehciptr->nfree; i++) {
				ehciptr->freeq[i]->qhlp = (uint32)curr | EHCI_QHD_QHLP_TYP_QH;
			}

			ehciptr->opptr->usbcmd |= EHCI_USBCMD_IAAD;
		}
	}

	if(status & EHCI_USBSTS_IAA) {
		kprintf("ehcihandler: doorbell\n");

		ehciptr->opptr->usbsts = EHCI_USBSTS_IAA;

		for(i = 0; i < ehciptr->nfree; i++) {
			send(ehciptr->freeq[i]->_pid, OK);
		}
	}
}
