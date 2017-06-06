/* usb.h - USB related definitions */

#pragma pack(1)

#define	USB_DVRQ_RT_H2D		0x00
#define	USB_DVRQ_RT_D2H		0x80
#define	USB_DVRQ_RT_DEV		0x00
#define	USB_DVRQ_RT_INTF	0x01
#define	USB_DVRQ_RT_EP		0x00

#define	USB_DVRQ_GET_STS	0
#define	USB_DVRQ_CLR_FEAT	1
#define	USB_DVRQ_SET_FEAT	3
#define	USB_DVRQ_SET_ADDR	5
#define	USB_DVRQ_GET_DESC	6
#define	USB_DVRQ_SET_DESC	7
#define	USB_DVRQ_GET_CFG	8
#define	USB_DVRQ_SET_CFG	9
#define	USB_DVRQ_GET_INTF	10
#define	USB_DVRQ_SET_INTF	11
#define	USB_DVRQ_SYNC_FRM	12

struct	usb_devreq {
	byte	reqtype;	/* Request type		*/
	byte	request;	/* Reqeust		*/
	uint16	value;		/* Specific to request	*/
	uint16	index;		/* Specific to request	*/
	uint16	length;		/* #bytes in data stage	*/
};

struct	usb_devdesc {
	byte	length;		/* Size in bytes	*/
	byte	desctype;	/* Descriptor type	*/
	uint16	bcdusb;		/* USB Spec release	*/
	byte	devclass;	/* Device class		*/
	byte	devsubclass;	/* Device sub-class	*/
	byte	devproto;	/* Protocol code	*/
	byte	maxpktsiz0;	/* Max. pkt size EP 0	*/
	uint16	vendid;		/* Vendor ID		*/
	uint16	prodid;		/* Product ID		*/
	uint16	bcddev;		/* Device release num	*/
	byte	mfgstr;		/* Manufacturer string	*/
	byte	prodstr;	/* Product string	*/
	byte	sernostr;	/* Serial number string	*/
	byte	numcfg;		/* No. of configurations*/
};

struct	usb_cfgdesc {
	byte	length;		/* Size in bytes	*/
	byte	desctype;	/* Descriptor type	*/
	uint16	totallen;	/* Total bytes returned	*/
	byte	numintf;	/* No. of interfaces	*/
	byte	cfgvalue;	/* Configuration value	*/
	byte	cfgstr;		/* Configuration string	*/
	byte	attr;		/* Attributes		*/
	byte	maxpwr;		/* Max power consumption*/
};

struct	usb_intfdesc {
	byte	length;		/* Size in bytes	*/
	byte	desctype;	/* Descriptor type	*/
	byte	intfnum;	/* Interface number	*/
	byte	altsetting;	/* Alternate setting	*/
	byte	numep;		/* No. of endpoints	*/
	byte	intfclass;	/* Interface class	*/
	byte	intfsubclass;	/* Interface sub-class	*/
	byte	intfproto;	/* Protocol code	*/
	byte	intfstr;	/* Interface string	*/
};

struct	usb_epdesc {
	byte	length;		/* Size in bytes	*/
	byte	desctype;	/* Descriptor type	*/
	byte	epaddr;		/* Endpoint address	*/
	byte	attr;		/* Attributes		*/
	uint16	maxpktsize;	/* Max packet size	*/
	byte	interval;	/* Polling interval	*/
};

#pragma pack()

#define	USBEP_STATE_FREE	0
#define	USBEP_STATE_USED	1

struct	usbepcblk {
	byte	state;		/* Used or free		*/
	did32	devid;		/* Index in device table*/
	struct	usbdcblk *usbdptr;
				/* USB device ctrl blk	*/
	struct	usbep *epptr;	/* USB endpoint info	*/
};

#define	USBEP_DIR_OUT		0
#define	USBEP_DIR_IN		1

#define	MAX_USBD_EP		16

#define	USBEP_TYPE_CTRL		0
#define	USBEP_TYPE_ISO		1
#define	USBEP_TYPE_BULK		2
#define	USBEP_TYPE_INTR		3

struct	usbep {
	byte	addr;		/* Endpoint address	*/
	byte	type;		/* Endpoint type	*/
	byte	dir;		/* ENdpoint direction	*/
	did32	devid;		/* Device index		*/
};

#define	USBD_HCI_EHCI		0

#define	USBD_STATE_FREE		0
#define	USBD_STATE_PWRD		1
#define	USBD_STATE_DFLT		2
#define	USBD_STATE_ADDR		3
#define	USBD_STATE_CFG		4

//#define	NUSBD			1

struct	usbdcblk {
	did32	devid;			/* Index in device table*/
	byte	hcitype;		/* Host controller type	*/
	did32	hcidev;			/* Host contr. device	*/
	byte	address;		/* Device address	*/
	byte	state;			/* Device state		*/
	struct	usb_devdesc *devdesc;	/* Device descriptor	*/
	struct	usb_cfgdesc *cfgdesc;	/* Config. descriptor	*/
	struct	usbep ep[MAX_USBD_EP];	/* Device endpoints	*/
	int32	nep;			/* No. of endpoints	*/
};

extern	struct usbdcblk usbdtab[NUSBD];

//#define	NUSBEP	(NUSBD * MAX_USBD_EP)
extern	struct	usbepcblk usbeptab[NUSBEP];

#define	USB_CTRL_TRANSFER	0

#define	USB_TFR_EP_CTRL		0
#define	USB_TFR_EP_ISO		1
#define	USB_TFR_EP_BULK		2
#define	USB_TFR_EP_INTR		4

struct	usbtransfer {
	struct	usbdcblk *usbdptr;/* USB device ctl blk	*/
	byte	eptype;		/* Endpoint type	*/
	byte	ep;		/* Endpoint number	*/
	bool8	dirin;		/* Endpoint direction	*/
	struct	usb_devreq *dvrq;/* USB device request	*/
	char	*buffer;	/* Data buffer		*/
	int32	size;		/* Data buffer size	*/
};
