/*
 * Flash Base address for BCM97xxx boards
 *
  * Copyright (C) 2002,2003,2004,2005 Broadcom Corporation
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 * THT 10/22/2002
 */

#include <linux/module.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/config.h>

#ifdef CODE_IN_FLASH
#undef CODE_IN_FLASH
#endif

#ifndef CODE_IN_ROM
#define CODE_IN_ROM
#endif

#include <asm/brcmstb/common/brcmstb.h>

const unsigned long ROM_FLASH_BASE = CPU_PHYS_FLASH_BASE;
