#
#
# Copyright (c) 2002-2005 Broadcom Corporation 
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
#
#
# File: drivers/net/brcmint7038/Makefile
#
# Makefile for the BCM7038 Ethernet driver
#

obj-$(CONFIG_BCMINTEMAC_7038) += bcmumacnet.o

bcmumacnet-objs := bcmunimac.o bcmmii.o boardparms.o unimac_proc.o unimac_proc_entries.o


#-DDUMP_DATA -- dump data
#-DDUMP_TRACE -DDUMP_DATA  ---for debug, dump trace or dump packet data.
#-DUSE_PROC -- use proc file system
#-DCONFIG_RX_DESC_THROTTLE ---Use Rx descriptor interrupt throttling.
#-DCONFIG_NAPI	--- use NAPI (Software interrupt throttling).
#-DCONFIG_L4_CSUM	--- Use hardware L4 checksum offloading.

EXTRA_CFLAGS += -I. -DDUMP_TRACE -DCONFIG_UMAC_RX_DESC_THROTTLE -DUSE_PROC

