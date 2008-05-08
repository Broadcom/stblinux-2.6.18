
/*     Copyright (c) 1999-2005, Broadcom Corporation
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
 */

#ifndef _INT1_API_H
#define _INT1_API_H


#ifdef  __cplusplus
extern "C" {
#endif


/***************************************************************************
 *  Logical Level 1 Interrupt IDs
 **************************************************************************/

/* Level 1 Interrupt IDs */
#define INT1_ID_PCI_INTA_2_CPU_INTR    0x03e
#define INT1_ID_PCI_INTA_1_CPU_INTR    0x03d
#define INT1_ID_PCI_INTA_0_CPU_INTR    0x03c
#define INT1_ID_EXT_IRQ_6_CPU_INTR     0x03b
#define INT1_ID_EXT_IRQ_5_CPU_INTR     0x03a
#define INT1_ID_EXT_IRQ_4_CPU_INTR     0x039
#define INT1_ID_EXT_IRQ_3_CPU_INTR     0x038
#define INT1_ID_EXT_IRQ_2_CPU_INTR     0x037
#define INT1_ID_EXT_IRQ_1_CPU_INTR     0x036
#define INT1_ID_EXT_IRQ_0_CPU_INTR     0x035
#define INT1_ID_SPARE_3_CPU_INTR       0x031
#define INT1_ID_SPARE_2_CPU_INTR       0x030
#define INT1_ID_SPARE_1_CPU_INTR       0x02f
#define INT1_ID_SPARE_0_CPU_INTR       0x02e
#define INT1_ID_DVP_HR_CPU_INTR        0x02d
#define INT1_ID_DVP_DL_CPU_INTR        0x02c
#define INT1_ID_BSP_CPU_INTR           0x02b
#define INT1_ID_RPTD_CPU_INTR          0x02a
#define INT1_ID_AIO_CPU_INTR           0x029
#define INT1_ID_GFX_CPU_INTR           0x028
#define INT1_ID_GFX_MEMC_CPU_INTR      0x027
#define INT1_ID_HDMI_RX_1_CPU_INTR     0x026
#define INT1_ID_HDMI_RX_0_CPU_INTR     0x025
#define INT1_ID_BVNM_CPU_INTR          0x024
#define INT1_ID_MCIF_CPU_INTR          0x023
#define INT1_ID_USB20_OHCI_INTR        0x022
#define INT1_ID_USB20_EHCI_INTR        0x021
#define INT1_ID_USB20_BRIDGE_INTR      0x020
#define INT1_ID_VDECS_CPU_INTR         0x01f
#define INT1_ID_OB_CPU_INTR            0x01e
#define INT1_ID_TER_CPU_INTR           0x01d
#define INT1_ID_MTP_CPU_INTR           0x01c
#define INT1_ID_SUN_CPU_INTR           0x01b
#define INT1_ID_UPG_ANT_INTR           0x01a
#define INT1_ID_UPG_GPIO_INTR          0x019
#define INT1_ID_UPG_SC_CPU_INTR        0x018
#define INT1_ID_UPG_UART0_CPU_INTR     0x017
#define INT1_ID_UPG_SPI_CPU_INTR       0x016
#define INT1_ID_UPG_BSC_CPU_INTR       0x015
#define INT1_ID_UPG_CPU_INTR           0x014
#define INT1_ID_UPG_TMR_CPU_INTR       0x013
#define INT1_ID_IFD_CPU_INTR           0x012
#define INT1_ID_DS2_CPU_INTR           0x011
#define INT1_ID_DS1_CPU_INTR           0x010
#define INT1_ID_BVNF_CPU_INTR_3        0x00f
#define INT1_ID_BVNF_CPU_INTR_2        0x00e
#define INT1_ID_BVNF_CPU_INTR_1        0x00d
#define INT1_ID_BVNF_CPU_INTR_0        0x00c
#define INT1_ID_BVNB_CPU_INTR          0x00b
#define INT1_ID_VEC_CPU_INTR           0x00a
#define INT1_ID_AP_CPU_INTR            0x009
#define INT1_ID_DVO_CPU_INTR           0x008
#define INT1_ID_XPT_MSG_STAT_CPU_INTR  0x007
#define INT1_ID_XPT_FE_CPU_INTR        0x006
#define INT1_ID_XPT_PCR_CPU_INTR       0x005
#define INT1_ID_XPT_RAV_CPU_INTR       0x004
#define INT1_ID_XPT_MSG_CPU_INTR       0x003
#define INT1_ID_XPT_OVFL_CPU_INTR      0x002
#define INT1_ID_XPT_STATUS_CPU_INTR    0x001
#define INT1_ID_HIF_CPU_INTR           0x000

#ifndef __ASSEMBLY__

/***************************************************************************
 *  Level 1 ISR Type Definition
 **************************************************************************/
typedef void (*FN_L1_ISR) (void *, int);


/***************************************************************************
 *  Level 1 Interrupt Control Structure
 **************************************************************************/

typedef struct Int1Control {
unsigned long        			IntrW0Status;
#define VDECS_CPU_INTR          0x80000000
#define OB_CPU_INTR             0x40000000
#define TER_CPU_INTR            0x20000000
#define MTP_CPU_INTR            0x10000000
#define SUN_CPU_INTR            0x08000000
#define UPG_ANT_INTR            0x04000000
#define UPG_GPIO_INTR           0x02000000
#define UPG_SC_CPU_INTR         0x01000000
#define UPG_UART0_CPU_INTR      0x00800000
#define UPG_SPI_CPU_INTR        0x00400000
#define UPG_BSC_CPU_INTR        0x00200000
#define UPG_CPU_INTR            0x00100000
#define UPG_TMR_CPU_INTR        0x00080000
#define IFD_CPU_INTR            0x00040000
#define DS2_CPU_INTR            0x00020000
#define DS1_CPU_INTR            0x00010000
#define BVNF_CPU_INTR_3         0x00008000
#define BVNF_CPU_INTR_2         0x00004000
#define BVNF_CPU_INTR_1         0x00002000
#define BVNF_CPU_INTR_0         0x00001000
#define BVNB_CPU_INTR           0x00000800
#define VEC_CPU_INTR            0x00000400
#define AP_CPU_INTR             0x00000200
#define DVO_CPU_INTR            0x00000100
#define XPT_MSG_STAT_CPU_INTR   0x00000080
#define XPT_FE_CPU_INTR         0x00000040
#define XPT_PCR_CPU_INTR        0x00000020
#define XPT_RAV_CPU_INTR        0x00000010
#define XPT_MSG_CPU_INTR        0x00000008
#define XPT_OVFL_CPU_INTR       0x00000004
#define XPT_STATUS_CPU_INTR     0x00000002
#define HIF_CPU_INTR            0x00000001
unsigned long        			IntrW1Status;
#define PCI_INTA_2_CPU_INTR     0x40000000
#define PCI_INTA_1_CPU_INTR     0x20000000
#define PCI_INTA_0_CPU_INTR     0x10000000
#define EXT_IRQ_6_CPU_INTR      0x08000000
#define EXT_IRQ_5_CPU_INTR      0x04000000
#define EXT_IRQ_4_CPU_INTR      0x02000000
#define EXT_IRQ_3_CPU_INTR      0x01000000
#define EXT_IRQ_2_CPU_INTR      0x00800000
#define EXT_IRQ_1_CPU_INTR      0x00400000
#define EXT_IRQ_0_CPU_INTR      0x00200000
#define SPARE_3_CPU_INTR        0x00020000
#define SPARE_2_CPU_INTR        0x00010000
#define SPARE_1_CPU_INTR        0x00008000
#define SPARE_0_CPU_INTR        0x00004000
#define DVP_HR_CPU_INTR         0x00002000
#define DVP_DL_CPU_INTR         0x00001000
#define RPTD_CPU_INTR           0x00000400
#define AIO_CPU_INTR            0x00000200
#define GFX_CPU_INTR            0x00000100
#define GFX_MEMC_CPU_INTR       0x00000080
#define HDMI_RX_1_CPU_INTR      0x00000040
#define HDMI_RX_0_CPU_INTR      0x00000020
#define BVNM_CPU_INTR           0x00000010
#define MCIF_CPU_INTR           0x00000008
  unsigned long        IntrW0MaskStatus;
  unsigned long        IntrW1MaskStatus;
  unsigned long        IntrW0MaskSet;
  unsigned long        IntrW1MaskSet;
  unsigned long        IntrW0MaskClear;
  unsigned long        IntrW1MaskClear;
} Int1Control;                                


/***************************************************************************
 *  Level 1 Interrupt Function Prototypes
 **************************************************************************/
extern void CPUINT1_SetInt1Control(Int1Control *int1c);
extern unsigned long CPUINT1_GetInt1ControlAddr(void);
extern void CPUINT1_Isr(void);
extern void CPUINT1_Disable(unsigned long intId);
extern void CPUINT1_Enable(unsigned long intId);
extern int  CPUINT1_ConnectIsr(unsigned long intId, FN_L1_ISR pfunc,
                               void *param1, int param2);

#endif /* __ASSEMBLY__ */

#ifdef  __cplusplus
}
#endif

#endif /* _INT1_API_H */

