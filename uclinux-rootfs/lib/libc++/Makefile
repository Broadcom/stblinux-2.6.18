include $(ROOTDIR)/config.arch

TOOL_DIR_PREFIX=$(shell dirname $(shell which $(CC)))/..

ifneq (,$(findstring linux,$(CROSS)))
	TARGET=$(CROSS:linux-=linux)
	TARGET_ARCH ?= $(CROSS:-linux-=)
else
	ifneq (,$(findstring uclibc,$(CROSS)))
		TARGET_ARCH ?= $(CROSS:-uclibc-=)
		TARGET=$(TARGET_ARCH)-linux
	else
		echo "FIXME: Cannot determine target architecture"
		exit 99
	endif
endif


all:

clean:

distclean: clean

romfs:
	$(ROMFSINST) $(TOOL_DIR_PREFIX)/$(TARGET)-uclibc/lib/libstdc++.so.6 /lib/libstdc++.so.6
	$(ROMFSINST) -s ./libstdc++.so.6 /lib/libstdc++.so
	$(ROMFSINST) $(TOOL_DIR_PREFIX)/$(TARGET)-uclibc/lib/libgcc_s.so.1 /lib/libgcc_s.so.1
	$(ROMFSINST) -s ./libgcc_s.so.1 /lib/libgcc_s.so
