#
# Copyright (C) 2000-2006 Broadcom Corporation
#
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License version 2 as
# published by the Free Software Foundation.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
#
#
#	Makefile -- Build instructions for Broadcom/97454 (7401C0 core)
#

include $(LINUX_CONFIG)
include $(CONFIG_CONFIG)
include $(ARCH_CONFIG)

PLATFORM := 7454
VERSION := $(shell cat $(ROOTDIR)/version)
LINUX_VERSION := $(shell cat $(ROOTDIR)/$(LINUXDIR)/include/linux/version.h | grep UTS_RELEASE|cut -d" " -f3)
LINUXFILE = vmlinux
COMPRESSED_LINUXFILE = vmlinuz
ROMFSFILE = romfs.img
CRAMFSFILE= cramfs.img
SQUASHFSFILE=squashfs.img
JFFSFILE  = jffs.img
JFFS2FILE = jffs2.img
LINUXIMG  = $(IMAGEDIR)/$(PLATFORM)/$(COMPRESSED_LINUXFILE)
ROMFSIMG  = $(IMAGEDIR)/$(PLATFORM)/$(ROMFSFILE)
INITRDIMG = $(INITRDDIR)/$(PLATFORM)/ramdisk.gz
JFFSIMG   = $(IMAGEDIR)/$(PLATFORM)/$(JFFSFILE)
JFFS2IMG  = $(IMAGEDIR)/$(PLATFORM)/$(JFFS2FILE)
CRAMFSIMG = $(IMAGEDIR)/$(PLATFORM)/$(CRAMFSFILE)
SQUASHFSIMG = $(IMAGEDIR)/$(PLATFORM)/$(SQUASHFSFILE)
INITRAMFS_DIR = $(IMAGEDIR)/$(PLATFORM)/initramfs
UMDRV     = umdrv/bcmdriver.o
# For now until we make umdrv part of the kernel module build process
UMDRVDIR  := $(ROMFSDIR)/lib/modules/$(LINUX_VERSION)/drivers/umdrv
KINITDIR  = ../common/kinit
ROMFSDIR ?= $(ROOTDIR)/romfs

DIRS =

ROMFS_DIRS = bin dev etc etc/dhcpc lib lib/modules home mnt mnt/hd mnt/nfs proc root usr var sys

ifdef CONFIG_BLK_DEV_INITRD
ROMFS_DIRS += sbin tmp var/run var/log var/tmp /var/lock dev/pts usr/sbin
#FSIMG = $(INITRDIMG)
#INITRDDIR = $(ROOTDIR)/$(LINUXDIR)/arch/mips/ramdisk
endif
ifdef CONFIG_ROMFS_FS
ROMFS_DIRS += sbin
FSIMG = $(ROMFSIMG)
endif
#ifdef CONFIG_JFFS_FS
#ROMFS_DIRS += sbin tmp var/run var/log var/tmp /var/lock usr/sbin
#FSIMG = $(JFFSIMG)
#endif
ifdef CONFIG_JFFS2_FS
ROMFS_DIRS += sbin tmp var/run var/log var/tmp /var/lock usr/sbin
FSIMG += $(JFFS2IMG)
endif
ifdef CONFIG_CRAMFS
ROMFS_DIRS += sbin tmp var/run var/log var/tmp /var/lock usr/sbin
FSIMG += $(CRAMFSIMG)
endif
ifdef CONFIG_SQUASHFS
FSIMG += $(SQUASHFSIMG)
endif
ifdef CONFIG_OPROFILE
BMIPS_DIR=mips/bcm3300
TARGET_PREFIX=/usr/local/share/oprofile/
BUILD_PREFIX=$(ROOTDIR)/romfs/$(TARGET_PREFIX)
OPROFILE_DIR=$(ROOTDIR)/user/oprofile
endif

DEVICES = \
	tty,c,5,0		console,c,5,1		ptmx,c,5,2 \
	ram0,b,1,0		ram1,b,1,1		mem,c,1,1 \
	kmem,c,1,2		null,c,1,3		zero,c,1,5 \
	random,c,1,8		urandom,c,1,9 \
	\
	tty0,c,4,0		tty1,c,4,1		tty2,c,4,2 \
	tty3,c,4,3		tty4,c,4,4		tty5,c,4,5 \
	ttyS0,c,4,64		ttyS1,c,4,65 \
    	ttyS64,c,4,128 \
	\
	sda,b,8,0		sda1,b,8,1		sda2,b,8,2 \
	sda3,b,8,3		sda4,b,8,4		sda5,b,8,5 \
	sda6,b,8,6 \
	\
	sdb,b,8,16		sdb1,b,8,17		sdb2,b,8,18 \
	sdb3,b,8,19		sdb4,b,8,20		sdb5,b,8,21 \
	sdb6,b,8,22 \
	\
	sr0,b,11,0		sr1,b,11,1		\
	scd0,b,11,0		scd1,b,11,1		\
	\
	mtd0,c,90,0		mtdr0,c,90,1		mtdblock0,b,31,0 \
	mtd1,c,90,2		mtdr1,c,90,3		mtdblock1,b,31,1 \
	mtd2,c,90,4		mtdr2,c,90,5		mtdblock2,b,31,2 \
	mtd3,c,90,6		mtdr3,c,90,7		mtdblock3,b,31,3 \
	mtd4,c,90,8		mtdr4,c,90,9		mtdblock4,b,31,4 \
	mtd5,c,90,10		mtdr5,c,90,11		mtdblock5,b,31,5 \
	mtd6,c,90,12		mtdr6,c,90,13		mtdblock6,b,31,6 \
	\
        loop0,b,7,0		loop1,b,7,1		loop2,b,7,2 \
	loop3,b,7,3		loop4,b,7,4		loop5,b,7,5 \
	loop6,b,7,6		loop7,b,7,7		loop8,b,7,8 \
	loop9,b,7,9		loop10,b,7,10		loop11,b,7,11 \
	loop12,b,7,12		loop13,b,7,13		loop14,b,7,14 \
	loop15,b,7,15
