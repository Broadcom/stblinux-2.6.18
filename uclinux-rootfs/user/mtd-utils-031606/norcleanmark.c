#include <sys/types.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdarg.h>
#include <stdint.h>
#include <libgen.h>
#include <ctype.h>
#include <time.h>
#include <getopt.h>
#include <sys/ioctl.h>
#include <sys/mount.h>
#include "crc32.h"

#include <mtd/mtd-user.h>
#include <mtd/jffs2-user.h>
int target_endian = __BYTE_ORDER;

#define PROGRAM "norcleanmark"
#define VERSION "1.0"

static struct jffs2_unknown_node cleanmarker;

void setup_cleanmarker(void)
{
	cleanmarker.magic = cpu_to_je16(JFFS2_MAGIC_BITMASK);
	cleanmarker.nodetype = cpu_to_je16(JFFS2_NODETYPE_CLEANMARKER);
	cleanmarker.totlen = cpu_to_je32(sizeof(cleanmarker));
	cleanmarker.hdr_crc = cpu_to_je32(crc32(0, &cleanmarker, sizeof(struct jffs2_unknown_node)-4));
}

int main(int argc, char *argv[]) 
{
	mtd_info_t meminfo;
	//erase_info_t erase;
	char isjffs2 = 1;
#ifdef MTD_USER_LARGE
	uint64_t seekPos;
	unsigned long imgsize, blkno;
#else
	unsigned long imgsize, seekPos, blkno;
#endif
	char quiet = 1;
	int fd, isNAND;
	char *mtd_device;
	char *exe_name;

	if (argc != 3) {
		printf("Usage: %s <mtd_device> <fs_image_size>\n", PROGRAM);
		printf("%s appends cleanmarkers to a JFFS2 image previously written to the NOR flash\n", PROGRAM);
		exit(-1);
	}
	exe_name = argv[0];
	mtd_device = argv[1];
	imgsize = atol(argv[2]);
	if (!quiet) {
		printf("Image size = %ld\n", imgsize);
	}
	if ((fd = open(mtd_device, O_RDWR)) < 0) {
		perror("Cannot open device: ");
		exit(-1);
	}

	if (ioctl(fd, MEMGETINFO, &meminfo) != 0) {
		fprintf(stderr, "%s: %s: unable to get MTD device info\n", exe_name, mtd_device);
		perror("ioclt(MEMGETINFO) failed");
		exit(1);
	}
	isNAND = (meminfo.type == MTD_NANDFLASH) ? 1 : 0;

	if (isNAND) {
		fprintf(stderr, "Error: Not a NOR flash\n");
		exit(-1);
	}

	
	if (isjffs2) {
		setup_cleanmarker();
	}
	blkno = ((imgsize + (meminfo.erasesize - 1))/meminfo.erasesize);
	
#ifdef MTD_USER_LARGE
	seekPos = blkno << (ffs(meminfo.erasesize)-1);
#else
	seekPos = blkno * meminfo.erasesize;
#endif
	if (!quiet) {
		printf("block to be written = %d\n", blkno);
	}
	for (; seekPos < meminfo.size; seekPos += meminfo.erasesize) {
		if (!quiet) {
#ifdef MTD_USER_LARGE
			printf("Writing at %016llx: %04x %04x %08x %08x \n", seekPos, 
				cleanmarker.magic, cleanmarker.nodetype, cleanmarker.totlen, cleanmarker.hdr_crc);
#else
			printf("Writing at %08x: %04x %04x %08x %08x \n", seekPos, 
				cleanmarker.magic, cleanmarker.nodetype, cleanmarker.totlen, cleanmarker.hdr_crc);
#endif
		}
		if (lseek (fd, (off_t) seekPos, SEEK_SET) < 0) {
			perror("lseek failed");
			//fprintf(stderr, "\n%s: %s: MTD lseek failure: %s\n", exe_name, mtd_device, strerror(errno));
		}
		if (write (fd , &cleanmarker, sizeof (cleanmarker)) != sizeof (cleanmarker)) {
			perror("write failed");
			//fprintf(stderr, "\n%s: %s: MTD write failure: %s\n", exe_name, mtd_device, strerror(errno));
		}
	}
	close(fd);
}
