/* xsh_ping.c - xsh_ping */

#include <xinu.h>
#include <stdio.h>
#include <string.h>

/*------------------------------------------------------------------------
 * xsh_ping - shell command to ping a remote host
 *------------------------------------------------------------------------
 */
shellcmd xsh_ping(int nargs, char *args[])
{
	uint32	ipaddr;			/* IP address in binary		*/
	int32	retval;			/* Return value			*/
	int32	slot;			/* Slot in ICMP to use		*/
	static	int32	seq = 0;	/* Sequence number		*/
	char	buf[56];		/* Buffer of chars		*/
	int32	i;			/* Index into buffer		*/
	int32	nextval;		/* Next value to use		*/

	/* For argument '--help', emit help about the 'ping' command	*/

	if (nargs == 2 && strncmp(args[1], "--help", 7) == 0) {
		printf("Use: %s  address\n\n", args[0]);
		printf("Description:\n");
		printf("\tUse ICMP Echo to ping a remote host\n");
		printf("Options:\n");
		printf("\t--help\t display this help and exit\n");
		printf("\taddress\t an IP address in dotted decimal\n");
		return 0;
	}

	/* Check for valid number of arguments */

	if (nargs != 2) {
		fprintf(stderr, "%s: invalid arguments\n", args[0]);
		fprintf(stderr, "Try '%s --help' for more information\n",
				args[0]);
		return 1;
	}

	if(dnslookup(args[1], &ipaddr) == SYSERR) {
		fprintf(stderr, "DNS cannot resolve %s\n", args[1]);
		return 1;
	}
	printf("Pinging %d.%d.%d.%d\n", (ipaddr>>24)&0xff,
						(ipaddr>>16)&0xff,
						(ipaddr>>8)&0xff,
						(ipaddr)&0xff);

	/* Register to receive an ICMP Echo Reply */

	slot = icmp_register(ipaddr);
	if (slot == SYSERR) {
		fprintf(stderr,"%s: ICMP registration failed\n", args[0]);
		return 1;
	}

	/* Fill the buffer with values - start with low-order byte of	*/
	/*	the sequence number and increment			*/

	nextval = seq;
	for (i = 0; i<sizeof(buf); i++) {
		buf[i] = 0xff & nextval++;
	}

	/* Send an ICMP Echo Request */
	retval = icmp_send(ipaddr, ICMP_ECHOREQST, slot,
					seq++, buf, sizeof(buf));
	if (retval == SYSERR) {
		fprintf(stderr, "%s: cannot send ping\n", args[0]);
		icmp_release(slot);
		return 1;
	}

	/* Read a reply, waiting up to 3 seconds */

	retval = icmp_recv(slot, buf, sizeof(buf), 3000);
	icmp_release(slot);
	if (retval == TIMEOUT) {
		fprintf(stderr, "%s: no response from host %s\n", args[0],
					args[1]);
		icmp_release(slot);
		return 1;
	}

	if (retval != sizeof(buf)) {
		fprintf(stderr,"expected %d bytes but got back %d\n",
			sizeof(buf), retval);
		}
	fprintf(stderr, "host %s is alive\n", args[1]);
	icmp_release(slot);
	return 0;
}
