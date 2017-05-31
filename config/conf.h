/* conf.h (GENERATED FILE; DO NOT EDIT) */

/* Device switch table declarations */

/* Device table entry */
struct	dentry	{
	int32   dvnum;
	int32   dvminor;
	char    *dvname;
	devcall (*dvinit) (struct dentry *);
	devcall (*dvopen) (struct dentry *, char *, char *);
	devcall (*dvclose)(struct dentry *);
	devcall (*dvread) (struct dentry *, void *, uint32);
	devcall (*dvwrite)(struct dentry *, void *, uint32);
	devcall (*dvseek) (struct dentry *, int32);
	devcall (*dvgetc) (struct dentry *);
	devcall (*dvputc) (struct dentry *, char);
	devcall (*dvcntl) (struct dentry *, int32, int32, int32);
	void    *dvcsr;
	void    (*dvintr)(void);
	byte    dvirq;
};

extern	struct	dentry	devtab[]; /* one entry per device */

/* Device name definitions */

#define CONSOLE              0	/* type tty      */
#define NULLDEV              1	/* type null     */
#define ETHER0               2	/* type eth      */
#define EHCI                 3	/* type ehci     */
#define USB                  4	/* type usb      */
#define USBD0                5	/* type usbd     */
#define USBD1                6	/* type usbd     */
#define USBD2                7	/* type usbd     */
#define USBD3                8	/* type usbd     */
#define USBD4                9	/* type usbd     */
#define NAMESPACE           10	/* type nam      */
#define RDISK               11	/* type rds      */
#define RAM0                12	/* type ram      */
#define RFILESYS            13	/* type rfs      */
#define RFILE0              14	/* type rfl      */
#define RFILE1              15	/* type rfl      */
#define RFILE2              16	/* type rfl      */
#define RFILE3              17	/* type rfl      */
#define RFILE4              18	/* type rfl      */
#define RFILE5              19	/* type rfl      */
#define RFILE6              20	/* type rfl      */
#define RFILE7              21	/* type rfl      */
#define RFILE8              22	/* type rfl      */
#define RFILE9              23	/* type rfl      */
#define LFILESYS            24	/* type lfs      */
#define LFILE0              25	/* type lfl      */
#define LFILE1              26	/* type lfl      */
#define LFILE2              27	/* type lfl      */
#define LFILE3              28	/* type lfl      */
#define LFILE4              29	/* type lfl      */
#define LFILE5              30	/* type lfl      */

/* Control block sizes */

#define	Nnull	1
#define	Ntty	1
#define	Neth	1
#define	Nehci	1
#define	Nusb	1
#define	Nusbd	5
#define	Nrds	1
#define	Nram	1
#define	Nrfs	1
#define	Nrfl	10
#define	Nlfs	1
#define	Nlfl	6
#define	Nnam	1

#define NDEVS 31


/* Configuration and Size Constants */

#define	NPROC	     100	/* number of user processes		*/
#define	NSEM	     100	/* number of semaphores			*/
#define	IRQBASE	     32		/* base ivec for IRQ0			*/
#define	IRQ_TIMER    IRQ_HW5	/* timer IRQ is wired to hardware 5	*/
#define	IRQ_ATH_MISC IRQ_HW4	/* Misc. IRQ is wired to hardware 4	*/
#define CLKFREQ      200000000	/* 200 MHz clock			*/

#define	LF_DISK_DEV	RAM0

#ifndef	ETHER0
#define	ETHER0	0
#endif

