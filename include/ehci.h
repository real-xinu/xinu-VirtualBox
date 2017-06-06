/* ehci.h - USB EHCI related definitions */

#pragma pack(1)

#define	EHCI_HCSP_NPORTS	0x00000007
#define	EHCI_HCSP_PPC		0x00000008
#define	EHCI_HCSP_PRR		0x00000080
#define	EHCI_HCSP_NPCC		0x00000F00
#define	EHCI_HCSP_NCC		0x0000F000
#define	EHCI_HCSP_PIND		0x00010000
#define	EHCI_HCSP_DBGPRT	0x00F00000

#define	EHCI_HCCP_64B		0x00000001
#define	EHCI_HCCP_PFLF		0x00000002
#define	EHCI_HCCP_ASPC		0x00000004
#define	EHCI_HCCP_IST		0x000000F0
#define	EHCI_HCCP_EECP		0x00000F00

struct	ehci_capreg {
	byte	caplen;		/* Capability length	*/
	byte	res;		/* Reserved		*/
	uint16	hcivers;	/* HCI Version		*/
	uint32	hcsp;		/* Structural parameters*/
	uint32	hccp;		/* Capability parameters*/
	byte	portrrte[];	/* Port route descrip.	*/
};

#define	EHCI_USBCMD_RS		0x00000001
#define	EHCI_USBCMD_HCR		0x00000002
#define	EHCI_USBCMD_FLS1024	0x00000000
#define	EHCI_USBCMD_FLS512	0x00000004
#define	EHCI_USBCMD_FLS256	0x00000008
#define	EHCI_USBCMD_PSE		0x00000010
#define	EHCI_USBCMD_ASE		0x00000020
#define	EHCI_USBCMD_IAAD	0x00000040
#define	EHCI_USBCMD_LHCR	0x00000080
#define	EHCI_USBCMD_ASPMC	0x00000300
#define	EHCI_USBCMD_ASPME	0x00000800
#define	EHCI_USBCMD_ITC1	0x00010000
#define	EHCI_USBCMD_ITC2	0x00020000
#define	EHCI_USBCMD_ITC4	0x00040000
#define	EHCI_USBCMD_ITC8	0x00080000
#define	EHCI_USBCMD_ITC16	0x00100000
#define	EHCI_USBCMD_ITC32	0x00200000
#define	EHCI_USBCMD_ITC64	0x00400000

#define	EHCI_USBSTS_INT		0x00000001
#define	EHCI_USBSTS_ERRINT	0x00000002
#define	EHCI_USBSTS_PCD		0x00000004
#define	EHCI_USBSTS_FLR		0x00000008
#define	EHCI_USBSTS_HSE		0x00000010
#define	EHCI_USBSTS_IAA		0x00000020
#define	EHCI_USBSTS_HCH		0x00001000
#define	EHCI_USBSTS_REC		0x00002000
#define	EHCI_USBSTS_PSS		0x00004000
#define	EHCI_USBSTS_ASS		0x00008000

#define	EHCI_USBINTR_IE		0x00000001
#define	EHCI_USBINTR_EIE	0x00000002
#define	EHCI_USBINTR_PCIE	0x00000004
#define	EHCI_USBINTR_FLRE	0x00000008
#define	EHCI_USBINTR_HSEE	0x00000010
#define	EHCI_USBINTR_IAAE	0x00000020

#define	EHCI_PSC_CCS		0x00000001
#define	EHCI_PSC_CSC		0x00000002
#define	EHCI_PSC_PED		0x00000004
#define	EHCI_PSC_PEDC		0x00000008
#define	EHCI_PSC_OCA		0x00000010
#define	EHCI_PSC_OCC		0x00000020
#define	EHCI_PSC_FPR		0x00000040
#define	EHCI_PSC_SUSP		0x00000080
#define	EHCI_PSC_PR		0x00000100
#define	EHCI_PSC_LS		0x00000C00
#define	EHCI_PSC_LS_SE0		0x00000000
#define	EHCI_PSC_LS_K		0x00000400
#define	EHCI_PSC_LS_J		0x00000800
#define	EHCI_PSC_PP		0x00001000
#define	EHCI_PSC_PO		0x00002000
#define	EHCI_PSC_WCE		0x00100000
#define	EHCI_PSC_WDE		0x00200000
#define	EHCI_PSC_WOCE		0x00400000

struct	ehci_opreg {
	uint32	usbcmd;		/* USB Command	register*/
	uint32	usbsts;		/* USB Status register	*/
	uint32	usbintr;	/* USB Intr. Enable reg	*/
	uint32	frindex;	/* USB Frame index reg	*/
	uint32	ctrldsseg;	/* 4G Segment selector	*/
	uint32	perbase;	/* Periodic list addr	*/
	uint32	asyncbase;	/* Async list addr	*/
	uint32	res[9];		/* Reserved		*/
	uint32	configflag;	/* Configured flag reg	*/
	uint32	portsc[];	/* Port status, ctl reg	*/
};

