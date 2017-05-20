/* acpi.h - ACPI related definitions */

#pragma pack(1)

struct	acpi_rsdp {
	char	sig[8];		/* Signature "RSD PTR"	*/
	byte	cksum;		/* Checksum		*/
	char	oemid[6];	/* OEM ID		*/
	byte	rev;		/* Revision		*/
	uint32	rsdtaddr;	/* RSDT Address		*/
	uint32	len;		/* Length in bytes	*/
	uint64	xsdtaddr;	/* XSDT Address		*/
	byte	xcksum;		/* Extended checksum	*/
	byte	res[3];		/* Reserved		*/
};

struct	acpi_rsdt {
	char	sig[4];		/* Signature "RSDT"	*/
	uint32	len;		/* Length in bytes	*/
	byte	rev;		/* Revision		*/
	byte	cksum;		/* Checksum		*/
	char	oemid[6];	/* OEM ID		*/
	char	oemtabid[8];	/* OEM Table ID		*/
	uint32	oemrev;		/* OEM revision		*/
	uint32	creatid;	/* Creator ID		*/
	uint32	creatrev;	/* Creator revision	*/
	uint32	entry[];	/* Addresses of tables	*/
};

#define	ACPI_MADT_IC_LAPIC	0
#define	ACPI_MADT_IC_IOAPIC	1

struct	acpi_madt {
	char	sig[4];		/* Signature "APIC"	*/
	uint32	len;		/* Length in bytes	*/
	byte	rev;		/* Revision		*/
	byte	cksum;		/* Checksum		*/
	char	oemid[6];	/* OEM ID		*/
	char	oemtabid[8];	/* OEM Table ID		*/
	uint32	oemrev;		/* OEM revision		*/
	uint32	creatid;	/* Creator ID		*/
	uint32	creatrev;	/* Creator revision	*/
	uint32	lapicaddr;	/* Local APIC address	*/
	uint32	flags;		/* Multiple APIC flags	*/
	byte	intrctl[];	/* Interrupt controller	*/
};

#define	ACPI_LAPIC_F_EN	0x00000001

struct	acpi_lapic {
	byte	type;		/* Type of entry	*/
	byte	len;		/* Length in bytes	*/
	byte	acpi_procid;	/* ACPI processor ID	*/
	byte	apicid;		/* APIC ID		*/
	uint32	flags;		/* Local APIC flags	*/
};

#pragma pack()
