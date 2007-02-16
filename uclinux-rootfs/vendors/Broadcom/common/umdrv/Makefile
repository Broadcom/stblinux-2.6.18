############################################################################
#     Copyright (c) 2002,2003,2004 Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: Makefile $
# $brcm_Revision: Irvine_BSEAVSW_Devel/12 $
# $brcm_Date: 2/25/04 3:18p $
#
# Module Description:
#
# Revision History:
#
# $brcm_Log: /SetTop/linux/driver/usermodedrv/Makefile $
# 
# Irvine_BSEAVSW_Devel/12   2/25/04 3:18p aram
# PR9857: added support for 7319 and fix a comment changed back the name
# of the driver to its original name.
# 
# Irvine_BSEAVSW_Devel/11   11/25/03 4:57p marcusk
# PR8740: Unified structure and collapsed driver to work on all
# platforms.  Currently tested on 97038 and 97115 platforms.
# 
# Irvine_BSEAVSW_Devel/10   10/27/03 11:49a aram
# added make rel for hydra
#
# Irvine_BSEAVSW_Devel/9   10/24/03 4:25p aram
# added the HYDRA flag
#
# Irvine_HDDemo_Devel/8   8/27/02 12:56p aram
# create release directory in case
# 
# Irvine_HDDemo_Devel/7   8/14/02 11:2a aram
# added the header and a rel target
# 2/24/04 tht updated to build with uclibc
############################################################################

PLATFORM = $(shell if $(CROSS_COMPILE)gcc -v 2>&1 | grep uclibc >/dev/null; then echo "uClibc"; else echo "glibc"; fi)
ifeq ($(PLATFORM),uClibc)
#uClibc build

CFLAGS += -O2 -Wstrict-prototypes -fomit-frame-pointer -fno-strict-aliasing  -G 0 -mno-abicalls -fno-pic -mips2 -Wa,--trap -pipe -DMODULE -mlong-calls
CFLAGS +=  -DLINUX -D__KERNEL__ -I$(ROOTDIR)/$(LINUXDIR)/include

else

# glibc build
CROSS_COMPILE 	= /opt/toolchains/mipsel/bin/mipsel-linux-
CFLAGS := -O2 -Wstrict-prototypes -fomit-frame-pointer -fno-strict-aliasing  -G 0 -mno-abicalls -fno-pic -mips2 -Wa,--trap -pipe -DMODULE -mlong-calls 

CFLAGS +=  -DLINUX -D__KERNEL__


MYDATE=$(shell date -I)
RELEASEDIRDATE= /opt/rootfs/$(MYDATE)
RELEASEDIR= /opt/rootfs/latest


LD		= $(CROSS_COMPILE)ld
CC		= $(CROSS_COMPILE)gcc
CPP		= $(CC) -E

endif

DEVDRIVER = bcmdriver

OBJS = umdrv.o

all: $(DEVDRIVER).o

$(DEVDRIVER).o: $(OBJS)
	$(LD) -r $^ -o $@

clean:
	rm -f *.o *~ core .depend depend
	rm -f ../*.o *~ ../core .depend


ifeq ($(PLATFORM),glibc)
# glibc build
install:
	mknod /dev/brcm c 30 0

rel:
	mkdir -p $(RELEASEDIRDATE)
	mkdir -p $(RELEASEDIR)
	cp $(DEVDRIVER).o $(RELEASEDIR)
	cp $(DEVDRIVER).o $(RELEASEDIRDATE)

endif

depend .depend dep:
	$(CC) $(CFLAGS) -M *.c > $@


ifeq (.depend,$(wildcard .depend))
include .depend
endif
