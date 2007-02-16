sinclude $(ROOTDIR)/config.arch

TOOL_DIR_PREFIX=$(shell dirname $(shell which $(CC)))/..
RECURSIVE_TARGETS = all-recursive check-recursive dvi-recursive \
	html-recursive info-recursive install-data-recursive \
	install-exec-recursive install-info-recursive \
	install-recursive installcheck-recursive installdirs-recursive \
	pdf-recursive ps-recursive uninstall-info-recursive \
	uninstall-recursive

ifneq (,$(findstring linux,$(CROSS)))
	TARGET=$(CROSS:linux-=linux)
	TARGET_ARCH ?= $(CROSS:-linux-=)
else
	ifneq (,$(findstring uclibc,$(CROSS)))
		TARGET_ARCH ?= $(CROSS:-uclibc-=)
		TARGET=$(TARGET_ARCH)-linux
	else
		#echo "FIXME: Cannot determine target architecture"
		#exit 99
	endif
endif
UCLIBC_INC = -I$(ROOTDIR)/uClibc/include
LINUX_INC=-I$(ROOTDIR)/$(LINUXDIR)/include
GCC_INC=-I$(TOOL_DIR_PREFIX)/$(TARGET)/include
# THT: Order is important, as we want to use the uClibc headers first,
# and the headers from gcc last.
# BITS_PER_LONG is defined under #ifdef __KERNEL__ in 2.6 headers, so we define it here.
MY_CFLAGS += $(UCLIBC_INC) $(LINUX_INC) $(LINUX_INC)/asm/mach-generic $(GCC_INC)

# THT added disable-nls to disable dependency on dgettext
all:
	CC=$(CC) CFLAGS="$(MY_CFLAGS)" \
		./configure \
		--build=i386-linux \
		--target=$(TARGET)		\
		--host=$(TARGET)	\
#		--disable-nls \
#		--enable-shared	\
#		--with-mmap \
#		--cache-file=config.cache.brcmstb.$(TARGET_ARCH)
	make -f Makefile

clean:
	-make -f Makefile clean
	-find . -name '*.o' -exec /bin/rm -f {} \;

distclean: clean
	-make -f Makefile distclean
	-rm Makefile config.cache config.h config.status

romfs:
	$(ROMFSINST) -e CONFIG_USER_GDB_LIBBFD /usr/lib/libbfd.so
	
$(RECURSIVE_TARGETS) all-am:
	make -f Makefile $@
