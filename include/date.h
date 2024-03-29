/* date.h - ntim2xtim, xtim2ntim, isleap, utim2ltim */

/************************************************************************/
/*									*/
/* 	Constants and declarations for date storage and conversion	*/
/*									*/
/*   Xinu stores time as seconds past Jan 1, 1970 (UNIX format), with	*/
/* 1 being 1 second into Jan. 1, 1970, GMT (universal time).  The	*/
/* Internet uses seconds past Jan 1, 1900 (also GMT or Universal Time)	*/
/*									*/
/************************************************************************/

/* Conversion between network and local representations */

#define	ntim2xtim(x)  ((x)-2208988800U)	/* Net-to-xinu time		*/
#define	xtim2ntim(x)  ((x)+2208988800U)	/* Xinu-to-net time		*/

/* Constants for Daylight Savings Time					*/									

#define	DATE_DST_OFF	0		/* Force DST to be off		*/
#define	DATE_DST_ON	1		/* Force DST to be on		*/
#define	DATE_DST_AUTO	2		/* Compute DST automatically	*/

/* Days in months and month names used to format a date */

struct	dateinfo {
	uint32	dt_boot;		/* Time when system booted	*/
					/*  add clktime to get the	*/
					/*  current time-of-day		*/
	bool8	dt_bootvalid;		/* Is dt_boot field valid?	*/
	int32	dt_daylight;		/* Whether to compute daylight	*/
					/*   savings time		*/
	int32	dt_msize[12];		/* Days per month		*/
	char	*dt_mnam[12];		/* Month names			*/
	char	*dt_dnam[7];		/* Day names			*/
};

extern	struct	dateinfo Date;		/* Global date information	*/

/* Constants for converting time to month/day/year/hour/minute/second	*/

#define	isleap(x)	((x)%4==0)	/* Leap year? (1970-2099)	*/
#define	SECPERDY	(60*60*24)	/* One day in seconds		*/
#define	SECPERHR	(60*60)		/* One hour in seconds		*/
#define	SECPERMN	(60)		/* One minute in seconds	*/

/* The local time zone can be set to EST, CST, MST,or PST.		*/

#define	ZONE_EST	5		/* Eastern Standard time is 5	*/
#define	ZONE_CST	6		/*  hours west of England	*/
#define	ZONE_MST	7
#define	ZONE_PST	8
#define	TIMEZONE	ZONE_EST	/* Timezone for this system	*/

/* In-line procedures to convert universal-to-local time and vice versa	*/

#define	utim2ltim(x)	((x)-TIMEZONE*SECPERHR)
#define	ltim2utim(x)	((x)+TIMEZONE*SECPERHR)

#define	TIMERPORT	123		/* UDP port for time server	*/
#define	TIMELPORT	53678		/* Local UDP port for time	*/

#ifndef	TIMESERVER
#define	TIMESERVER	"pool.ntp.org"	/* IP address of NTP time server*/
#endif

#define	TIMETIMEOUT	2000		/* Timeout for time server (ms)	*/