# PR12239 - Remove Broadcom specific device nodes from root filesystem

all:
	dirs=$(DIRS) ; \
	for i in $$dirs ; do  make -C $$i || exit $? ; done

clean:
	-dirs=$(DIRS) ; \
	for i in $$dirs; do [ ! -d $$i ] || make -C $$i clean; done
	-make -C $(KINITDIR) clean
	rm -f $(ROOTDIR)/$(LINUXDIR)/usr/initramfs.dev
	make -C $(ROOTDIR)/$(LINUXDIR)/usr clean
	#make -C $(ROOTDIR)/user/kinit clean
	rm -rf $(IMAGEDIR)/$(PLATFORM)

romfs:
	[ -d $(ROMFSDIR)/$$i ] || mkdir -p $(ROMFSDIR)
	for i in $(ROMFS_DIRS); do \
		[ -d $(ROMFSDIR)/$$i ] || mkdir -p $(ROMFSDIR)/$$i; \
	done
	dirs=$(DIRS) ; for i in $$dirs; do  make -C $$i romfs || exit $? ; done
ifdef CONFIG_ROMFS_FS
	$(ROMFSINST) -s /var/tmp /tmp
endif
	rm -f $(ROMFSDIR)/dev/log
	cp -af /dev/log $(ROMFSDIR)/dev
	echo "$(VERSIONSTR) -- " `date` > $(ROMFSDIR)/etc/version
	date '+%Y.%m.%d-%T' > $(ROMFSDIR)/etc/build-date
	rm -rf $(ROMFSDIR)/include
	rm -rf $(ROMFSDIR)/man
	rm -rf $(ROMFSDIR)/share/aclocal
	rm -rf $(ROMFSDIR)/lib/directfb-*/gfxdrivers
	rm -rf $(ROMFSDIR)/lib/directfb-*/interfaces/IDirectFBVideoProvider

