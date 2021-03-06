/*
 * Copyright (c) 2003 Silicon Graphics, Inc.  All Rights Reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it would be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Further, this software is distributed without any warranty that it is
 * free of the rightful claim of any third person regarding infringement
 * or the like.  Any license provided herein, whether implied or
 * otherwise, applies only to this software file.  Patent licenses, if
 * any, provided herein do not apply to combinations of this program with
 * other software, or any other product whatsoever.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write the Free Software Foundation, Inc., 59
 * Temple Place - Suite 330, Boston MA 02111-1307, USA.
 *
 * Contact information: Silicon Graphics, Inc., 1600 Amphitheatre Pkwy,
 * Mountain View, CA  94043, or:
 *
 * http://www.sgi.com
 *
 * For further information regarding this notice, see:
 *
 * http://oss.sgi.com/projects/GenInfo/SGIGPLNoticeExplan/
 */

#include <sys/disk.h>
#include <sys/stat.h>
#include <sys/mount.h>
#include <sys/ioctl.h>
#include <xfs/libxfs.h>

extern char *progname;

int
platform_check_ismounted(char *name, char *block, struct stat64 *s, int verbose)
{
	return 0;
}

int
platform_check_iswritable(char *name, char *block, struct stat64 *s, int fatal)
{
	int	fd, writable;

	if ((fd = open(block, O_RDONLY, 0)) < 0) {
		fprintf(stderr, _("%s: "
			"error opening the device special file \"%s\": %s\n"),
			progname, block, strerror(errno));
		exit(1);
	}

	if (ioctl(fd, DKIOCISWRITABLE, &writable) < 0) {
		fprintf(stderr, _("%s: can't tell if \"%s\" is writable: %s\n"),
			progname, block, strerror(errno));
		exit(1);
	}
	close(fd);
	return (writable == 0);
}

void
platform_set_blocksize(int fd, char *path, int blocksize)
{
}

void
platform_flush_device(int fd)
{
	ioctl(fd, DKIOCSYNCHRONIZECACHE, NULL);
}

void
platform_findsizes(char *path, int fd, long long *sz, int *bsz)
{
	__uint64_t	size;
	struct stat64	st;

	if (fstat64(fd, &st) < 0) {
		fprintf(stderr,
			_("%s: cannot stat the device file \"%s\": %s\n"),
			progname, path, strerror(errno));
		exit(1);
	}
	if ((st.st_mode & S_IFMT) == S_IFREG) {
		*sz = (long long)(st.st_size >> 9);
		*bsz = BBSIZE;
		return;
	}
	if (ioctl(fd, DKIOCGETBLOCKCOUNT, &size) < 0) {
		fprintf(stderr, _("%s: can't determine device size: %s\n"),
			progname, strerror(errno));
		exit(1);
	}
	*sz = (long long)size;
	*bsz = BBSIZE;
}
