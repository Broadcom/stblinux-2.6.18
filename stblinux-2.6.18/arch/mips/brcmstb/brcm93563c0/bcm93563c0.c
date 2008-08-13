/*
 *
 * BRIEF MODULE DESCRIPTION
 *	Bcm93563c0 Board specific code.
 *
 * Copyright 2004-2006 Broadcom Corp.
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
 */


#include <linux/config.h>
#include <asm/bootinfo.h>
#include <asm/brcmstb/common/brcmstb.h>

static int  is_reserved_node(unsigned long addr, unsigned long size, long type, void* data)
{
	struct si_bm_rsvd* pdata = (struct si_bm_rsvd*) data;
	
	if (type == BOOT_MEM_RESERVED) {
		pdata->addr = addr;
		pdata->size = size;
		return 1;
	}
	return 0;  // Next node please
}

int si_bootmem_reserved(int idx, phys_t *addr, phys_t *size)
{
#if defined (CONFIG_DISCONTIGMEM)
	struct si_bm_rsvd data;
	

	/* Find first reserved entry starting at idx */
	(void) brcm_walk_boot_mem_map(&data, is_reserved_node);

	*addr = data.addr;
	*size = data.size;

	return 0;    
#else
	return 0;
#endif
}
