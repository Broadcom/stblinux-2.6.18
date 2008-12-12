include $(ROOTDIR)/config.arch

ifneq (,$(findstring linux,$(CROSS)))
	TARGET=$(CROSS:linux-=linux)
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
	set AR=$(CROSS)ar
	set AS=$(CROSS)as
	set LD=$(CROSS)ld
	set NM=$(CROSS)nm
	set CC=$(CROSS)gcc
	set GCC=$(CROSS)gcc
	set CXX=$(CROSS)g++
	set RANLIB=$(CROSS)ranlib
	./configure --build=i686-pc-linux-gnu \
		--target=$(TARGET) 		\
		--host=$(TARGET) 		\
		--disable-nls 			\
		--disable-gdbserver 		\
		--without-included-gettext 	\
		--without-uiout 		\
		--disable-tui --disable-gdbtk --without-x 
	make -f Makefile all-gdb

clean:
	cd bfd ; make distclean ; cd ..
	cd config ; make distclean ; cd ..
	cd gdb/gdbserver ; make distclean ; cd ../
	make distclean ; cd ..
	cd intl ; make distclean ; cd ..
	cd libdecnumber/ ; make distclean ; cd ..
	cd libiberty/ ; make distclean ; cd ..
	cd opcodes/ ; make distclean ; cd ..
	cd readline/ ; make distclean ; cd ..
	cd sim ; make distclean ; cd ..
	cd texinfo/ ; make distclean ; cd ..
	find . -name "*.o" |xargs rm -f
	find . -name "*.lo" |xargs rm -f
	find . -name "*.a" |xargs rm -f
	find . -name "*.la" |xargs rm -f
	find . -name "config\.cache" |xargs rm -f
	find . -name "config\.status" |xargs rm -f
	find . -name "config\.log" |xargs rm -f
	find . -name "Makefile" |xargs rm -f
	rm -f gdb/gdb
	rm -f gdb/gdbserver/gdbserver
	rm -f gdb/gdbserver/gdbreplay

romfs:
	cp gdb/gdb gdb/gdb-strip
	$(CROSS)strip gdb/gdb-strip
	cp gdb/gdb-strip ../../romfs/usr/bin/gdb


