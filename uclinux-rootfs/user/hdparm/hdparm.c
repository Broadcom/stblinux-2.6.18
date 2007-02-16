/* hdparm.c - Command line interface to get/set hard disk parameters */
/*          - by Mark S. Lord (C) 1994-2000 -- freely distributable */



#include <unistd.h>
#include <linux/string.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#ifndef __USE_GNU
#define __USE_GNU
/* To turn on O_DIRECT, someone above turn it off, but bits/fcntl needs it */
#endif

#include <fcntl.h>
#include <errno.h>
#include <ctype.h>
#include <sys/ioctl.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/sysmacros.h>
#include <sys/time.h>
#include <sys/times.h>
#include <sys/types.h>
#include <linux/version.h>
#include <linux/hdreg.h>

#include <linux/major.h>


#ifndef _SYS_IPC_H
#define _SYS_IPC_H
/* To suppress fs.h from defining __kernel_key_t */
#endif




#include <linux/fs.h>

#define VERSION "v4.0c"

#define	 DO_FLUSHCACHE		/* under construction: force cache flush on -W0 */

#ifndef CDROM_SELECT_SPEED	/* already defined in 2.3.xx kernels and above */
#define CDROM_SELECT_SPEED       0x5322
#endif
/*
 * For kernels prior to 1.3.61, HDIO_SET_32BIT is HDIO_SET_CHIPSET
 */
#ifndef HDIO_SET_32BIT
#ifdef HDIO_SET_CHIPSET
#define	HDIO_SET_32BIT HDIO_SET_CHIPSET
#define	HDIO_GET_32BIT HDIO_GET_CHIPSET
#endif
#endif

#define TIMING_MB		64
#define TIMING_BUF_MB		1
#define TIMING_BUF_BYTES	(TIMING_BUF_MB * 1024 * 1024)
#define TIMING_BUF_COUNT	(timing_MB / TIMING_BUF_MB)
#define BUFCACHE_FACTOR		2

char *progname;
static struct hd_driveid id;

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,0)
static struct hd_big_geometry r;
static struct hd_big_geometry c;

#else
static struct hd_geometry r;
static struct hd_geometry c;
#endif
static struct hd_geometry d;

static int verbose = 0, get_identity = 0, get_geom = 0, noisy = 1, quiet = 0;
static int flagcount = 0, do_flush = 0, is_scsi_hd = 0, is_xt_hd = 0;
static int do_ctimings, do_timings = 0, old_identity = 0, timing_MB = TIMING_MB;
static int do_odirect = 0;

static unsigned long set_readahead= 0, get_readahead= 0, readahead= 0;     
static unsigned long set_readonly = 0, get_readonly = 0, readonly = 0;     
static unsigned long set_unmask   = 0, get_unmask   = 0, unmask   = 0;     
static unsigned long set_mult     = 0, get_mult     = 0, mult     = 0;     
#ifdef HDIO_SET_DMA
static unsigned long set_dma      = 0, get_dma      = 0, dma      = 0;     
#endif
#ifdef HDIO_SET_NOWERR
static unsigned long set_nowerr   = 0, get_nowerr   = 0, nowerr   = 0;
#endif
#ifdef HDIO_GET_KEEPSETTINGS
static unsigned long set_keep     = 0, get_keep     = 0, keep     = 0;
#endif
#ifdef HDIO_SET_32BIT
static unsigned long set_io32bit  = 0, get_io32bit  = 0, io32bit  = 0;
#endif
#ifdef HDIO_SET_PIO_MODE
static unsigned long set_piomode  = 0, noisy_piomode= 0, piomode  = 0;
#endif
#ifdef HDIO_DRIVE_CMD
static unsigned long set_dkeep    = 0, get_dkeep    = 0, dkeep    = 0;
static unsigned long set_standby  = 0, get_standby  = 0, standby  = 0;
static unsigned long set_xfermode = 0, get_xfermode = 0, xfermode = 0;
static unsigned long set_lookahead= 0, get_lookahead= 0, lookahead= 0;
static unsigned long set_prefetch = 0, get_prefetch = 0, prefetch = 0;
static unsigned long set_defects  = 0, get_defects  = 0, defects  = 0;
static unsigned long set_wcache   = 0, get_wcache   = 0, wcache   = 0;
static unsigned long set_doorlock = 0, get_doorlock = 0, doorlock = 0;
static unsigned long set_accoustic = 0, get_accoustic = 0, accoustic = 0;
static unsigned long set_seagate  = 0, get_seagate  = 0;
static unsigned long set_standbynow = 0, get_standbynow = 0;
static unsigned long set_sleepnow   = 0, get_sleepnow   = 0;
static unsigned long get_powermode  = 0;
#endif
#ifdef CDROM_SELECT_SPEED
static unsigned long set_cdromspeed = 0, cdromspeed = 0;
#endif /* CDROM_SELECT_SPEED */
#ifdef HDIO_SET_32BIT
static int get_IDentity = 0;
#endif
#ifdef HDIO_UNREGISTER_HWIF
static int	unregister_hwif = 0;
static int	hwif = 0;
#endif
#ifdef HDIO_SCAN_HWIF
static int	scan_hwif = 0;
static int	hwif_data = 0;
static int	hwif_ctrl = 0;
static int	hwif_irq = 0;
#endif

#ifdef HDIO_DRIVE_RESET
static int	perform_reset = 0;
#endif /* HDIO_DRIVE_RESET */
#ifdef HDIO_TRISTATE_HWIF
static int	perform_tristate = 0,	tristate = 0;
#endif /* HDIO_TRISTATE_HWIF */

char *cfg_str[] =
{	"",             " HardSect",   " SoftSect",  " NotMFM",
	" HdSw>15uSec", " SpinMotCtl", " Fixed",     " Removeable",
	" DTR<=5Mbs",   " DTR>5Mbs",   " DTR>10Mbs", " RotSpdTol>.5%",
	" dStbOff",     " TrkOff",     " FmtGapReq", " nonMagnetic"
};

char *ata_major_str[] = {
	"reserved ",	"ATA-1 ",	"ATA-2 ",	"ATA-3 ",
	"ATA-4 ",	"ATA-5 ",	"ATA-6 ",	"ATA-7 ",
	"ATA-8 ",	"ATA-9 ",	"ATA-10 ",	"ATA-11 ",
	"ATA-12 ",	"ATA-13 ",	"ATA-14 ",	"reserved "
};

char *ata_minor_str[] = {
	"Reserved : ",						/* 0x0000 */
	"ATA (ATA-1) X3T9.2 781D prior to revision 4 : ",	/* 0x0001 */
	"ATA-1 published, ANSI X3.221-1994 : ",			/* 0x0002 */
	"ATA (ATA-1) X3T10 781D revision 4 : ",			/* 0x0003 */
	"ATA-2 published, ANSI X3.279-1996 : ",			/* 0x0004 */
	"ATA-2 X3T10 948D prior to revision 2k : ",		/* 0x0005 */
	"ATA-3 X3T10 2008D revision 1 : ",			/* 0x0006 */
	"ATA-2 X3T10 948D revision 2k : ",			/* 0x0007 */
	"ATA-3 X3T10 2008D revision 0 : ",			/* 0x0008 */
	"ATA-2 X3T10 948D revision 3 : ",			/* 0x0009 */
	"ATA-3 published, ANSI X3.298-199x : ",			/* 0x000A */
	"ATA-3 X3T10 2008D revision 6 : ",			/* 0x000B */
	"ATA-3 X3T13 2008D revision 7 and 7a : ",		/* 0x000C */
	"ATA/ATAPI-4 X3T13 1153D revision 6 : ",		/* 0x000D */
	"ATA/ATAPI-4 T13 1153D revision 13 : ",			/* 0x000E */
	"ATA/ATAPI-4 X3T13 1153D revision 7 : ",		/* 0x000F */
	"Reserved : ",						/* 0x0010 */
	"ATA/ATAPI-4 T13 1153D revision 15 : ",			/* 0x0011 */
	"Reserved : ",						/* 0x0012 */
	"Reserved : ",						/* 0x0013 */
	"ATA/ATAPI-4 T13 1153D revision 14 : ",			/* 0x0014 */
	"Reserved : ",						/* 0x0015 */
	"Reserved : ",						/* 0x0016 */
	"ATA/ATAPI-4 T13 1153D revision 17 : ",			/* 0x0017 */
	"Reserved : "						/* 0x0018 */
};

