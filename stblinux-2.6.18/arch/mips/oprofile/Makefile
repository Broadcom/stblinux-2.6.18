EXTRA_CFLAGS := -Werror

obj-$(CONFIG_OPROFILE) += oprofile.o

DRIVER_OBJS = $(addprefix ../../../drivers/oprofile/, \
		oprof.o cpu_buffer.o buffer_sync.o \
		event_buffer.o oprofile_files.o \
		oprofilefs.o oprofile_stats.o \
		timer_int.o )

oprofile-y				:= $(DRIVER_OBJS) common.o

oprofile-$(CONFIG_CPU_MIPS32)		+= op_model_mipsxx.o
oprofile-$(CONFIG_CPU_MIPS64)		+= op_model_mipsxx.o
oprofile-$(CONFIG_CPU_SB1)		+= op_model_mipsxx.o
oprofile-$(CONFIG_CPU_RM9000)		+= op_model_rm9000.o
oprofile-$(CONFIG_MIPS_BCM7038)		+= op_model_brcm7038.o
oprofile-$(CONFIG_BMIPS3300)		+= op_model_brcm3300.o
oprofile-$(CONFIG_BMIPS4350)		+= op_model_brcm4350.o
oprofile-$(CONFIG_BMIPS4380)		+= op_model_brcm4380.o
oprofile-$(CONFIG_BMIPS5000)		+= op_model_brcm5000.o
