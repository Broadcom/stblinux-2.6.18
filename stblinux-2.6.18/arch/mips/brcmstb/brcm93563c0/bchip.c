/*
 * arch/mips/brcmstb/brcm93563/bchip.c
 *
 * Copyright (C) 2005 Broadcom Corporation
 *                    Richard Y. Hsu<ryhsu@broadcom.com>
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * 1/30/2007 jipeng - adopt from 3560b0 
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
 */

extern unsigned int par_val2;

#define	RAC_CONFIGURATION_REGISTER	0xFF400000
#define	RAC_ADDRESS_RANGE_REGISTER	0xFF400004

int rac_setting(int value)
{
	unsigned long	rac_value;
	char 	msg[256];

	switch(value) {
		case 0:		/* RAC disabled, PF_D='0'b, PF_I='0'b, RAC_D='0'b, RAC_I='0'b */
			rac_value = 0x00000000;		
			break;

		case 1:		/* I-RAC enabled, PF_D='0'b, PF_I='1'b, RAC_D='0'b, RAC_I='1'b */
			bcm_inv_rac_all();
			rac_value = 0x00000005;	
			break;

		case 2:		/* D-RAC enabled, PF_D='1'b, PF_I='0'b, RAC_D='1'b, RAC_I='0'b */
			bcm_inv_rac_all();
			rac_value = 0x0000000A;
			break;

		case 3:		/* I/D-RAC enabled, PF_D='1'b, PF_I='1'b, RAC_D='1'b, RAC_I='1'b */
			bcm_inv_rac_all();
			rac_value = 0x0000000F;	
			break;

		default:	/* unspecified value, set to default */
			rac_value = 0x00000000;			
			sprintf(msg, "Invalid input for 3560b0 RAC mode setting. Default setting(RAC disabled) applied.\n");
			uart_puts(msg);
			break;
	}

	*((volatile unsigned long *)RAC_ADDRESS_RANGE_REGISTER) = par_val2;  /* 0x04000000; 64M for 3560b0 */
	*((volatile unsigned long *)RAC_CONFIGURATION_REGISTER) |= rac_value;

	sprintf(msg, "after init RAC 0x%08x    0x%08x\n", 
			*((volatile unsigned long *)RAC_CONFIGURATION_REGISTER), 
			*((volatile unsigned long *)RAC_ADDRESS_RANGE_REGISTER));

	uart_puts(msg);
}
