

SRCS = $(wildcard *.c)
OBJS   =  $(SRCS:.c=.o)

CFLAGS +=  -I.

LIBPROC = libproc.a


version.o:	version.c version.h
ifdef MINORVERSION
	$(strip $(CC) $(CFLAGS) -DVERSION=\"$(VERSION)\" \
		\-DSUBVERSION=\"$(SUBVERSION)\" \
		-DMINORVERSION=\"$(MINORVERSION)\" -c version.c)
else
	$(strip $(CC) $(CFLAGS) -DVERSION=\"$(VERSION)\" \
		\-DSUBVERSION=\"$(SUBVERSION)\" -c version.c)
endif

signals.o : signames.h

signames.h ../proc/signames.h : /$(ROOTDIR)/uClibc/include/signal.h
	$(CPP) -dM $(ROOTDIR)/uClibc/include/signal.h |	\
	grep -v SIGSTKSZ | \
	tr -s '\t ' '  ' | sort -n +2 | sed		\
	's:#define SIG\([A-Z]\+[0-9]*\) \([0-9]\+\) *\(\|/\*.*\)$$:{\
\2,"\1" },:p;d'  > signames.h


all: $(LIBPROC)

$(LIBPROC): $(OBJS)
	ar crv $@ $(OBJS)
	$(CROSS_COMPILE)ranlib $@


romfs:

clean:
	-rm -f $(OBJS) $(LIBPROC)
