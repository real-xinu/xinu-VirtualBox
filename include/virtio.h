/* virtio.h - Definitions related to VirtIO */

#define	VIRTIO_ALIGN(x)	(((x) + 4095) & (~4095))

#pragma pack(1)

/* Format of Virtual Queue Descriptor */

#define	VIRTQ_DESC_F_NEXT	1
#define	VIRTQ_DESC_F_WRITE	2
#define	VIRTQ_DESC_F_INDIRECT	4

struct	virtq_desc {
	uint64	addr;	/* Buffer address	*/
	uint32	len;	/* Buffer length	*/
	uint16	flags;	/* Descriptor flags	*/
	uint16	next;	/* Next descriptor index*/
};

/* Format of Available Ring */

#define	VIRTQ_AVAIL_F_NO_INTERRUPT	1

struct	virtq_avail {
	uint16	flags;	/* Avail. ring flags	*/
	uint16	idx;	/* Next index in ring	*/
	uint16	ring[];	/* Available ring	*/
	/* Here comes the used_event field */
};

/* Format of Used Ring Element */

struct	virtq_used_elem {
	uint32	id;	/* Descriptor index		*/
	uint32	lenl;	/* Length of descriptor chain	*/
};

/* Format of Used Ring */

struct	virtq_used {
	uint16	flags;	/* Used ring flags	*/
	uint16	idx;	/* Next index in ring	*/
	struct	virtq_used_elem ring[];
			/* Used ring		*/
	/* Here comes the avail_event field	*/
};

/* Format of VirtIO control registers	*/

#define	VIRTIO_DEV_ST_ACK	0x00000001
#define	VIRTIO_DEV_ST_DRV	0x00000002
#define	VIRTIO_DEV_ST_DRVOK	0x00000004

struct	virtio_csr {
	uint32	dev_features;	/* Device features	*/
	uint32	drv_features;	/* Driver features	*/
	uint32	qaddr;		/* Queue address (/4096)*/
	uint16	qsize;		/* Queue size		*/
	uint16	qselect;	/* Queue select		*/
	uint16	qnotify;	/* Queue notify		*/
	byte	dev_status;	/* Device status	*/
	byte	isr;		/* Interrupt status	*/
	union {
	  struct {
	    byte mac[6];		/* Mac address for net	*/
	  };
	};
};

#pragma pack()

/* Format of Virtual Queue */

struct	virtq {
	volatile struct	virtq_desc *desc;	/* Descriptor array	*/
	volatile struct	virtq_avail *avail;	/* Available ring	*/
	volatile struct	virtq_used *used;	/* Used ring		*/
	int32	queue_size;			/* Virtq size		*/
	int32	avail_idx;
	int32	used_idx;
};

/* Format of VirtIO control block */

struct	virtio_cblk {
	struct	virtio_csr *csr;/* Control registers		*/
	uint32	dev_features;	/* Device features		*/
	uint32	neg_features;	/* Negotiated features		*/
	int32	nqueues;	/* No. of virtual queues	*/
	struct	virtq queue[];	/* Virtual Queues		*/
};
