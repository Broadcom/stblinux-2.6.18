obj-$(CONFIG_BCMUMAC) += bcmumac.o

bcmumac-objs := bcmunimac.o bcmmii.o boardparms.o unimac_proc.o \
	unimac_proc_entries.o

#-DDUMP_DATA -- dump data
#-DDUMP_TRACE -DDUMP_DATA  ---for debug, dump trace or dump packet data.
#-DUSE_PROC -- use proc file system
#-DCONFIG_RX_DESC_THROTTLE ---Use Rx descriptor interrupt throttling.
#-DCONFIG_NAPI	--- use NAPI (Software interrupt throttling).
#-DCONFIG_L4_CSUM	--- Use hardware L4 checksum offloading.

EXTRA_CFLAGS += -I. -DUSE_PROC
