include $(ROOTDIR)/config.arch

CC			:= $(MACHINE)-linux-gcc
STRIP			:= $(MACHINE)-linux-strip

unexport CFLAGS

PASSTHRU		:= distclean distclean-local \
	lib/ext2fs/ext2_types.h lib/blkid/blkid_types.h lib/uuid/uuid_types.h

.PHONY: all
all:
	if [ ! -e Makefile ]; then \
		CC=$(CC) ./configure --host=$(MACHINE)-linux || exit 1 ; \
	fi
	$(MAKE) -f Makefile libs progs CFLAGS="-Os"

.PHONY: $(PASSTHRU)
$(PASSTHRU):
	$(MAKE) -f Makefile $@

.PHONY: clean
clean:
	$(MAKE) -f Makefile clean
	$(MAKE) -f Makefile distclean

.PHONY: romfs

romfs:
	$(ROMFSINST) -e CONFIG_USER_E2FSPROGS_DEBUGFS_DEBUGFS debugfs/debugfs /bin/debugfs
	$(ROMFSINST) -e CONFIG_USER_E2FSPROGS_E2FSCK_E2FSCK e2fsck/e2fsck /bin/e2fsck
		$(ROMFSINST) -e CONFIG_USER_E2FSPROGS_E2FSCK_E2FSCK -s /bin/e2fsck /bin/fsck.ext3
		$(ROMFSINST) -e CONFIG_USER_E2FSPROGS_E2FSCK_E2FSCK -s /bin/e2fsck /bin/fsck.ext2
	$(ROMFSINST) -e CONFIG_USER_E2FSPROGS_MISC_BADBLOCKS misc/badblocks /bin/badblocks
	$(ROMFSINST) -e CONFIG_USER_E2FSPROGS_MISC_CHATTR misc/ /bin/chattr
	$(ROMFSINST) -e CONFIG_USER_E2FSPROGS_MISC_DUMPE2FS misc/dumpe2fs /bin/dumpe2fs
	$(ROMFSINST) -e CONFIG_USER_E2FSPROGS_MISC_E2LABEL misc/e2label /bin/e2label
	$(ROMFSINST) -e CONFIG_USER_E2FSPROGS_MISC_FSCK misc/fsck /bin/fsck
	$(ROMFSINST) -e CONFIG_USER_E2FSPROGS_MISC_LSATTR misc/lsattr /bin/lsattr
	$(ROMFSINST) -e CONFIG_USER_E2FSPROGS_MISC_MKE2FS misc/mke2fs /bin/mke2fs
		$(ROMFSINST) -e CONFIG_USER_E2FSPROGS_MISC_MKE2FS -s /bin/mke2fs /bin/mkfs.ext2
		$(ROMFSINST) -e CONFIG_USER_E2FSPROGS_MISC_MKE2FS -s /bin/mke2fs /bin/mkfs.ext3
	$(ROMFSINST) -e CONFIG_USER_E2FSPROGS_MISC_MKLOST_FOUND misc/mklost+found /bin/mklost+found
	$(ROMFSINST) -e CONFIG_USER_E2FSPROGS_MISC_TUNE2FS misc/tune2fs /bin/tune2fs
	$(ROMFSINST) -e CONFIG_USER_E2FSPROGS_MISC_UUIDGEN misc/uuidgen /bin/uuidgen

