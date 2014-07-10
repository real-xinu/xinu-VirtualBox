/* quark_eth.h */

/* Definitions for Intel Quark Ethernet */

#define INTEL_QUARK_ETH_PCI_DID	0x0937		/* MAC PCI Device ID	*/
#define INTEL_QUARK_ETH_PCI_VID	0x8086		/* MAC PCI Vendor ID	*/

struct quark_eth_csreg {
	uint32	maccr;		/* MAC Configuration Register		*/
	uint32	macff;		/* MAC Frame Filter Register		*/
	uint32	hthr;		/* Hash Table High Register		*/
	uint32	htlr;		/* Hash Table Low Register		*/
	uint32	gmiiar;		/* GMII Address Register		*/
	uint32	gmiidr;		/* GMII Data Register			*/
	uint32	fcr;		/* Flow Control Register		*/
	uint32	vlantag;	/* VLAV Tag Register			*/
	uint32	version;	/* Version Register			*/
	uint32	debug;		/* Debug Register			*/
	uint32	res1[4];	/* Skipped Addresses			*/
	uint32	ir;		/* Interrupt Register			*/
	uint32	imr;		/* Interrupt Mask Register		*/
	uint32	macaddr0h;	/* MAC Address0 High Register		*/
	uint32	macaddr0l;	/* MAC Address0 Low Register		*/
	uint32  res2[46];
	uint32  mmccr;		/* MAC Management Counter Cntl Register	*/
	uint32  mmcrvcir;	/* MMC Receive Interrupt Register	*/
	uint32  mmctxir;	/* MMC Transmit Interrupt Register	*/
	uint32	res3[957];	/* Skipped Addresses			*/
	uint32	bmr;		/* Bus Mode Register			*/
	uint32	tpdr;		/* Transmit Poll Demand Register	*/
	uint32	rpdr;		/* Receive Poll Demand Register		*/
	uint32	rdla;		/* Receive Descriptor List Addr		*/
	uint32	tdla;		/* Transmit Descriptor List Addr	*/
	uint32	sr;		/* Status Register			*/
	uint32	omr;		/* Operation Mode Register		*/
	uint32	ier;		/* Interrupt Enable Register		*/
};

/* Individual Bits in Control and Status Registers */

/* MAC Configuration Register	*/

#define QUARK_ETH_MACCR_PE2K	0x08000000	/* Enable 2K Packets	*/
#define QUARK_ETH_MACCR_WD	0x00800000	/* Watchdog Disable	*/
#define QUARK_ETH_MACCR_JD	0x00400000	/* Jabber Disable	*/
#define QUARK_ETH_MACCR_JE	0x00100000	/* Jumbo Frame Enable	*/

/* Inter-frame gap values */
#define QUARK_ETH_MACCR_IFG96	0x00000000	/* 96 bit times		*/
#define QUARK_ETH_MACCR_IFG88	0x00020000	/* 88 bit times		*/
#define QUARK_ETH_MACCR_IFG80	0x00040000	/* 80 bit times		*/
#define QUARK_ETH_MACCR_IFG40	0x000E0000	/* 40 bit times		*/
#define QUARK_ETH_MACCR_IFG64	0x00080000	/* 64 bit times		*/

#define QUARK_ETH_MACCR_DCRS	0x00010000	/* Dis. C. Sense dur TX	*/
#define QUARK_ETH_MACCR_RMIISPD10  0x00000000	/* RMII Speed = 10 Mbps	*/
#define QUARK_ETH_MACCR_RMIISPD100 0x00004000	/* RMII Speed = 100 Mbps*/
#define QUARK_ETH_MACCR_DO	0x00002000	/* Disable Receive Own	*/
#define QUARK_ETH_MACCR_LM	0x00001000	/* Loopback Mode Enable	*/
#define QUARK_ETH_MACCR_DM	0x00000800	/* Duplex Mode Enable	*/
#define QUARK_ETH_MACCR_IPC	0x00000400	/* Checksum Offload	*/
#define QUARK_ETH_MACCR_DR	0x00000200	/* Disable Retry	*/
#define QUARK_ETH_MACCR_ACS	0x00000080	/* Auto Pad or CRC Strip*/
#define QUARK_ETH_MACCR_DC	0x00000010	/* Deferral Check	*/
#define QUARK_ETH_MACCR_TE	0x00000008	/* Transmitter Enable	*/
#define QUARK_ETH_MACCR_RE	0x00000004	/* Receiver Enable	*/
#define QUARK_ETH_MACCR_PRELEN7	0x00000000	/* Preamble = 7 bytes	*/
#define QUARK_ETH_MACCR_PRELEN5	0x00000001	/* Preamble = 5 bytes	*/
#define QUARK_ETH_MACCR_PRELEN3	0x00000002	/* Preamble = 3 bytes	*/