#define	EHCI_QTD_NEXT_T		0x00000001

#define	EHCI_QTD_STS_PERR	0x01
#define	EHCI_QTD_STS_STS	0x02
#define	EHCI_QTD_STS_MMF	0x04
#define	EHCI_QTD_STS_TERR	0x08
#define	EHCI_QTD_STS_BD		0x10
#define	EHCI_QTD_STS_DBERR	0x20
#define	EHCI_QTD_STS_HLT	0x40
#define	EHCI_QTD_STS_ACT	0x80

#define	EHCI_QTD_PID_OUT	0
#define	EHCI_QTD_PID_IN		1
#define	EHCI_QTD_PID_SETUP	2

struct	ehci_qtd {
	uint32	next;		/* Next qTD pointer	*/
	uint32	altnext;	/* Alternative next ptr	*/
	struct {
	  uint32 status:8;	/* Status		*/
	  uint32 pid:2;		/* Transfer PID		*/
	  uint32 cerr:2;	/* Error counter	*/
	  uint32 cpage:3;	/* Current page		*/
	  uint32 ioc:1;		/* Intr. on completion	*/
	  uint32 size:15;	/* Size of transfer	*/
	  uint32 dt:1;		/* Data toggle		*/
	};
	uint32	buffer[5];	/* Buffer pointers	*/
	void	*_next;		/* Next QTD		*/
	void	*_start;	/* Start address	*/
};

#define	EHCI_QHD_QHLP_T		0x00000001
#define	EHCI_QHD_QHLP_TYP_ITD	0x00000000
#define	EHCI_QHD_QHLP_TYP_QH	0x00000002
#define	EHCI_QHD_QHLP_TYP_SITD	0x00000004
#define	EHCI_QHD_QHLP_TYP_FSTN	0x00000006

#define	EHCI_QHD_EPS_FULL	0
#define	EHCI_QHD_EPS_LOW	1
#define	EHCI_QHD_EPS_HIGH	2

struct	ehci_qhd {
	uint32	qhlp;		/* Horizontal pointer	*/
	struct {
	  uint32 devaddr:7;	/* Device address	*/
	  uint32 I:1;		/* Inactive on next tran*/
	  uint32 endpt:4;	/* Endpoint number	*/
	  uint32 eps:2;		/* Endpoint speed	*/
	  uint32 dtc:1;		/* Data toggle control	*/
	  uint32 H:1;		/* Head of recl. list	*/
	  uint32 maxlen:11;	/* Max packet length	*/
	  uint32 C:1;		/* Control endpoint flag*/
	  uint32 RL:4;		/* Nak count reload	*/
	};
	struct {
	  byte	 ufrsmask;	/* Intr. schedule mask	*/
	  byte	 ufrcmask;	/* Split completion mask*/
	  uint16 hubaddr:7;	/* Hub address		*/
	  uint16 portnum:7;	/* Port number		*/
	  uint16 mult:2;
	};
	uint32	currqtd;
	struct	ehci_qtd qtd;
	void	*_next;		/* Next QTD		*/
	void	*_start;	/* Start address	*/
	pid32	_pid;		/* PID of waiting proc	*/
};

#pragma pack()

#define	NEHCI	1

#define	EHCI_FREEQ_SIZ		100000

struct	ehcicblk {
	did32	devid	;		/* Index in device table	*/
	int32	pcidev;			/* PCI device handle		*/
	volatile struct	ehci_capreg *cpptr;/* Capability registers	*/
	volatile struct	ehci_opreg *opptr;/* Operational registers	*/
	struct	ehci_qhd *lastqh;	/* Last Queue head in Async list*/
	struct	ehci_qhd *freeq[EHCI_FREEQ_SIZ];
					/* Queue of to-be-free QHs	*/
	int32	nfree;			/* No. of elements in freeq	*/
	int32	nused;			/* No. of QHs being used	*/
};

extern	struct ehcicblk ehcitab[];

#define	EHCI_TFR_EP_CTRL	0
#define	EHCI_TFR_EP_BULK	1
#define	EHCI_TFR_EP_INTR	2
#define	EHCI_TFR_EP_ISO		3

/* Structure of EHCI transfer information */

struct	ehcitransfer {
	byte	devaddr;	/* USB Device address	*/
	byte	ep;		/* If not control EP	*/
	byte	eptype;		/* Endpoint type	*/
	bool8	dirin;		/* EP direction		*/
	char	*ctrlbuffer;	/* USB Device Request	*/
	char	*buffer;	/* Data buffer		*/
	int32	size;		/* Data buffer size	*/
};

/* Functions in ehcicontrol */

#define	EHCI_CTRL_TRANSFER	0

#define	EHCI_TRANSFER_TIMEOUT	1000
