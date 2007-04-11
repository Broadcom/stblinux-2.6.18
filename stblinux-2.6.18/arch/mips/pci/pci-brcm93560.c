/*
 *
 * BRIEF MODULE DESCRIPTION
 *	Bcm97038B0 PCI COntroller specific pci setup.
 *
 * Copyright 2004 Broadcom Corp.
 *
 * This file was derived from the sample file pci_ops.c
 *
 * Carsten Langgaard, carstenl@mips.com
 * Copyright (C) 1999,2000 MIPS Technologies, Inc.  All rights reserved.
 *
 *  This program is free software; you can redistribute  it and/or modify it
 *  under  the terms of  the GNU General  Public License as published by the
 *  Free Software Foundation;  either version 2 of the  License, or (at your
 *  option) any later version.
 *
 *  THIS  SOFTWARE  IS PROVIDED   ``AS  IS'' AND   ANY  EXPRESS OR IMPLIED
 *  WARRANTIES,   INCLUDING, BUT NOT  LIMITED  TO, THE IMPLIED WARRANTIES OF
 *  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN
 *  NO  EVENT  SHALL   THE AUTHOR  BE    LIABLE FOR ANY   DIRECT, INDIRECT,
 *  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 *  NOT LIMITED   TO, PROCUREMENT OF  SUBSTITUTE GOODS  OR SERVICES; LOSS OF
 *  USE, DATA,  OR PROFITS; OR  BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 *  ANY THEORY OF LIABILITY, WHETHER IN  CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 *  THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  You should have received a copy of the  GNU General Public License along
 *  with this program; if not, write  to the Free Software Foundation, Inc.,
 *  675 Mass Ave, Cambridge, MA 02139, USA.
 *
 * Revision Log
 * who when    what
 * tht  041004 Adapted from sample codes from kernel tree
 */
#include <linux/init.h>
#include <linux/types.h>
#include <linux/pci.h>
#include <linux/kernel.h>
#include <linux/ioport.h>

//#include <asm/pci_channel.h>

//extern const unsigned long mips_io_port_base;

static struct resource pci_io_resource = {
	"io pci IO space",
	0x0000000,
	0x0000000 + 0x0060000B,
	IORESOURCE_IO,
	NULL, NULL, NULL
};

static struct resource pci_mem_resource = {
	"ext pci memory space",
	0xd0000000,
	0xd0000000 + 0x08000000,
	IORESOURCE_MEM,
	NULL, NULL, NULL
};


extern struct pci_ops bcm3560_pci_ops; 

struct pci_controller bcm3560_controller = {
	.pci_ops 		= &bcm3560_pci_ops, 
	.io_resource 	= &pci_io_resource, 
	.mem_resource= &pci_mem_resource,
};

static int __init brcm93560_pci_init(void)
{
	register_pci_controller(&bcm3560_controller);
	return 0;
}

arch_initcall(brcm93560_pci_init);