char *SlowMedFast[]	= {"slow", "medium", "fast", "eide", "ata"};
char *BuffType[]	= {"unknown", "1Sect", "DualPort", "DualPortCache"};

#define YN(b)	(((b)==0)?"no":"yes")

static void dmpstr (char *prefix, unsigned int i, char *s[], unsigned int maxi)
{
	if (i > maxi)
		printf("%s%d", prefix, i);
	else
		printf("%s%s", prefix, s[i]);
}


#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,0)
static void dump_identity (struct hd_driveid *id , struct hd_big_geometry *R, struct hd_big_geometry *C, struct hd_geometry *D)

#else
/*
 * 2.6 no longer have hd_big_geometry defined.  
 */
static void dump_identity (struct hd_driveid *id , struct hd_geometry *R, struct hd_geometry *C, struct hd_geometry *D)

#endif
{
	int i;
	char pmodes[64] = {0,}, dmodes[128]={0,};

	printf("\n Model=%.40s, FwRev=%.8s, SerialNo=%.20s", 
		id->model, id->fw_rev, id->serial_no);
	printf("\n Config={");
	for (i=0; i<=15; i++) {
		if (id->config & (1<<i))
			printf("%s", cfg_str[i]);
	}
	printf(" }\n");
	printf(" RawCHS=%d/%d/%d, TrkSize=%d, SectSize=%d, ECCbytes=%d\n",
		id->cyls, id->heads, id->sectors,
		id->track_bytes, id->sector_bytes, id->ecc_bytes);
	dmpstr (" BuffType=",id->buf_type,BuffType,3);
	printf(", BuffSize=%dkB, MaxMultSect=%d", id->buf_size/2, id->max_multsect);
	if (id->max_multsect) {
		printf(", MultSect=");
		if (!(id->multsect_valid&1))
			printf("?%d?", id->multsect);
		else if (id->multsect)
			printf("%d", id->multsect);
		else
			printf("off");
	}
	putchar('\n');
	if (id->tPIO <= 5) {
		strcat(pmodes, "pio0 ");
		if (id->tPIO >= 1) strcat(pmodes, "pio1 ");
		if (id->tPIO >= 2) strcat(pmodes, "pio2 ");
	}
	if (!(id->field_valid&1))
		printf(" (maybe):");
	printf(" CurCHS=%d/%d/%d, CurSects=%d",
		id->cur_cyls, id->cur_heads, id->cur_sectors, *(int *)&id->cur_capacity0);
	printf(", LBA=%s", YN(id->capability&2));
	if (id->capability&2)
 		printf(", LBAsects=%d", id->lba_capacity);

	if (id->capability&1) {
		if (id->dma_1word | id->dma_mword) {
			if (id->dma_1word & 0x100)	strcat(dmodes,"*");
			if (id->dma_1word & 1)		strcat(dmodes,"sdma0 ");
			if (id->dma_1word & 0x200)	strcat(dmodes,"*");
			if (id->dma_1word & 2)		strcat(dmodes,"sdma1 ");
			if (id->dma_1word & 0x400)	strcat(dmodes,"*");
			if (id->dma_1word & 4)		strcat(dmodes,"sdma2 ");
			if (id->dma_1word & 0xf800)	strcat(dmodes,"*");
			if (id->dma_1word & 0xf8)	strcat(dmodes,"sdma? ");
			if (id->dma_mword & 0x100)	strcat(dmodes,"*");
			if (id->dma_mword & 1)		strcat(dmodes,"mdma0 ");
			if (id->dma_mword & 0x200)	strcat(dmodes,"*");
			if (id->dma_mword & 2)		strcat(dmodes,"mdma1 ");
			if (id->dma_mword & 0x400)	strcat(dmodes,"*");
			if (id->dma_mword & 4)		strcat(dmodes,"mdma2 ");
			if (id->dma_mword & 0xf800)	strcat(dmodes,"*");
			if (id->dma_mword & 0xf8)	strcat(dmodes,"mdma? ");
		}
	}
	printf("\n IORDY=");
	if (id->capability&8)
		printf((id->capability&4) ? "on/off" : "yes");
	else
		printf("no");
	if ((id->capability&8) || (id->field_valid&2)) {
		if (id->field_valid&2) {
			printf(", tPIO={min:%d,w/IORDY:%d}", id->eide_pio, id->eide_pio_iordy);
			if (id->eide_pio_modes & 1)	strcat(pmodes, "pio3 ");
			if (id->eide_pio_modes & 2)	strcat(pmodes, "pio4 ");
			if (id->eide_pio_modes &~3)	strcat(pmodes, "pio? ");
		}
#ifdef HDIO_OBSOLETE_IDENTITY
		if (id->field_valid&4) {
			if (old_identity) {
				strcat(dmodes," udma??");
			} else {
				if (id->dma_ultra & 0x100)	strcat(dmodes,"*");
				if (id->dma_ultra & 0x001)	strcat(dmodes,"udma0 ");
				if (id->dma_ultra & 0x200)	strcat(dmodes,"*");
				if (id->dma_ultra & 0x002)	strcat(dmodes,"udma1 ");
				if (id->dma_ultra & 0x400)	strcat(dmodes,"*");
				if (id->dma_ultra & 0x004)	strcat(dmodes,"udma2 ");
#ifdef __NEW_HD_DRIVE_ID
				if (id->hw_config & 0x2000) {
#else /* !__NEW_HD_DRIVE_ID */
				if (id->word93 & 0x2000) {
#endif /* __NEW_HD_DRIVE_ID */
					if (id->dma_ultra & 0x800)	strcat(dmodes,"*");
					if (id->dma_ultra & 0x008)	strcat(dmodes,"udma3 ");
					if (id->dma_ultra & 0x1000)	strcat(dmodes,"*");
					if (id->dma_ultra & 0x0010)	strcat(dmodes,"udma4 ");
					if (id->dma_ultra & 0x2000)	strcat(dmodes,"*");
					if (id->dma_ultra & 0x0020)	strcat(dmodes,"udma5 ");
					if (id->dma_ultra & 0x4000)	strcat(dmodes,"*");
					if (id->dma_ultra & 0x0040)	strcat(dmodes,"udma6 ");
					if (id->dma_ultra & 0x8000)	strcat(dmodes,"*");
					if (id->dma_ultra & 0x0080)	strcat(dmodes,"udma7 ");
				}
			}
#endif
		}
	}
	if ((id->capability&1) && (id->field_valid&2))
		printf(", tDMA={min:%d,rec:%d}", id->eide_dma_min, id->eide_dma_time);
	printf("\n PIO modes: %s", pmodes);
	printf("\n DMA modes: %s", dmodes);
#ifdef __NEW_HD_DRIVE_ID
	if ((id->minor_rev_num) || (id->major_rev_num)) {
		printf("\n Drive Supports : ");
		printf("%s", ata_minor_str[id->minor_rev_num]);
		for (i=0; i<=15; i++) {
			if (id->major_rev_num & (1<<i))
				printf("%s", ata_major_str[i]);
		}
	}
#endif /* __NEW_HD_DRIVE_ID */
	if (C->cylinders) {
		printf("\n Kernel Drive Geometry LogicalCHS=%d/%d/%d",
			C->cylinders, C->heads, C->sectors);
		if (R->cylinders)
			printf(" PhysicalCHS=%d/%d/%d",
				R->cylinders, R->heads, R->sectors);
	} else if (D->cylinders) {
		printf("\n Kernel Drive Geometry LogicalCHS=%d/%d/%d",
			D->cylinders, D->heads, D->sectors);
	}
	printf("\n");
}

void flush_buffer_cache (int fd)
{
	fsync (fd);				/* flush buffers */
	if (ioctl(fd, BLKFLSBUF, NULL))		/* do it again, big time */
		perror("BLKFLSBUF failed");
#ifdef HDIO_DRIVE_CMD
	if (is_scsi_hd || is_xt_hd) {
		sleep(1);
	} else {
		if (ioctl(fd, HDIO_DRIVE_CMD, NULL))	/* await completion */
			perror("HDIO_DRIVE_CMD(null) failed");
	}
#endif
}

int seek_to_zero (int fd)
{
	if (lseek(fd, (off_t) 0, SEEK_SET)) {
		perror("lseek() failed");
		return 1;
	}
	return 0;
}

int read_big_block (int fd, char *buf)
{
	int i, rc;
	if ((rc = read(fd, buf, TIMING_BUF_BYTES)) != TIMING_BUF_BYTES) {
		if (rc)
			perror("read() failed");
		else
			fputs ("read() hit EOF - device too small\n", stderr);
		return 1;
	}
	/* access all sectors of buf to ensure the read fully completed */
	for (i = 0; i < TIMING_BUF_BYTES; i += 512)
		buf[i] &= 1;
	return 0;
}

static double correction = 0.0;

void time_cache (int fd)
{
	int i;
	char *buf;
	struct itimerval e1, e2;
	int shmid;

	if ((shmid = shmget(IPC_PRIVATE, TIMING_BUF_BYTES, 0600)) == -1) {
		perror ("could not allocate sharedmem buf");
		return;
	}
	if (shmctl(shmid, SHM_LOCK, NULL) == -1) {
		perror ("could not lock sharedmem buf");
		(void) shmctl(shmid, IPC_RMID, NULL);
		return;
	}
	if ((buf = shmat(shmid, (char *) 0, 0)) == (char *) -1) {
		perror ("could not attach sharedmem buf");
		(void) shmctl(shmid, IPC_RMID, NULL);
		return;
	}
	if (shmctl(shmid, IPC_RMID, NULL) == -1)
		perror ("shmctl(,IPC_RMID,) failed");

	/* Clear out the device request queues & give them time to complete */
	sync();
	sleep(3);

	/* Calculate a correction factor for the basic
	 * overhead of doing a read() from the buffer cache.
	 * To do this, we read the data once to "cache it" and
	 * to force full preallocation of our timing buffer,
	 * and then we re-read it 10 times while timing it.
	 *
	 * getitimer() is used rather than gettimeofday() because
	 * it is much more consistent (on my machine, at least).
	 */
	setitimer(ITIMER_REAL, &(struct itimerval){{1000,0},{1000,0}}, NULL);
	if (seek_to_zero (fd)) return;
	if (read_big_block (fd, buf)) return;
	printf(" Timing buffer-cache reads:   ");
	fflush(stdout);

	/* Clear out the device request queues & give them time to complete */
	sync();
	sleep(1);

	/* Time re-reading from the buffer-cache */
	getitimer(ITIMER_REAL, &e1);
	for (i = (BUFCACHE_FACTOR * TIMING_BUF_COUNT) ; i > 0; --i) {
		if (seek_to_zero (fd)) goto quit;
		if (read_big_block (fd, buf)) goto quit;
	}
	getitimer(ITIMER_REAL, &e2);
	correction = (e1.it_value.tv_sec - e2.it_value.tv_sec)
	 + ((e1.it_value.tv_usec - e2.it_value.tv_usec) / 1000000.0);

	/* Now remove the lseek() from the correction factor */
	getitimer(ITIMER_REAL, &e1);
	for (i = (BUFCACHE_FACTOR * TIMING_BUF_COUNT) ; i > 0; --i) {
		if (seek_to_zero (fd)) goto quit;
	}
	getitimer(ITIMER_REAL, &e2);
	correction -= (e1.it_value.tv_sec - e2.it_value.tv_sec)
	 + ((e1.it_value.tv_usec - e2.it_value.tv_usec) / 1000000.0);

	if ((BUFCACHE_FACTOR * timing_MB) >= correction)  /* more than 1MB/s */
		printf("%2d MB in %5.2f seconds =%6.2f MB/sec\n",
			(BUFCACHE_FACTOR * timing_MB), correction,
			(BUFCACHE_FACTOR * timing_MB) / correction);
	else
		printf("%2d MB in %5.2f seconds =%6.2f kB/sec\n",
			(BUFCACHE_FACTOR * timing_MB), correction,
			(BUFCACHE_FACTOR * timing_MB) / correction * 1024);
	correction /= BUFCACHE_FACTOR;

	flush_buffer_cache(fd);
	sleep(1);
quit:
	if (-1 == shmdt(buf))
		perror ("could not detach sharedmem buf");
}

void time_device (int fd)
{
	int i;
	char *buf;
	double elapsed;
	struct itimerval e1, e2;
	int shmid;

	if ((shmid = shmget(IPC_PRIVATE, TIMING_BUF_BYTES, 0600)) == -1) {
		perror ("could not allocate sharedmem buf");
		return;
	}
	if (shmctl(shmid, SHM_LOCK, NULL) == -1) {
		perror ("could not lock sharedmem buf");
		(void) shmctl(shmid, IPC_RMID, NULL);
		return;
	}
	if ((buf = shmat(shmid, (char *) 0, 0)) == (char *) -1) {
		perror ("could not attach sharedmem buf");
		(void) shmctl(shmid, IPC_RMID, NULL);
		return;
	}
	if (shmctl(shmid, IPC_RMID, NULL) == -1)
		perror ("shmctl(,IPC_RMID,) failed");

	/* Clear out the device request queues & give them time to complete */
	sync();
	sleep(3);

	printf(" Timing buffered disk reads:  ");
	fflush(stdout);

	/*
	 * getitimer() is used rather than gettimeofday() because
	 * it is much more consistent (on my machine, at least).
	 */
	setitimer(ITIMER_REAL, &(struct itimerval){{1000,0},{1000,0}}, NULL);

	/* Now do the timings for real */
	getitimer(ITIMER_REAL, &e1);
	for (i = TIMING_BUF_COUNT; i > 0; --i) {
		if (read_big_block (fd, buf)) goto quit;
	}
	getitimer(ITIMER_REAL, &e2);

	elapsed = (e1.it_value.tv_sec - e2.it_value.tv_sec)
	 + ((e1.it_value.tv_usec - e2.it_value.tv_usec) / 1000000.0);
	if (timing_MB >= elapsed)  /* more than 1MB/s */
		printf("%2d MB in %5.2f seconds =%6.2f MB/sec\n",
			timing_MB, elapsed, timing_MB / elapsed);
	else
		printf("%2d MB in %5.2f seconds =%6.2f kB/sec\n",
			timing_MB, elapsed, timing_MB / elapsed * 1024);

	if (elapsed <= (correction * 2))
		printf("Hmm.. suspicious results: probably not enough free memory for a proper test.\n");
#if 0  /* the "estimate" is just plain wrong for many systems.. */
	else if (correction != 0.0) {
		printf(" Estimating raw driver speed: ");
		elapsed -= correction;
		if (timing_MB >= elapsed)  /* more than 1MB/s */
			printf("%2d MB in %5.2f seconds =%6.2f MB/sec\n",
				timing_MB, elapsed, timing_MB / elapsed);
		else
			printf("%2d MB in %5.2f seconds =%6.2f kB/sec\n",
				timing_MB, elapsed, timing_MB / elapsed * 1024);
	}
#endif
quit:
	if (-1 == shmdt(buf))
		perror ("could not detach sharedmem buf");
}

void no_scsi (void)
{
	if (is_scsi_hd) {
		fputs (" operation not supported on SCSI disks\n", stderr);
		exit(EINVAL);
	}
}

void no_xt (void)
{
	if (is_xt_hd) {
		fputs (" operation not supported on XT disks\n", stderr);
		exit(EINVAL);
	}
}

static void on_off (unsigned int value)
{
	printf(value ? " (on)\n" : " (off)\n");
}

#ifdef HDIO_DRIVE_CMD
static void interpret_standby (unsigned int standby)
{
	printf(" (");
	switch(standby) {
		case 0:		printf("off");
				break;
		case 252:	printf("21 minutes");
				break;
		case 253:	printf("vendor-specific");
				break;
		case 254:	printf("?reserved");
				break;
		case 255:	printf("21 minutes + 15 seconds");
				break;
		default:
			if (standby <= 240) {
				unsigned int secs = standby * 5;
				unsigned int mins = secs / 60;
				secs %= 60;
				if (mins)	  printf("%d minutes", mins);
				if (mins && secs) printf(" + ");
				if (secs)	  printf("%d seconds", secs);
			} else if (standby <= 251) {
				unsigned int mins = (standby - 240) * 30;
				unsigned int hrs  = mins / 60;
				mins %= 60;
				if (hrs)	  printf("%d hours", hrs);
				if (hrs && mins)  printf(" + ");
				if (mins)	  printf("%d minutes", mins);
			} else
				printf("illegal value)\n");
			break;
	}
	printf(")\n");
}

static void interpret_xfermode (unsigned int xfermode)
{
	printf(" (");
	switch(xfermode) {
		case 0:		printf("default PIO mode");
				break;
		case 1:		printf("default PIO mode, disable IORDY");
				break;
		case 8:
		case 9:
		case 10:
		case 11:
		case 12:
		case 13:
		case 14:
		case 15:	printf("PIO flow control mode%d", xfermode-8);
				break;
		case 16:
		case 17:
		case 18:
		case 19:
		case 20:
		case 21:
		case 22:
		case 23:	printf("singleword DMA mode%d", xfermode-16);
				break;
		case 32:
		case 33:
		case 34:
		case 35:
		case 36:
		case 37:
		case 38:
		case 39:	printf("multiword DMA mode%d", xfermode-32);
				break;
		case 64:
		case 65:
		case 66:
		case 67:
		case 68:
		case 69:
		case 70:
		case 71:	printf("UltraDMA mode%d", xfermode-64);
				break;
		default:
				printf("unknown, probably not valid");
				break;
	}
	printf(")\n");
}
#endif /* HDIO_DRIVE_CMD */

int can_geometry (unsigned char type)
{
	if ((type & 0x07) == 0x07 ||
	    ((type & 0x05) != 0x05 &&
	     (type & 0x04) == 0x04) ||
	    (type == 0x00))
		return 1;
	return 0;
}


#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,0)
void get_geometry (int fd, struct hd_big_geometry *R, struct hd_big_geometry *C, struct hd_geometry *D, unsigned long *E)
{
	if (ioctl(fd, BLKGETSIZE, E))
		perror(" BLKGETSIZE failed");
	else if (ioctl(fd, HDIO_GETGEO_BIG, C))
		perror(" HDIO_GETGEO_BIG failed");

	if (ioctl(fd, BLKGETSIZE, E))
		perror(" BLKGETSIZE failed");
	else if (ioctl(fd, HDIO_GETGEO_BIG_RAW, R))
		perror(" HDIO_GETGEO_BIG_RAW failed");

	if (ioctl(fd, BLKGETSIZE, E))
		perror(" BLKGETSIZE failed");
	else if (ioctl(fd, HDIO_GETGEO, D))
		perror(" HDIO_GETGEO failed");
}

#else
void get_geometry (int fd, struct hd_geometry *R, struct hd_geometry *C, struct hd_geometry *D, unsigned long *E)
{

	if (ioctl(fd, BLKGETSIZE, E))
		perror(" BLKGETSIZE failed");
	else if (ioctl(fd, HDIO_GETGEO, D))
		perror(" HDIO_GETGEO failed");
}

#endif

void process_dev (char *devname)
{
	int fd;
	static long parm, multcount;
	struct stat stat_buf;
#ifndef HDIO_DRIVE_CMD
	int force_operation = 0;
#endif
	if (stat(devname,&stat_buf)) {
		perror(devname);
		exit(errno);
	}

#ifdef SCSI_DISK0_MAJOR
	if ((major(stat_buf.st_rdev) == SCSI_DISK0_MAJOR) 
	 || (major(stat_buf.st_rdev) == SCSI_DISK1_MAJOR) 
	 || (major(stat_buf.st_rdev) == SCSI_DISK2_MAJOR) 
	 || (major(stat_buf.st_rdev) == SCSI_DISK3_MAJOR) 
	 || (major(stat_buf.st_rdev) == SCSI_DISK4_MAJOR) 
	 || (major(stat_buf.st_rdev) == SCSI_DISK5_MAJOR) 
	 || (major(stat_buf.st_rdev) == SCSI_DISK6_MAJOR) 
	 || (major(stat_buf.st_rdev) == SCSI_DISK7_MAJOR) 
#else
	if ((major(stat_buf.st_rdev) == SCSI_DISK_MAJOR) 
#endif
#ifdef MD_MAJOR
	 || (major(stat_buf.st_rdev) == MD_MAJOR)
#endif
	) 
		is_scsi_hd = 1;
	else
#ifdef XT_DISK_MAJOR
	if (major(stat_buf.st_rdev) == XT_DISK_MAJOR) {
		is_xt_hd = 1;
		timing_MB = 2;
	} else
#endif
		if (major(stat_buf.st_rdev) != IDE0_MAJOR
		 && major(stat_buf.st_rdev) != IDE1_MAJOR
#ifdef IDE2_MAJOR
		 && major(stat_buf.st_rdev) != IDE2_MAJOR
#endif
#ifdef IDE3_MAJOR
		 && major(stat_buf.st_rdev) != IDE3_MAJOR
#endif
#ifdef IDE4_MAJOR
		 && major(stat_buf.st_rdev) != IDE4_MAJOR
#endif
#ifdef IDE5_MAJOR
		 && major(stat_buf.st_rdev) != IDE5_MAJOR
#endif
#ifdef IDE6_MAJOR
		 && major(stat_buf.st_rdev) != IDE6_MAJOR
#endif
#ifdef IDE7_MAJOR
		 && major(stat_buf.st_rdev) != IDE7_MAJOR
#endif
#ifdef IDE8_MAJOR
		 && major(stat_buf.st_rdev) != IDE8_MAJOR
#endif
#ifdef IDE9_MAJOR
		 && major(stat_buf.st_rdev) != IDE9_MAJOR
#endif
	) {
		fprintf(stderr,"%s not supported by hdparm\n",devname);
		exit(EINVAL);
	}

	if (do_odirect) {
		fd = open (devname, O_RDONLY|O_NONBLOCK|O_DIRECT);
	} else {
		fd = open (devname, O_RDONLY|O_NONBLOCK);
	}
	if (fd < 0) {
		perror(devname);
		exit(errno);
	}
	if (!quiet)
		printf("\n%s:\n", devname);
	if (set_readahead) {
		if (get_readahead)
			printf(" setting fs readahead to %ld\n", readahead);
		if (ioctl(fd, BLKRASET, readahead)) 
			perror(" BLKRASET failed");
	}
#ifdef HDIO_UNREGISTER_HWIF
	if (unregister_hwif) {
		no_scsi();
		printf(" attempting to unregister hwif#%d\n", hwif);
		if (ioctl(fd, HDIO_UNREGISTER_HWIF, hwif)) 
			perror(" HDIO_UNREGISTER_HWIF failed");
	}
#endif
#ifdef HDIO_SCAN_HWIF
	if (scan_hwif) {
		int	args[3];
		no_scsi();
		printf(" attempting to scan hwif (0x%x, 0x%x, %d)\n", hwif_data, hwif_ctrl, hwif_irq);
		args[0] = hwif_data;
		args[1] = hwif_ctrl;
		args[2] = hwif_irq;
		if (ioctl(fd, HDIO_SCAN_HWIF, args)) 
			perror(" HDIO_SCAN_HWIF failed");
	}
#endif
#ifdef HDIO_SET_PIO_MODE
	if (set_piomode) {
		no_scsi();
		no_xt();
		if (noisy_piomode) {
			if (piomode == 255)
				printf(" attempting to auto-tune PIO mode\n");
			else
				printf(" attempting to set PIO mode to %ld\n", piomode);
		}
		if (ioctl(fd, HDIO_SET_PIO_MODE, piomode)) 
			perror(" HDIO_SET_PIO_MODE failed");
	}
#endif
#ifdef HDIO_SET_32BIT
	if (set_io32bit) {
		no_scsi();
		no_xt();
		if (get_io32bit)
			printf(" setting 32-bit I/O support flag to %ld\n", io32bit);
		if (ioctl(fd, HDIO_SET_32BIT, io32bit)) 
			perror(" HDIO_SET_32BIT failed");
	}
#endif
	if (set_mult) {
		no_scsi();
		no_xt();
		if (get_mult)
			printf(" setting multcount to %ld\n", mult);
		if (ioctl(fd, HDIO_SET_MULTCOUNT, mult)) 
			perror(" HDIO_SET_MULTCOUNT failed");
#ifndef HDIO_DRIVE_CMD
		else force_operation = 1;
#endif
	}
	if (set_readonly) {
		if (get_readonly) {
			printf(" setting readonly to %ld", readonly);
			on_off(readonly);
		}
		if (ioctl(fd, BLKROSET, &readonly))
			perror(" BLKROSET failed");
	}
	if (set_unmask) {
		no_scsi();
		no_xt();
		if (get_unmask) {
			printf(" setting unmaskirq to %ld", unmask);
			on_off(unmask);
		}
		if (ioctl(fd, HDIO_SET_UNMASKINTR, unmask))
			perror(" HDIO_SET_UNMASKINTR failed");
	}
#ifdef HDIO_SET_DMA
	if (set_dma) {
		no_scsi();
		if (get_dma) {
			printf(" setting using_dma to %ld", dma);
			on_off(dma);
		}
		if (ioctl(fd, HDIO_SET_DMA, dma))
			perror(" HDIO_SET_DMA failed");
	}
#endif
#ifdef HDIO_SET_NOWERR
	if (set_nowerr) {
		no_scsi();
		no_xt();
		if (get_nowerr) {
			printf(" setting nowerr to %ld", nowerr);
			on_off(nowerr);
		}
		if (ioctl(fd, HDIO_SET_NOWERR, nowerr))
			perror(" HDIO_SET_NOWERR failed");
	}
#endif
#ifdef HDIO_GET_KEEPSETTINGS
	if (set_keep) {
		no_scsi();
		no_xt();
		if (get_keep) {
			printf(" setting keep_settings to %ld", keep);
			on_off(keep);
		}
		if (ioctl(fd, HDIO_SET_KEEPSETTINGS, keep))
			perror(" HDIO_SET_KEEPSETTINGS failed");
	}
#endif /* HDIO_GET_KEEPSETTINGS */
#ifdef HDIO_DRIVE_CMD
	if (set_doorlock) {
		unsigned char args[4] = {0,0,0,0};
		no_scsi();
		no_xt();
		args[0] = doorlock ? WIN_DOORLOCK : WIN_DOORUNLOCK;
		if (get_doorlock) {
			printf(" setting drive doorlock to %ld", doorlock);
			on_off(doorlock);
		}
		if (ioctl(fd, HDIO_DRIVE_CMD, &args))
			perror(" HDIO_DRIVE_CMD(doorlock) failed");
	}
	if (set_dkeep) {
		/* lock/unlock the drive's "feature" settings */
		unsigned char args[4] = {WIN_SETFEATURES,0,0,0};
		no_scsi();
		no_xt();
		if (get_dkeep) {
			printf(" setting drive keep features to %ld", dkeep);
			on_off(dkeep);
		}
		args[2] = dkeep ? 0x66 : 0xcc;
		if (ioctl(fd, HDIO_DRIVE_CMD, &args))
			perror(" HDIO_DRIVE_CMD(keepsettings) failed");
	}
	if (set_defects) {
		unsigned char args[4] = {WIN_SETFEATURES,0,0x04,0};
		no_scsi();
		args[2] = defects ? 0x04 : 0x84;
		if (get_defects)
			printf(" setting drive defect-mgmt to %ld\n", defects);
		if (ioctl(fd, HDIO_DRIVE_CMD, &args))
			perror(" HDIO_DRIVE_CMD(defectmgmt) failed");
	}
	if (set_prefetch) {
		unsigned char args[4] = {WIN_SETFEATURES,0,0xab,0};
		no_scsi();
		no_xt();
		args[1] = prefetch;
		if (get_prefetch)
			printf(" setting drive prefetch to %ld\n", prefetch);
		if (ioctl(fd, HDIO_DRIVE_CMD, &args))
			perror(" HDIO_DRIVE_CMD(setprefetch) failed");
	}
	if (set_xfermode) {
		unsigned char args[4] = {WIN_SETFEATURES,0,3,0};
		no_scsi();
		no_xt();
		args[1] = xfermode;
		if (get_xfermode) {
			printf(" setting xfermode to %ld", xfermode);
			interpret_xfermode(xfermode);
		}
		if (ioctl(fd, HDIO_DRIVE_CMD, &args))
			perror(" HDIO_DRIVE_CMD(setxfermode) failed");
	}
	if (set_accoustic) {
		unsigned char args[4] = {WIN_SETFEATURES,0,0xC2,0};
		no_scsi();
		no_xt();
		args[1] = accoustic ? accoustic : 0;
		args[2] = accoustic ? 0x42 : 0xC2;
		if (get_accoustic) {
			printf(" setting accoustic management to %ld\n", accoustic);
		}
		if (ioctl(fd, HDIO_DRIVE_CMD, &args))
			perror(" HDIO_DRIVE_CMD(set_accoustic) failed");
	}
	if (set_lookahead) {
		unsigned char args[4] = {WIN_SETFEATURES,0,0,0};
		no_scsi();
		no_xt();
		args[2] = lookahead ? 0xaa : 0x55;
		if (get_lookahead) {
			printf(" setting drive read-lookahead to %ld", lookahead);
			on_off(lookahead);
		}
		if (ioctl(fd, HDIO_DRIVE_CMD, &args))
			perror(" HDIO_DRIVE_CMD(setreadahead) failed");
	}
#ifdef CDROM_SELECT_SPEED
	if (set_cdromspeed) {
		printf ("setting cdrom speed to %ld\n", cdromspeed);
		if (ioctl (fd, CDROM_SELECT_SPEED, cdromspeed))
			perror(" CDROM_SELECT_SPEED failed");
	}
#endif                                    
	if (set_wcache) {
#ifdef DO_FLUSHCACHE
#ifndef WIN_FLUSHCACHE
#define WIN_FLUSHCACHE 0xe7
#endif
		unsigned char flushcache[4] = {WIN_FLUSHCACHE,0,0,0};
#endif /* DO_FLUSHCACHE */
		unsigned char args[4] = {WIN_SETFEATURES,0,0,0};
		no_scsi();
		no_xt();
		args[2] = wcache ? 0x02 : 0x82;
		if (get_wcache) {
			printf(" setting drive write-caching to %ld", wcache);
			on_off(wcache);
		}
#ifdef DO_FLUSHCACHE
		if (!wcache && ioctl(fd, HDIO_DRIVE_CMD, &flushcache))
			perror (" HDIO_DRIVE_CMD(flushcache) failed");
#endif /* DO_FLUSHCACHE */
		if (ioctl(fd, HDIO_DRIVE_CMD, &args))
			perror(" HDIO_DRIVE_CMD(setcache) failed");
#ifdef DO_FLUSHCACHE
		if (!wcache && ioctl(fd, HDIO_DRIVE_CMD, &flushcache))
			perror (" HDIO_DRIVE_CMD(flushcache) failed");
#endif /* DO_FLUSHCACHE */
	}
	if (set_standbynow) {
#ifndef WIN_STANDBYNOW1
#define WIN_STANDBYNOW1 0xE0
#endif
#ifndef WIN_STANDBYNOW2
#define WIN_STANDBYNOW2 0x94
#endif
		unsigned char args1[4] = {WIN_STANDBYNOW1,0,0,0};
		unsigned char args2[4] = {WIN_STANDBYNOW2,0,0,0};
		no_scsi();
		if (get_standbynow)
			printf(" issuing standby command\n");
		if (ioctl(fd, HDIO_DRIVE_CMD, &args1)
		 && ioctl(fd, HDIO_DRIVE_CMD, &args2))
			perror(" HDIO_DRIVE_CMD(standby) failed");
	}
	if (set_sleepnow) {
#ifndef WIN_SLEEPNOW1
#define WIN_SLEEPNOW1 0xE6
#endif
#ifndef WIN_SLEEPNOW2
#define WIN_SLEEPNOW2 0x99
#endif
		unsigned char args1[4] = {WIN_SLEEPNOW1,0,0,0};
		unsigned char args2[4] = {WIN_SLEEPNOW2,0,0,0};
		no_scsi();
		if (get_sleepnow)
			printf(" issuing sleep command\n");
		if (ioctl(fd, HDIO_DRIVE_CMD, &args1)
		 && ioctl(fd, HDIO_DRIVE_CMD, &args2))
			perror(" HDIO_DRIVE_CMD(sleep) failed");
	}
	if (set_seagate) {
		unsigned char args[4] = {0xfb,0,0,0};
		no_scsi();
		no_xt();
		if (get_seagate)
			printf(" disabling Seagate auto powersaving mode\n");
		if (ioctl(fd, HDIO_DRIVE_CMD, &args))
			perror(" HDIO_DRIVE_CMD(seagatepwrsave) failed");
	}
	if (set_standby) {
		unsigned char args[4] = {WIN_SETIDLE1,standby,0,0};
		no_scsi();
		no_xt();
		if (get_standby) {
			printf(" setting standby to %ld", standby);
			interpret_standby(standby);
		}
		if (ioctl(fd, HDIO_DRIVE_CMD, &args))
			perror(" HDIO_DRIVE_CMD(setidle1) failed");
	}
#else	/* HDIO_DRIVE_CMD */
	if (force_operation) {
		char buf[512];
		flush_buffer_cache(fd);
		if (-1 == read(fd, buf, sizeof(buf)))
			perror(" access failed");
	}
#endif	/* HDIO_DRIVE_CMD */

	if (!flagcount)
		verbose = 1;

	if ((verbose && !is_scsi_hd && !is_xt_hd) || get_mult || get_identity) {
		no_scsi();
		multcount = -1;
		if (ioctl(fd, HDIO_GET_MULTCOUNT, &multcount)) {
			if ((verbose && !is_xt_hd) || get_mult)
				perror(" HDIO_GET_MULTCOUNT failed");
		} else if (verbose | get_mult) {
			printf(" multcount    = %2ld", multcount);
			on_off(multcount);
		}
	}
#ifdef HDIO_GET_32BIT
	if ((verbose && !is_scsi_hd && !is_xt_hd) || get_io32bit) {
		no_scsi();
		no_xt();
		if (ioctl(fd, HDIO_GET_32BIT, &parm))
			perror(" HDIO_GET_32BIT failed");
		else {
			printf(" I/O support  =%3ld (", parm);
			switch (parm) {
				case 0:	printf("default ");
				case 2: printf("16-bit)\n");
					break;
				case 1:	printf("32-bit)\n");
					break; 
				case 3:	printf("32-bit w/sync)\n");
					break; 
				default:printf("???)\n");
			}
		}
	}
#endif
	if ((verbose && !is_scsi_hd && !is_xt_hd) || get_unmask) {
		no_scsi();
		no_xt();
		if (ioctl(fd, HDIO_GET_UNMASKINTR, &parm))
			perror(" HDIO_GET_UNMASKINTR failed");
		else {
			printf(" unmaskirq    = %2ld", parm);
			on_off(parm);
		}
	}
#ifdef HDIO_GET_DMA
	if ((verbose && !is_scsi_hd) || get_dma) {
		no_scsi();
		if (ioctl(fd, HDIO_GET_DMA, &parm))
			perror(" HDIO_GET_DMA failed");
		else {
			printf(" using_dma    = %2ld", parm);
			on_off(parm);
		}
	}
#endif
#ifdef HDIO_GET_KEEPSETTINGS
	if ((verbose && !is_scsi_hd && !is_xt_hd) || get_keep) {
		no_scsi();
		no_xt();
		if (ioctl(fd, HDIO_GET_KEEPSETTINGS, &parm))
			perror(" HDIO_GET_KEEPSETTINGS failed");
		else {
			printf(" keepsettings = %2ld", parm);
			on_off(parm);
		}
	}
#endif /* HDIO_GET_KEEPSETTINGS */

#ifdef HDIO_SET_NOWERR
	if ((verbose && !is_scsi_hd && !is_xt_hd) || get_nowerr) {
		no_scsi();
		no_xt();
		if (ioctl(fd, HDIO_GET_NOWERR, &parm))
			perror(" HDIO_GET_NOWERR failed");
		else {
			printf(" nowerr       = %2ld", parm);
			on_off(parm);
		}
	}
#endif
	if (verbose || get_readonly) {
		if (ioctl(fd, BLKROGET, &parm))
			perror(" BLKROGET failed");
		else {
			printf(" readonly     = %2ld", parm);
			on_off(parm);
		}
	}
	if ((verbose && !is_scsi_hd) || get_readahead) {
		if (ioctl(fd, BLKRAGET, &parm))
			perror(" BLKRAGET failed");
		else {
			printf(" readahead    = %2ld", parm);
			on_off(parm);
		}
	}
#if 0
	if ((verbose && !is_scsi_hd) || get_wcache) {
		if (ioctl(fd, HDIO_GET_WCACHE, &parm))
			perror(" HDIO_GET_WCACHE failed");
		else {
			printf(" write cache  = %2ld", parm);
			on_off(parm);
		}
	}
	if ((verbose && !is_scsi_hd) || get_accoustic) {
		if (ioctl(fd, HDIO_GET_ACCOUSTIC, &parm))
			perror(" HDIO_GET_ACCOUSTIC failed");
		else
			printf(" accoustic    = %2ld", parm);
	}
#endif
	if (verbose || get_geom) {
		static struct hd_geometry g;
		if (ioctl(fd, BLKGETSIZE, &parm))
			perror(" BLKGETSIZE failed");
		else if (ioctl(fd, HDIO_GETGEO, &g))
			perror(" HDIO_GETGEO failed");
		else	printf(" geometry     = %d/%d/%d, sectors = %ld, start = %ld\n",
				g.cylinders, g.heads, g.sectors, parm, g.start);
	}
#ifdef HDIO_DRIVE_CMD
	if (get_powermode) {
#ifndef WIN_CHECKPOWERMODE1
#define WIN_CHECKPOWERMODE1 0xE5
#endif
#ifndef WIN_CHECKPOWERMODE2
#define WIN_CHECKPOWERMODE2 0x98
#endif
		unsigned char args[4] = {WIN_CHECKPOWERMODE1,0,0,0};
		const char *state;
		no_scsi();
		if (ioctl(fd, HDIO_DRIVE_CMD, &args)
		 && (args[0] = WIN_CHECKPOWERMODE2) /* try again with 0x98 */
		 && ioctl(fd, HDIO_DRIVE_CMD, &args)) {
			if (errno != EIO || args[0] != 0 || args[1] != 0)
				state = "unknown";
			else
				state = "sleeping";
		} else {
			state = (args[2] == 255) ? "active/idle" : "standby";
		} 
		printf(" drive state is:  %s\n", state);
	}
#endif
#ifdef HDIO_DRIVE_RESET
	if (perform_reset) {
		no_scsi();
		no_xt();
		if (ioctl(fd, HDIO_DRIVE_RESET, NULL))
			 perror(" HDIO_DRIVE_RESET failed");
	}
#endif /* HDIO_DRIVE_RESET */
#ifdef HDIO_TRISTATE_HWIF
	if (perform_tristate) {
		unsigned char args[4] = {0,tristate,0,0};
		no_scsi();
		no_xt();
		if (ioctl(fd, HDIO_TRISTATE_HWIF, &args))
			perror(" HDIO_TRISTATE_HWIF failed");
	}
#endif /* HDIO_TRISTATE_HWIF */
	if (get_identity) {
		no_scsi();
		no_xt();
#ifdef HDIO_OBSOLETE_IDENTITY
		if (!(old_identity = ioctl(fd, HDIO_GET_IDENTITY, &id)) || !ioctl(fd, HDIO_OBSOLETE_IDENTITY)) {
#else
		if (!ioctl(fd, HDIO_GET_IDENTITY, &id)) {
			old_identity = 1;
#endif
			if (can_geometry(((unsigned char) ((id.config >> 8) & 0x1f))))
				get_geometry(fd, &r, &c, &d, &parm);
			if (multcount != -1) {
				id.multsect = multcount;
				id.multsect_valid |= 1;
			} else
				id.multsect_valid &= ~1;
			dump_identity(&id, &r, &c, &d);
		} else if (errno == -ENOMSG)
			printf(" no identification info available\n");
		else
			perror(" HDIO_GET_IDENTITY failed");
	}
#ifdef HDIO_SET_32BIT
	if (get_IDentity) {
		unsigned char args[4+512] = {WIN_IDENTIFY,0,0,1,};
		no_scsi();
		no_xt();
		if (!(ioctl(fd, HDIO_DRIVE_CMD, &args))) {
			struct hd_driveid *id = (struct hd_driveid *)&args[4];
			old_identity = 0;
			if (can_geometry(((unsigned char) ((id->config >> 8) & 0x1f))))
				get_geometry(fd, &r, &c, &d, &parm);
			dump_identity((struct hd_driveid *)&args[4], &r, &c, &d);
		} else {
			args[0] = WIN_PIDENTIFY;
			if (!(ioctl(fd, HDIO_DRIVE_CMD, &args))) {
				struct hd_driveid *id = (struct hd_driveid *)&args[4];
				old_identity = 0;
				if (can_geometry(((unsigned char) ((id->config >> 8) & 0x1f))))
					get_geometry(fd, &r, &c, &d, &parm);
				dump_identity((struct hd_driveid *)&args[4], &r, &c, &d);
			} else
				perror(" HDIO_DRIVE_CMD(identify) failed");
		}
	}
#endif
	if (do_ctimings)
		time_cache (fd);
	if (do_timings)
		time_device (fd);
	if (do_flush)
		flush_buffer_cache (fd);
	close (fd);
}

void usage_error (void)
{
	fprintf(stderr,"\n%s - get/set hard disk parameters - version %s\n\n", progname, VERSION);
	fprintf(stderr,"Usage:  %s  [options] [device] ..\n\n", progname);
	fprintf(stderr,"Options:\n"
	" -a   get/set fs readahead\n"
#ifdef HDIO_DRIVE_CMD
	" -A   set drive read-lookahead flag (0/1)\n"
#endif
#ifdef HDIO_SET_32BIT
	" -c   get/set IDE 32-bit IO setting\n"
#endif
#ifdef HDIO_DRIVE_CMD
	" -C   check IDE power mode status\n"
#endif
#ifdef HDIO_SET_DMA
	" -d   get/set using_dma flag\n"
#endif
	" -D   enable/disable drive defect-mgmt\n"
#ifdef CDROM_SELECT_SPEED
	" -E   set cd-rom drive speed\n"
#endif
	" -f   flush buffer cache for device on exit\n"
	" -g   display drive geometry\n"
	" -h   display terse usage information\n"
	" -i   display drive identification\n"
#ifdef HDIO_SET_32BIT
	" -I   read drive identification directly from drive\n"
#endif
#ifdef HDIO_GET_KEEPSETTINGS
	" -k   get/set keep_settings_over_reset flag (0/1)\n"
#endif
#ifdef HDIO_DRIVE_CMD
	" -K   set drive keep_features_over_reset flag (0/1)\n"
	" -L   set drive doorlock (0/1) (removable harddisks only)\n"
#endif
	" -m   get/set multiple sector count\n"
#ifdef HDIO_SET_NOWERR 
	" -n   get/set ignore-write-errors flag (0/1)\n"
#endif
#ifdef HDIO_SET_PIO_MODE
	" -p   set PIO mode on IDE interface chipset (0,1,2,3,4,...)\n"
#endif
#ifdef HDIO_DRIVE_CMD
	" -P   set drive prefetch count\n"
#endif
	" -q   change next setting quietly\n"
	" -r   get/set readonly flag (DANGEROUS to set)\n"
#ifdef HDIO_SCAN_HWIF
	" -R   register an IDE interface (DANGEROUS)\n"
#endif
#ifdef HDIO_DRIVE_CMD
	" -S   set standby (spindown) timeout\n"
#endif
	" -t   perform device read timings\n"
	" -T   perform cache read timings\n"
	" -u   get/set unmaskirq flag (0/1)\n"
#ifdef HDIO_UNREGISTER_HWIF
	" -U   un-register an IDE interface (DANGEROUS)\n"
#endif
	" -v   default; same as -acdgkmnru (-gr for SCSI, -adgr for XT)\n"
	" -V   display program version and exit immediately\n"
#ifdef HDIO_DRIVE_CMD
#ifdef HDIO_DRIVE_RESET
	" -w   perform device reset (DANGEROUS)\n"
#endif
	" -W   set drive write-caching flag (0/1) (DANGEROUS)\n"
#ifdef HDIO_TRISTATE_HWIF
	" -x   perform device for hotswap flag (0/1) (DANGEROUS)\n"
#endif
	" -X   set IDE xfer mode (DANGEROUS)\n"
	" -y   put IDE drive in standby mode\n"
	" -Y   put IDE drive to sleep\n"
	" -Z   disable Seagate auto-powersaving mode\n"
	" -z   get/set automatic acoustic management mode (DANGEROUS)\n"
#endif
	);
	exit(1);
}

#define GET_NUMBER(flag,num)	num = 0; \
				if (!*p && argc && isdigit(**argv)) \
					p = *argv++, --argc; \
				while (isdigit(*p)) { \
					flag = 1; \
					num = (num * 10) + (*p++ - '0'); \
				}

int main(int argc, char **argv)
{
	char c, *p;

	if  ((progname = (char *) strrchr(*argv, '/')) == NULL)
                progname = *argv;
        else
                progname++;
	++argv;

	if (!--argc)
		usage_error();
	while (argc--) {
		p = *argv++;
		if (*p == '-') {
			if (!*++p)
				usage_error();
			while ((c = *p++)) {
				++flagcount;
				switch (c) {
					case 'V':
						fprintf(stdout, "%s %s\n", progname, VERSION);
						exit(0);
						break;
					case 'v':
						verbose = 1;
						break;
#ifdef HDIO_SET_32BIT
					case 'I':
						get_IDentity = 1;
						break;
#endif
					case 'i':
						get_identity = 1;
						break;
					case 'g':
						get_geom = 1;
						break;
					case 'f':
						do_flush = 1;
						break;
					case 'q':
						quiet = 1;
						noisy = 0;
						break;
					case 'u':
						get_unmask = noisy;
						noisy = 1;
						if (!*p && argc && isdigit(**argv))
							p = *argv++, --argc;
						if (*p == '0' || *p == '1') {
							set_unmask = 1;
							unmask = *p++ - '0';
						}
						break;
#ifdef HDIO_SET_DMA
					case 'd':
						get_dma = noisy;
						noisy = 1;
						if (!*p && argc && isdigit(**argv))
							p = *argv++, --argc;
						if (*p == '0' || *p == '1') {
							set_dma = 1;
							dma = *p++ - '0';
						}
						break;
#endif
#ifdef HDIO_SET_NOWERR
					case 'n':
						get_nowerr = noisy;
						noisy = 1;
						if (!*p && argc && isdigit(**argv))
							p = *argv++, --argc;
						if (*p == '0' || *p == '1') {
							set_nowerr = 1;
							nowerr = *p++ - '0';
						}
						break;
#endif
#ifdef HDIO_SET_PIO_MODE
					case 'p':
						noisy_piomode = noisy;
						noisy = 1;
						if (!*p && argc && isdigit(**argv))
							p = *argv++, --argc;
						if (*p >= '0' && *p <= '9')
							piomode = *p++ - '0';
						else
							piomode = 255;  /* auto-tune */
						set_piomode = 1;
						break;
#endif
					case 'r':
						get_readonly = noisy;
						noisy = 1;
						if (!*p && argc && isdigit(**argv))
							p = *argv++, --argc;
						if (*p == '0' || *p == '1') {
							set_readonly = 1;
							readonly = *p++ - '0';
						}
						break;
					case 'm':
						get_mult = noisy;
						noisy = 1;
						GET_NUMBER(set_mult,mult);
						break;
#ifdef HDIO_SET_32BIT
					case 'c':
						get_io32bit = noisy;
						noisy = 1;
						GET_NUMBER(set_io32bit,io32bit);
						break;
#endif /* HDIO_SET_32BIT */
#ifdef HDIO_DRIVE_CMD
					case 'S':
						get_standby = noisy;
						noisy = 1;
						GET_NUMBER(set_standby,standby);
						if (!set_standby)
							fprintf(stderr, "-S: missing value\n");
						break;

					case 'D':
						get_defects = noisy;
						noisy = 1;
						GET_NUMBER(set_defects,defects);
						if (!set_defects)
							fprintf(stderr, "-D: missing value\n");
						break;
#ifdef CDROM_SELECT_SPEED
					case 'E':
						set_cdromspeed = 1;
						GET_NUMBER(set_cdromspeed,cdromspeed);
						if (!set_cdromspeed)
							fprintf (stderr, "-s: missing value\n");
						break;
#endif /* CDROM_SELECT_SPEED */
					case 'P':
						get_prefetch = noisy;
						noisy = 1;
						GET_NUMBER(set_prefetch,prefetch);
						if (!set_prefetch)
							fprintf(stderr, "-P: missing value\n");
						break;

					case 'X':
						get_xfermode = noisy;
						noisy = 1;
						GET_NUMBER(set_xfermode,xfermode);
						if (!set_xfermode)
							fprintf(stderr, "-X: missing value\n");
						break;

					case 'K':
						get_dkeep = noisy;
						noisy = 1;
						if (!*p && argc && isdigit(**argv))
							p = *argv++, --argc;
						if (*p == '0' || *p == '1') {
							set_dkeep = 1;
							dkeep = *p++ - '0';
						} else
							fprintf(stderr, "-K: missing value (0/1)\n");
						break;

					case 'A':
						get_lookahead = noisy;
						noisy = 1;
						if (!*p && argc && isdigit(**argv))
							p = *argv++, --argc;
						if (*p == '0' || *p == '1') {
							set_lookahead = 1;
							lookahead = *p++ - '0';
						} else
							fprintf(stderr, "-A: missing value (0/1)\n");
						break;

					case 'L':
						get_doorlock = noisy;
						noisy = 1;
						if (!*p && argc && isdigit(**argv))
							p = *argv++, --argc;
						if (*p == '0' || *p == '1') {
							set_doorlock = 1;
							doorlock = *p++ - '0';
						} else
							fprintf(stderr, "-L: missing value (0/1)\n");
						break;

					case 'W':
						get_wcache = noisy;
						noisy = 1;
						if (!*p && argc && isdigit(**argv))
							p = *argv++, --argc;
						if (*p == '0' || *p == '1') {
							set_wcache = 1;
							wcache = *p++ - '0';
						} else
							fprintf(stderr, "-W: missing value (0/1)\n");
						break;

					case 'C':
						get_powermode = noisy;
						noisy = 1;
						break;

					case 'y':
						get_standbynow = noisy;
						noisy = 1;
						set_standbynow = 1;
						break;

					case 'Y':
						get_sleepnow = noisy;
						noisy = 1;
						set_sleepnow = 1;
						break;

					case 'Z':
						get_seagate = noisy;
						noisy = 1;
						set_seagate = 1;
						break;
#endif /* HDIO_DRIVE_CMD */
#ifdef HDIO_GET_KEEPSETTINGS
					case 'k':
						get_keep = noisy;
						noisy = 1;
						if (!*p && argc && isdigit(**argv))
							p = *argv++, --argc;
						if (*p == '0' || *p == '1') {
							set_keep = 1;
							keep = *p++ - '0';
						}
						break;
#endif /* HDIO_GET_KEEPSETTINGS */
#ifdef HDIO_UNREGISTER_HWIF
					case 'U':
						if (!*p && argc && isdigit(**argv))
							p = *argv++, --argc;

						if(! p) {
							fprintf(stderr, "expected hwif_nr\n");
							exit(1);
						}

						sscanf(p++, "%i", &hwif);

						unregister_hwif = 1;
						break;
#endif /* HDIO_UNREGISTER_HWIF */
#ifdef HDIO_SCAN_HWIF
					case 'R':
						if (!*p && argc && isdigit(**argv))
							p = *argv++, --argc;

						if(! p) {
							fprintf(stderr, "expected hwif_data\n");
							exit(1);
						}

						sscanf(p++, "%i", &hwif_data);

						if (argc && isdigit(**argv))
							p = *argv++, --argc;
						else {
							fprintf(stderr, "expected hwif_ctrl\n");
							exit(1);
						}

						sscanf(p, "%i", &hwif_ctrl);

						if (argc && isdigit(**argv))
							p = *argv++, --argc;
						else {
							fprintf(stderr, "expected hwif_irq\n");
							exit(1);
						}

						sscanf(p, "%i", &hwif_irq);

						*p = '\0';

						scan_hwif = 1;
						break;
#endif /* HDIO_SCAN_HWIF */
#ifdef HDIO_DRIVE_RESET
					case 'w':
						perform_reset = 1;
						break;
#endif /* HDIO_DRIVE_RESET */
#ifdef HDIO_TRISTATE_HWIF
					case 'x':
						if (!*p && argc && isdigit(**argv))
							p = *argv++, --argc;
						if (*p == '0' || *p == '1') {
							perform_tristate = 1;
							tristate = *p++ - '0';
						} else
							fprintf(stderr, "-x: missing value (0/1)\n");
						break;

#endif /* HDIO_TRISTATE_HWIF */
					case 'a':
						get_readahead = noisy;
						noisy = 1;
						GET_NUMBER(set_readahead,readahead);
						break;
					case 'o':
						do_odirect = 1;
						/* Fall through */
					case 't':
						do_timings = 1;
						do_flush = 1;
						break;
					case 'O':
						do_odirect = 1;
					case 'T':
						do_ctimings = 1;
						do_flush = 1;
						break;
					case 'z':
						get_accoustic = noisy;
						noisy = 1;
						GET_NUMBER(set_accoustic,accoustic);
						if (!set_accoustic)
							fprintf(stderr, "-z: missing value\n");
						break;
					case 'h':
					default:
						usage_error();
				}
			}
			if (!argc)
				usage_error();
		} else {
			process_dev (p);
		} 
	}
	exit (0);
}
