/* acpi.c - acpi_scan */

#include <xinu.h>

/*------------------------------------------------------------------------
 * acpi_scan  -  Scan memory for ACPI tables
 *------------------------------------------------------------------------
 */
void	acpi_scan (void) {

	struct	acpi_rsdp *rsdpptr;	/* RSDP pointer			*/
	struct	acpi_rsdt *rsdtptr;	/* RSDT pointer			*/
	struct	acpi_madt *madtptr;	/* MADT pointer			*/
	struct	acpi_lapic *lapicptr;	/* Local APIC pointer		*/
	char	*ptr;			/* Pointer to scan memory	*/
	int32	found;			/* Flag to indicate found tables*/
	int32	n;			/* No. of entries in RSDT	*/
	int32	i;			/* Loop index variable		*/
	ptr = 0;

	/* Search for the RSDP table */

	found = -1;
	while(ptr < (char *)0x100000) {

		if(!strncmp(ptr, "RSD PTR", 6)) {
			found = 1;
			break;
		}

		ptr += 16;
	}

	if(!found) {
		return;
	}

	rsdpptr = (struct acpi_rsdp *)ptr;

	/* Get the RSDT address from RSDP */

	rsdtptr = (struct acpi_rsdt *)rsdpptr->rsdtaddr;

	n = (rsdtptr->len - sizeof(struct acpi_rsdt))/4;


	/* Search for the MADT in RSDT entries */

	for(i = 0; i < n; i++) {

		ptr = (char *)rsdtptr->entry[i];
		if(!strncmp(ptr, "APIC", 4)) {
			break;
		}
	}

	if(i >= n) {
		return;
	}

	madtptr = (struct acpi_madt *)ptr;

	ptr = (char *)madtptr->intrctl;

	while(ptr < ((char *)madtptr + madtptr->len)) {

		if((*ptr) == 0) {
			lapicptr = (struct acpi_lapic *)ptr;
			if(lapicptr->flags & ACPI_LAPIC_F_EN) {
				//kprintf("Found CPU APIC ID %d\n", lapicptr->apicid);
				cputab[ncpu].state = CPU_STATE_DOWN;
				cputab[ncpu].apicid = lapicptr->apicid;
				if(lapic->lapic_id == cputab[ncpu].apicid) {
					cputab[ncpu].bsp = TRUE;
					//kprintf("\tBSP\n");
					cputab[ncpu].state = CPU_STATE_UP;
				}
				ncpu++;
			}
		}

		ptr += *(ptr + 1);
	}
}
