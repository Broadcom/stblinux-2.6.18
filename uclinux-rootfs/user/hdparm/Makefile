#
# Makefile for hdparm
#
# 09/05/03 Ton Truong <ttruong@broadcom.com>
#

# __USE_GNU needed for O_DIRECT
CFLAGS += -I$(ROOTDIR)/$(LINUXDIR)/include -D__USE_GNU

.EXPORT_ALL_VARIABLES:


all: hdparm

hdparm: hdparm.o
hdparm.o: hdparm.c $(ROOTDIR)/$(LINUXDIR)/include/linux/hdreg.h

romfs:
	$(ROMFSINST) -e CONFIG_USER_HDPARM_HDPARM        /bin/hdparm

clean:
	rm -f hdparm *.o
