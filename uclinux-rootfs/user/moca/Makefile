include $(ROOTDIR)/config.arch

CFLAGS = -Os -I$(ROOTDIR)/$(LINUXDIR)/include -Wall
LDFLAGS = -Wl,--strip-all

.PHONY: all clean distclean romfs

all: simple_mocad

simple_mocad: simple_mocad.c
	$(CC) $(CFLAGS) $(LDFLAGS) $< -o $@

clean:
	rm -f simple_mocad

distclean: clean

romfs:
	mkdir -p $(ROMFSDIR)/etc/moca
	cp -f simple_mocad $(ROMFSDIR)/bin/
	cp -f mocacfg $(ROMFSDIR)/bin/
	chmod 0755 $(ROMFSDIR)/bin/mocacfg
	cp -f mocacore.bin $(ROMFSDIR)/etc/moca/
