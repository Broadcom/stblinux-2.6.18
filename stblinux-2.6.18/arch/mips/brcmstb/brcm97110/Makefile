#
# Makefile for BCM97110 board
#
# Copyright (C) 2004-2005 Broadcom Corporation
#
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License version 2 as
# published by the Free Software Foundation.
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
# Makefile for BCM973560 board
#
# when     who what
# -------- --- ------------------------------------------------------
# 04/20/05 THT Original codings
#
# Note! Dependencies are done automagically by 'make dep', which also
# removes any old dependencies. DON'T put your own dependencies here
# unless it's something special (ie not a .c file).
#

.S.s:
	$(CPP) $(AFLAGS) $< -o $*.s
.S.o:
	$(CC) $(AFLAGS) -c $< -o $*.o

O_TARGET:= brcm97110.a

export-objs :=led.o board.o
obj-y	 += irq.o led.o hw_init.o bcm71xx_uart.o io_cs_init.o board.o


#include $(TOPDIR)/Rules.make