initrd: kinit
	umask 022
	cp -af etc/* $(ROMFSDIR)/etc
	sed "s,@VERSION@,$(VERSION),g" usr/sbin/stbutil | \
		sed "s,@PLATFORM@,$(PLATFORM),g" > $(ROMFSDIR)/usr/sbin/stbutil
	sed 's,###mount,mount,g' etc/init.d/rcS > $(ROMFSDIR)/var/tmp/rcS.hd
	chmod +x $(ROMFSDIR)/usr/sbin/stbutil
	cp -f usr/sbin/usb* $(ROMFSDIR)/usr/sbin/
	chmod +x $(ROMFSDIR)/usr/sbin/usb*
	cp -f usr/sbin/bridge* $(ROMFSDIR)/usr/sbin/
	chmod +x $(ROMFSDIR)/usr/sbin/bridge*
	#Enabling 2nd serial port
	# chmod u+w $(ROMFSDIR)/etc/inittab
	sed 's,#@@@@@ttyS1::respawn:/bin/getty -L ttyS1,ttyS64::respawn:/bin/getty -L ttyS64,g' \
		etc/inittab >$(ROMFSDIR)/etc/inittab.ttyS64
	cp -af var/tmp/* $(ROMFSDIR)/var/tmp
	#[ -d $(UMDRVDIR) ] || mkdir -p $(UMDRVDIR)
	#cp -f $(UMDRV) $(UMDRVDIR)
	echo "Stripping executables"
	for fo in `find $(ROMFSDIR) -type f -perm +111`; do \
		if file $$fo | grep "ELF" | grep "executable" > /dev/null; then \
			$(STRIP) $$fo; \
		fi; \
	done
	echo "Removing old devices"
	-for i in $(DEVICES); do \
		rm -f $(ROMFSDIR)/dev/`echo $$i | sed -e 's/,/ /g'` ; \
	done
	make -C $(ROOTDIR)/$(LINUXDIR)/usr clean
	# Make the dev list to allow normal user to build the kernel
	echo "Make new initramfs dev list"
	-rm $(ROOTDIR)/$(LINUXDIR)/usr/initramfs.dev
	touch $(ROOTDIR)/$(LINUXDIR)/usr/initramfs.dev
	for i in $(DEVICES); do \
		echo "nod /dev/`echo $$i |sed -e 's/,/ 0600 0 0 /' \
			|sed -e 's/,/ /g'`" >>$(ROOTDIR)/$(LINUXDIR)/usr/initramfs.dev; \
	done
	# Insert /dev/root
	echo "nod /dev/root 0600 0 0 b 1 0" >>$(ROOTDIR)/$(LINUXDIR)/usr/initramfs.dev
	# Make the symlink required by initramfs
	cp -f $(KINITDIR)/init $(ROMFSDIR)/init
ifdef CONFIG_OPROFILE
	-mkdir -p $(BUILD_PREFIX)/$(BMIPS_DIR)
	cp -f $(OPROFILE_DIR)/events/$(BMIPS_DIR)/events \
		$(BUILD_PREFIX)/$(BMIPS_DIR)/events
	cp -f $(OPROFILE_DIR)/events/$(BMIPS_DIR)/unit_masks \
		$(BUILD_PREFIX)/$(BMIPS_DIR)/unit_masks
endif


.PHONY: kinit initramfs_dir images

kinit:
	echo "Making in kinit"
	make -C $(KINITDIR)

$(UMDRV):
	# No longer needed to be built
	# make -C ./umdrv

images: $(JFFS2IMG) $(CRAMFSIMG)  $(SQUASHFSIMG)

#*************************************************************************
# Be careful, removing --no-absolute-filenames or mispelling it will trash
# your PC host.  Will refuse to run if INITRAMFS_DIR is NULL, as that
# would also hose the host by overwriting the host's root fs.
# We only check for NULL, "/" and "//" as much as we can go.
# A value of "///" etc... will still trash it
#**************************************************************************
initramfs_dir: $(IMAGEDIR)/$(PLATFORM)/initramfs_data_nodev.cpio
	@if [ ! -z "$(INITRAMFS_DIR)" -a "$(INITRAMFS_DIR)" != "/" -a "$(INITRAMFS_DIR)" != "//" ]; then \
		cat $< | (cd $(INITRAMFS_DIR); cpio -ivd --no-absolute-filenames); \
	else \
		echo "Refuse to run: INITRAMFS_DIR=\"$(INITRAMFS_DIR)\" would trash your host"; exit 1;\
	fi


# THT The MirrorBit flash chip has a 64K erase block size.
$(JFFS2IMG): initramfs_dir
	$(ROOTDIR)/user/mtd-utils/build/mkfs.jffs2 -o $(JFFS2IMG) -q -e 0x20000 -d \
		$(INITRAMFS_DIR)

$(JFFSIMG):
	$(ROOTDIR)/user/mtd-utils/build/mkfs.jffs -o $(JFFSIMG) -d $(ROMFSDIR)

# jipeng - PR22427 dhcpcd failed with cramfs
# here we create var.img to contain RW /var and change rcS to call mountvar to mount it as first step
$(CRAMFSIMG): initramfs_dir
	dd if=/dev/zero of=$(INITRAMFS_DIR)/tmpfsvar.img bs=1024 count=500
	cp -f ../common/var/tmp/mountvar.sh $(INITRAMFS_DIR)/var/tmp/.
	cp -f $(INITRAMFS_DIR)/var/tmp/mountvar.sh $(INITRAMFS_DIR)/etc
	chmod +x $(INITRAMFS_DIR)/etc/mountvar.sh
	rm -f $(INITRAMFS_DIR)/var/tmp/*
	cd $(INITRAMFS_DIR)/var; tar -cvf $(INITRAMFS_DIR)/var.img . 2>&1 >/dev/null
	$(ROOTDIR)/user/cramfs/build/mkcramfs -v -n "Cramfs" $(INITRAMFS_DIR) $(CRAMFSIMG)

$(SQUASHFSIMG): initramfs_dir
	$(ROOTDIR)/user/squashfs-tools/build/mksquashfs $(INITRAMFS_DIR) $(SQUASHFSIMG) -b 65536 -le -all-root -noappend -info


$(ROMFSIMG):
	genromfs -v -V "ROMdisk" -f $(ROMFSIMG) -d $(ROMFSDIR)

.PHONY: image $(FSIMG)
image:   # Removed.  Now build later with images target $(FSIMG)
	[ -d $(INITRAMFS_DIR) ] || mkdir -p $(INITRAMFS_DIR)
	gzip -fc $(ROOTDIR)/$(LINUXDIR)/$(LINUXFILE) > $(LINUXIMG)
	cp -f $(ROOTDIR)/$(LINUXDIR)/usr/initramfs_data.cpio.gz $(IMAGEDIR)/$(PLATFORM)
	cp -f $(ROOTDIR)/$(LINUXDIR)/usr/initramfs_data_nodev.cpio $(IMAGEDIR)/$(PLATFORM)
