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
#define USBEP0               6	/* type usbep    */
#define USBEP1               7	/* type usbep    */
#define USBEP2               8	/* type usbep    */
#define USBEP3               9	/* type usbep    */
#define USBEP4              10	/* type usbep    */
#define USBEP5              11	/* type usbep    */
#define USBEP6              12	/* type usbep    */
#define USBEP7              13	/* type usbep    */
#define USBEP8              14	/* type usbep    */
#define USBEP9              15	/* type usbep    */
#define USBEP10             16	/* type usbep    */
#define USBEP11             17	/* type usbep    */
#define USBEP12             18	/* type usbep    */
#define USBEP13             19	/* type usbep    */
#define USBEP14             20	/* type usbep    */
#define USBEP15             21	/* type usbep    */
#define NAMESPACE           22	/* type nam      */
#define RDISK               23	/* type rds      */
#define RAM0                24	/* type ram      */
#define RFILESYS            25	/* type rfs      */
#define RFILE0              26	/* type rfl      */
#define RFILE1              27	/* type rfl      */
#define RFILE2              28	/* type rfl      */
#define RFILE3              29	/* type rfl      */
#define RFILE4              30	/* type rfl      */
#define RFILE5              31	/* type rfl      */
#define RFILE6              32	/* type rfl      */
#define RFILE7              33	/* type rfl      */
#define RFILE8              34	/* type rfl      */
#define RFILE9              35	/* type rfl      */
#define LFILESYS            36	/* type lfs      */
#define LFILE0              37	/* type lfl      */
#define LFILE1              38	/* type lfl      */
#define LFILE2              39	/* type lfl      */
#define LFILE3              40	/* type lfl      */
#define LFILE4              41	/* type lfl      */
#define LFILE5              42	/* type lfl      */

/* Control block sizes */

#define	Nnull	1
#define	Ntty	1
#define	Neth	1
#define	Nehci	1
#define	Nusb	1
#define	Nusbd	1
#define	Nusbep	16
#define	Nrds	1
#define	Nram	1
#define	Nrfs	1
#define	Nrfl	10
#define	Nlfs	1
#define	Nlfl	6
#define	Nnam	1

#define NDEVS 43


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