#define QUARK_ETH_MMC_CNTFREEZ 0x00000008   /* Freeze MMC counter values*/
#define QUARK_ETH_MMC_CNTRST   0x00000001   /* Reset all cntrs to zero	*/

/* GMII Address Register	*/
#define QUARK_ETH_GMIIAR_PAMASK	0x0000F800	/* Phys Layer Addr Mask	*/
#define QUARK_ETH_GMIIAR_GRMASK	0x000007C0	/* GMII Register Mask	*/
#define QUARK_ETH_GMIIAR_CR	0x00000004	/* Clk Range = 100-150	*/
						/*    MHz for Quark	*/
#define QUARK_ETH_GMIIAR_GW	0x00000002	/* GMII Write Enable	*/
#define QUARK_ETH_GMIIAR_GB	0x00000001	/* GMII Busy		*/

#define QUARK_ETH_SR_MMCI	0x08000000	/* MAC MMC interrupt	*/
#define QUARK_ETH_SR_TS_SUSP	0x00600000	/* TX DMA is suspended	*/
#define QUARK_ETH_SR_NIS	0x00010000	/* Normal Int summary	*/
#define QUARK_ETH_SR_AIS	0x00008000	/* Abnorm Intrupt summ.	*/
#define QUARK_ETH_SR_RI		0x00000040	/* Receive Interrupt	*/
#define QUARK_ETH_SR_TI		0x00000001	/* Transmit Interrupt	*/

#define QUARK_ETH_OMR_ST	0x00002000	/* Start/Stop TX	*/
#define QUARK_ETH_OMR_SR	0x00000002	/* Start/Stop RX	*/

#define QUARK_ETH_IER_NIE	0x00010000	/* Enable Norm Int Summ.*/
#define QUARK_ETH_IER_AIE	0x00008000	/* Enable Abnnom "   "	*/
#define QUARK_ETH_IER_RIE	0x00000040	/* Enable RX Interrupt	*/
#define QUARK_ETH_IER_TIE	0x00000001	/* Enable TX Interrupt	*/

struct quark_eth_tx_desc {
	uint32	tdes[4];
};

#define QUARK_ETH_TDES0_OWN	0x80000000	/* Descrip. owned by DMA*/
#define QUARK_ETH_TDES0_IC	0x40000000	/* Int on Completion	*/
#define QUARK_ETH_TDES0_LS	0x20000000	/* Last Segment		*/
#define QUARK_ETH_TDES0_FS	0x10000000	/* First Segment	*/
#define QUARK_ETH_TDES0_TER	0x00200000	/* Transmit End of Ring	*/
#define QUARK_ETH_TDES0_ES	0x00008000	/* Error Summary	*/

struct quark_eth_rx_desc {
	uint32	rdes[4];
};

#define QUARK_ETH_RDES0_OWN	0x80000000	/* Descrip. owned by DMA*/
#define QUARK_ETH_RDES0_ES	0x00008000	/* Error Summary	*/
#define QUARK_ETH_RDES0_FS	0x00000200	/* First Segment	*/
#define QUARK_ETH_RDES0_LS	0x00000100	/* Last segment		*/
#define QUARK_ETH_RDES0_FTETH	0x00000020	/* Frame Type = Ethernet*/

#define QUARK_ETH_RDES1_DIC	0x80000000	/* Dis. Int on Complet.	*/
#define QUARK_ETH_RDES1_RER	0x00008000	/* Recv End of Ring	*/

#define QUARK_ETH_RX_RING_SIZE	32
#define QUARK_ETH_TX_RING_SIZE	16

#define QUARK_ETH_INIT_DELAY	100000		/* Delay in micro secs	*/
#define QUARK_ETH_MAX_RETRIES	3		/* Max retries for init	*/
