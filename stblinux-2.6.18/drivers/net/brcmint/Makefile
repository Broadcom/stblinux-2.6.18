# File: drivers/net/bcm96345/enet/Makefile
#
# Makefile for the BCM6345 Enet/switch driver
#

O_TARGET := ../bcmemacnet.o

obj-y := bcmemac.o
obj-m := $(O_TARGET)

#-DDUMP_DATA -- dump data
#-DUSE_BH -- use bottom half
#-DUSE_PROC -- display ethernet driver internal info
# THT EXTRA_CFLAGS += -I$(HPATH)/asm/bcm96345 -I$(TOPDIR)/modulesrc/include/bcm96345
# THT for debug only EXTRA_CFLAGS += -I. -DDUMP_TRACE -DDUMP_DATA
EXTRA_CFLAGS += -I. -DUSE_PROC
#EXTRA_CFLAGS += -I. -DUSE_PROC -DDUMP_DATA
#EXTRA_CFLAGS += -I. -DUSE_PROC -DDUMP_TRACE
# THT Not Yet EXTRA_CFLAGS += -DUSE_BH

#include $(TOPDIR)/Rules.make

clean:
	rm -f core *.o *.a *.s

