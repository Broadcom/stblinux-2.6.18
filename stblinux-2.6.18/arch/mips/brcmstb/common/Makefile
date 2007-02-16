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
obj-$(CONFIG_SMP)	+= bmips-smp.o timerx.o


