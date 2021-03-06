/* vi: set sw=4 ts=4: */
/*
 * klogctl() for uClibc
 *
 * Copyright (C) 2000-2006 Erik Andersen <andersen@uclibc.org>
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */

#include "syscalls.h"
#include <unistd.h>
#include <sys/klog.h>
#define __NR__syslog		__NR_syslog
static inline _syscall3(int, _syslog, int, type, char *, buf, int, len);
int klogctl(int type, char *buf, int len)
{
	return (_syslog(type, buf, len));
}
