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
	ln -sf simple_mocad $(ROMFSDIR)/bin/mocad
	cp -f mocacore.bin $(ROMFSDIR)/etc/moca/
