/* usb.h - USB related definitions */

#pragma pack(1)

#define	USB_DEVREQ_REQTYP_DEV	0x00
#define	USB_DEVREQ_REQTYP_INT	0x01
#define	USB_DEVREQ_REQTYP_EP	0x02

#define	USB_DEVREQ_REQ_GET_STS	0
#define	USB_DEVREQ_REQ_CLR_FT	1
#define	USB_DEVREQ_REQ_SET_FT	3
#define	USB_DEVREQ_REQ_SET_ADDR 5
#define	USB_DEVREQ_REQ_GET_DESC 6
#define	USB_DEVREQ_REQ_SET_DESC 7
#define	USB_DEVREQ_REQ_GET_CONF 8
#define	USB_DEVREQ_REQ_SET_CONF 9
#define	USB_DEVREQ_REQ_GET_INT	10
#define	USB_DEVREQ_REQ_SET_INT	11
#define	USB_DEVREQ_REQ_SYNC_FR	12

#define	USB_DEVREQ_IDX_DIR_OUT	0x0000
#define	USB_DEVREQ_IDX_DIR_IN	0x0080

struct	usb_devreq {
	byte	reqtype;	/* Request Type		*/
	byte	request;	/* Request		*/
	uint16	value;		/* Value		*/
	uint16	index;		/* Index		*/
	uint16	length;		/* No. of bytes	of data	*/
};

#define	USB_DESC_DEV	1
#define	USB_DESC_CONF	2
#define	USB_DESC_STR	3
#define	USB_DESC_INT	4
#define	USB_DESC_EP	5
#define	USB_DESC_DEVQ	6
#define	USB_DESC_OSC	7
#define	USB_DESC_INTPWR 8

struct	usb_devdesc {
	byte	length;		/* Length in bytes	*/
	byte	desctype;	/* Descriptor type	*/
	uint16	bcdusb;		/* USB Spec release no.	*/
	byte	devclass;	/* Device class		*/
	byte	devsubclass;	/* Device sub class	*/
	byte	devproto;	/* Device protocol	*/
	byte	maxpktsiz0;	/* Max pkt size ep 0	*/
	uint16	vendid;		/* Vendor ID		*/
	uint16	prodid;		/* Product ID		*/
	uint16	bcddev;		/* Device release no.	*/
	byte	mfstr;		/* Manufacturer		*/
	byte	prodstr;	/* Product		*/
	byte	sernostr;	/* Serial number	*/
	byte	numconf;	/* No. of configurations*/
};

#define	USB_CONFDESC_ATTR_SP	0x40
#define	USB_CONFDESC_ATTR_RWK	0x20

struct	usb_confdesc {
	byte	length;		/* Length in bytes	*/
	byte	desctype;	/* Descriptor type	*/
	uint16	totallen;	/* Total length		*/
	byte	numint;		/* No. of interfaces	*/
	byte	confval;	/* Configuration value	*/
	byte	confstr;	/* Configuration string	*/
	byte	attr;		/* Attributes		*/
	byte	maxpower;	/* Max power consumption*/
};

struct	usb_intdesc {
	byte	length;		/* Length in bytes	*/
	byte	desctype;	/* Descriptor type	*/
	byte	intnum;		/* Interface number	*/
	byte	altset;		/* Alternate setting	*/
	byte	numep;		/* No. of endpoints	*/
	byte	intclass;	/* Interface class	*/
	byte	intsubclass;	/* Interface subclass	*/
	byte	intproto;	/* Interface protocol	*/
	byte	intstr;		/* Interface string	*/
};

#define	USB_EPDESC_EPA_DIR	0x80
#define	USB_EPDESC_EPA_DIR_OUT	0x00
#define	USB_EPDESC_EPA_DIR_IN	0x80
#define	USB_EPDESC_EPA_EP	0x0F

#define	USB_EPDESC_ATTR_CTL	0x00
#define	USB_EPDESC_ATTR_ISO	0x01
#define	USB_EPDESC_ATTR_BLK	0x02
#define	USB_EPDESC_ATTR_INT	0x03

struct	usb_epdesc {
	byte	length;		/* Length in bytes	*/
	byte	desctype;	/* Descriptor type	*/
	byte	epaddr;		/* Endpoint address	*/
	byte	attr;		/* Endpoint attributes	*/
	byte	maxpktsiz;	/* Mac packet size	*/
	byte	interval;	/* Interval for polling	*/
};
