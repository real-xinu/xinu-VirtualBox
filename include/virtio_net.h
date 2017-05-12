/* virtio_net.h - VirtIO Network related definitions */

#pragma pack(1)
/* Format of network packet header */

struct	virtio_net_hdr {
	byte	flags;
	byte	gso_type;
	uint16	hdr_len;
	uint16	gso_size;
	uint16	csum_start;
	uint16	csum_offset;
};
#pragma pack()

#define	VIRTIO_NET_F_CSUM			0x00000001
#define	VIRTIO_NET_F_GUEST_CSUM			0x00000002
#define	VIRTIO_NET_F_CTRL_GUEST_OFFLOADS	0x00000004
#define	VIRTIO_NET_F_MAC			0x00000020
#define	VIRTIO_NET_F_GUEST_TSO4			0x00000080
#define	VIRTIO_NET_F_GUEST_TSO6			0x00000100
#define	VIRTIO_NET_F_GUEST_ECN			0x00000200
#define	VIRTIO_NET_F_GUEST_UFO			0x00000400
#define	VIRTIO_NET_F_HOST_TSO4			0x00000800
#define	VIRTIO_NET_F_HOST_TSO6			0x00001000
#define	VIRTIO_NET_F_HOST_ECN			0x00002000
#define	VIRTIO_NET_F_HOST_UFO			0x00004000
#define	VIRTIO_NET_F_MRG_RXBUF			0x00008000
#define	VIRTIO_NET_F_STATUS			0x00010000
#define	VIRTIO_NET_F_CTRL_VQ			0x00020000
#define	VIRTIO_NET_F_CTRL_RX			0x00040000
#define	VIRTIO_NET_F_CTRL_VLAN			0x00080000
#define	VIRTIO_NET_F_GUEST_ANNOUNCE		0x00200000
#define	VIRTIO_NET_F_MQ				0x00400000
#define	VIRTIO_NET_F_CTRL_MAC_ADDR		0x00800000

#define	VIRTIO_NET_PCI_VID	0x1AF4
#define	VIRTIO_NET_PCI_DID	0x1000

#define	VIRTIO_NET_NQUEUE	3

#define	VIRTIO_NET_RX_RING_SIZE	64
#define	VIRTIO_NET_TX_RING_SIZE	32
