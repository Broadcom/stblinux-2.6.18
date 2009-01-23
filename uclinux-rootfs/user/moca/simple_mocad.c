/*---------------------------------------------------------------------------

    Copyright (c) 2008-2009 Broadcom Corporation                 /\
                                                          _     /  \     _
    _____________________________________________________/ \   /    \   / \_
                                                            \_/      \_/  

 Copyright (c) 2009 Broadcom Corporation
 All rights reserved.
 
 Redistribution and use of this software in source and binary forms, with or
 without modification, are permitted provided that the following conditions
 are met:
 
 * Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.
 
 * Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.
 
 * Neither the name of Broadcom Corporation nor the names of its contributors
   may be used to endorse or promote products derived from this software
   without specific prior written permission of Broadcom Corporation.
 
 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 POSSIBILITY OF SUCH DAMAGE.

 File: simple_mocad.c

 Description: Trivial MoCA daemon

 ------------------------------------------------------------------------- */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <linux/bmoca.h>

static int			moca_fd;
static struct moca_kdrv_info	kdrv_info;
static struct moca_fw_img	fw_img;
static int			link_state = 0;

#define BUF_SIZE		512

#define IE_ASSERT		0x8000
#define IE_LINK_UP_STATE	0x6003
#define IE_WDT			MOCA_IE_WDT

void die(char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	vprintf(fmt, ap);
	printf(" (errno is %d)\n", errno);
	va_end(ap);
	exit(1);
}

static int get_resp(int fd, uint8_t *buf, int timeout)
{
	struct timeval tv;
	fd_set fds;
	int ret;

	tv.tv_sec = timeout;
	tv.tv_usec = 0;

	FD_ZERO(&fds);
	FD_SET(fd, &fds);
	ret = select(fd + 1, &fds, NULL, NULL, &tv);
	if(ret < 1)
		return(-1);
	ret = read(fd, buf, BUF_SIZE);
	if(ret <= 0)
		return(-1);
	return(ret);
}

static int moca_err(uint8_t *buf)
{
	uint16_t ie = (buf[4] << 8) | buf[5];

	if((ie & 0xff00) == IE_ASSERT)
		return(1);
	if(ie == IE_WDT)
		return(1);
	if(ie == IE_LINK_UP_STATE) {
		int new_state = buf[11];
		if(link_state != new_state) {
			link_state = new_state;
			printf("MOCA: Link is %s\n", new_state ? "up" : "down");
		}
	}
	return(0);
}

#define MACPOS			0x28

static void run_moca(void)
{
	uint8_t init_time_cmd[] = {
		0x01, 0x00, 0x00, 0x01, 0x10, 0x01, 0x00, 0x50,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0xff, 0x00,
		0x00, 0x00, 0xde, 0xad, 0xc0, 0xef, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xf9,
	};
	uint8_t trace_enable_cmd[] = {
		0x01, 0x00, 0x00, 0x01, 0x20, 0x17, 0x00, 0x04,
		0x00, 0x00, 0x00, 0x00,
	};
	uint8_t start_cmd[] = {
		0x01, 0x00, 0x00, 0x01, 0x10, 0x02, 0x00, 0x00,
	};
	uint8_t status_cmd[] = {
		0x06, 0x00, 0x00, 0x01, 0x30, 0x00, 0x00, 0x00,
	};
	uint8_t buf[BUF_SIZE];
	time_t last_ack = 0;

	init_time_cmd[MACPOS + 0] = (kdrv_info.macaddr_hi >> 24) & 0xff;
	init_time_cmd[MACPOS + 1] = (kdrv_info.macaddr_hi >> 16) & 0xff;
	init_time_cmd[MACPOS + 2] = (kdrv_info.macaddr_hi >>  8) & 0xff;
	init_time_cmd[MACPOS + 3] = (kdrv_info.macaddr_hi >>  0) & 0xff;
	init_time_cmd[MACPOS + 4] = (kdrv_info.macaddr_lo >> 24) & 0xff;
	init_time_cmd[MACPOS + 5] = (kdrv_info.macaddr_lo >> 16) & 0xff;

	if(ioctl(moca_fd, MOCA_IOCTL_START, &fw_img) < 0)
		die("Can't upload firmware image");
	
	if(get_resp(moca_fd, buf, 3) < 0)
		die("Timeout waiting for MOCA_READY");
	
	write(moca_fd, init_time_cmd, sizeof(init_time_cmd));
	if(get_resp(moca_fd, buf, 1) < 0)
		die("Timeout waiting for INIT_TIME ack");
	
	write(moca_fd, trace_enable_cmd, sizeof(trace_enable_cmd));
	if(get_resp(moca_fd, buf, 1) < 0)
		die("Timeout waiting for TRACE_ENABLE ack");
	
	write(moca_fd, start_cmd, sizeof(start_cmd));
	if(get_resp(moca_fd, buf, 1) < 0)
		die("Timeout waiting for MOCA_START ack");
	
	while(1) {
		if(time(NULL) > (last_ack + 5)) {
			write(moca_fd, status_cmd, sizeof(status_cmd));
			if(get_resp(moca_fd, buf, 1) < 0) {
				printf("MOCA: No ACK to status cmd\n");
				break;
			}
			if(moca_err(buf)) {
				printf("MOCA: Error detected\n");
				break;
			}
			last_ack = time(NULL);
		}
		if(get_resp(moca_fd, buf, 1) > 0) {
			if(moca_err(buf)) {
				printf("MOCA: Error detected\n");
				break;
			}
		}
	}
	if(ioctl(moca_fd, MOCA_IOCTL_STOP, NULL) < 0)
		die("Can't stop MoCA core");
}

int main(int argc, char **argv)
{
	int fd;

	moca_fd = open("/dev/bmoca0", O_RDWR);
	if(moca_fd < 0)
		die("Can't open MoCA device");
	if(ioctl(moca_fd, MOCA_IOCTL_GET_DRV_INFO, &kdrv_info) < 0)
		die("MOCA_IOCTL_GET_DRV_INFO failed");
	
	fd = open("/etc/moca/mocacore.bin", O_RDONLY);
	if(fd < 0)
		die("Can't open firmware image");
	fw_img.img_len = lseek(fd, 0, SEEK_END);
	fw_img.img = mmap(NULL, fw_img.img_len, PROT_READ, MAP_SHARED, fd, 0);
	if(fw_img.img == NULL)
		die("Can't mmap firmware image");
	close(fd);

	printf("MOCA: Starting up...\n");
	while(1) {
		link_state = 0;
		run_moca();
		printf("MOCA: Restarting...\n");
	}
	
	return(0);
}
