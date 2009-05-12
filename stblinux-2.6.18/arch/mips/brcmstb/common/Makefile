#
# Makefile for the common code of Broadcom 97xxx board
# Copyright (C) 2001-2005 Broadcom Corporation
#                     Ton Truong <ttruong@broadcom.com>
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
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License version 2 as
#  published by the Free Software Foundation.
# 
#
# Note! Dependencies are done automagically by 'make dep', which also
# removes any old dependencies. DON'T put your own dependencies here
# unless it's something special (ie not a .c file).
#





obj-y	 += prom.o setup.o cfe_call.o cfe_entry.o \
		bootflash.o bootrom.o calibrate.o



obj-$(CONFIG_KGDB)	+= dbg_io.o

ifdef	CONFIG_MIPS_MT_SMTC
obj-$(CONFIG_MIPS_MT_SMTC) += bmips-mt.o
else
obj-$(CONFIG_SMP)	+= bmips-smp.o 
endif

obj-$(CONFIG_SMP)	+= timerx.o
obj-$(CONFIG_DISCONTIGMEM) += discontig.o
obj-$(CONFIG_BRCM_PM)	+= brcm-pm.o
obj-$(CONFIG_CPU_FREQ_BRCM) += cpufreq.o

# new platform-agnostic IRQ and RAC support
obj-$(CONFIG_MIPS_BCM7405B0) += irq.o rac.o
obj-$(CONFIG_MIPS_BCM7335) += irq.o rac.o
obj-$(CONFIG_MIPS_BCM3548) += irq.o rac.o
obj-$(CONFIG_MIPS_BCM7420) += irq.o
