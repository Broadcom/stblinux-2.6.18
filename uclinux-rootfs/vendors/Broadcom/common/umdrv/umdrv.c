/***************************************************************************
 *     Copyright (c) 2002, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: umdrv.c $
 * $brcm_Revision: Irvine_BSEAVSW_Devel/42 $
 * $brcm_Date: 6/21/04 5:11p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /SetTop/linux/driver/usermodedrv/umdrv.c $
 * 
 * Irvine_BSEAVSW_Devel/42   6/21/04 5:11p erickson
 * PR11611: updated numbering for 7038 B0
 * 
 * Irvine_BSEAVSW_Devel/41   6/21/04 8:49a erickson
 * PR11599: added IS_7038 check for free_irq
 * 
 * Irvine_BSEAVSW_Devel/40   4/19/04 2:41p aram
 * PR10608: support for 7314 was added
 * 
 * Irvine_BSEAVSW_Devel/39   2/25/04 3:18p aram
 * PR9857: added support for 7319 and fix a comment changed back the name
 * of the driver to its original name.
 * 
 * Irvine_BSEAVSW_Devel/38   2/4/04 11:01a marcusk
 * PR9610: Enabled HIF_CPU_INTR
 * 
 * Irvine_BSEAVSW_Devel/37   1/30/04 5:38p vsilyaev
 * PR 9465: Added handling for 7038 interrupt used for extension slots.
 * 
 * Irvine_BSEAVSW_Devel/36   1/22/04 9:21a aram
 * PR 9438: support for 7111 was added to this driver
 * 
 * Irvine_BSEAVSW_Devel/35   12/29/03 2:55p aram
 * PR9134: support for 7317 was added for now it is a copy of 7315
 * 
 * Irvine_BSEAVSW_Devel/34   12/29/03 2:49p marcusk
 * PR9117: Support smart card interrupt.
 * 
 * Irvine_BSEAVSW_Devel/33   12/15/03 10:55a erickson
 * PR8980: fixed error message
 * 
 * Irvine_BSEAVSW_Devel/32   12/4/03 12:18p vsilyaev
 * PR. 8850. Enable UPG_CPU_INTR - needed for KIR (infra red remote)
 * 
 * Irvine_BSEAVSW_Devel/31   12/3/03 6:52p vsilyaev
 * Fixed handling of W1 interrupts in 7038. Linux kernel doesn't uses flat
 * translation between interrupt number and bit number, so e.g. PCI_INT_0
 * has it's a bit 32 (W1 bit 0) but linux number is 27).
 * 
 * Enabled PCI_INT_1 (3250 IRQ), Disabled ENET IRQ.
 * 
 * Don't use special SmartCard virtual interrupts for 7038 (they confilct
 * with a real ones).
 * 
 * Irvine_BSEAVSW_Devel/30   11/25/03 4:57p marcusk
 * PR8740: Unified structure and collapsed driver to work on all
 * platforms.  Currently tested on 97038 and 97115 platforms.
 * 
 * Irvine_BSEAVSW_Devel/29   11/24/03 12:34p vsilyaev
 * Added I2C and SPI support for 7038.
 * 
 * Irvine_BSEAVSW_Devel/28   11/24/03 8:51a aram
 * time measurement
 * 
 * Irvine_BSEAVSW_Devel/27   11/11/03 1:35p aram
 * PR 8512
 * 
 * Irvine_BSEAVSW_Devel/26   10/27/03 11:49a aram
 * added make rel for hydra
 * 
 * Irvine_BSEAVSW_Devel/25   10/27/03 9:16a aram
 * Some of BVN interrupts were enabled
 * 
 * Irvine_BSEAVSW_Devel/24   10/24/03 4:40p aram
 * fixed compile problem
 * 
 * Irvine_BSEAVSW_Devel/23   10/22/03 1:40p dlwin
 * Added hooks for W1 interrupts for 7038, has not been tested.
 * 
 * Irvine_BSEAVSW_Devel/22   10/22/03 9:14a aram
 * full support for 7038 was added
 * 
 * Irvine_BSEAVSW_Devel/21   9/10/03 4:23p aram
 * initial support for 7038 was added
 * 
 * Irvine_BSEAVSW_Devel/20   5/28/03 10:33a marcusk
 * Added basic support for 7112.
 * 
 * Irvine_BSEAVSW_Devel/19   5/16/03 10:46a aram
 * Added support for 7328
 * 
 * Irvine_BSEAVSW_Devel/18   4/17/03 9:48a btan
 * To support Beast smart card service, we requires Linux kernel to create
 * software IRQs for smart card interfaces (to not depend on UPG IRQ), we
 * need to change the IRQ handling and IRS.
 * 
 * Irvine_HDDemo_Devel/17   10/1/02 4:9p aram
 * Fixed the 7320 sIntName structure
 * 
 * Irvine_HDDemo_Devel/16   9/13/02 9:21a aram
 * BRCM_IOCTL_DEBUG_LEVEL was added
 * 
 * Irvine_HDDemo_Devel/15   9/3/02 4:56p marcusk
 * Do not allow the user mode driver to manage the UPG interrupt. PR 4179.
 * 
 * Irvine_HDDemo_Devel/14   8/28/02 1:6p aram
 * put back the fix for rev 10
 * 
 * Irvine_HDDemo_Devel/13   8/28/02 12:3p aram
 * added /proc/umdrv
 * 
 * Irvine_HDDemo_Devel/12   8/19/02 3:25p marcusk
 * Made driver more data driven (use tables to define various interrupt
 * and register parameters)
 * 
 * Irvine_HDDemo_Devel/11   8/14/02 12:32p aram
 * Added the SDS interrupt for 7315
 * 
 * Irvine_HDDemo_Devel/10   8/14/02 11:3a aram
 * fixed a warning message in the driver
 * 
 * Irvine_HDDemo_Devel/9   8/13/02 5:48p marcusk
 * Use copy_to_user and copy_from_user to pass data.
 * Did some minor cleanup (removed unused functions, structures, and
 * includes).
 * Added standard copyright notice.
 * Display driver version based upon clearcase version information.
 * 
 ***************************************************************************/
#include "linux/module.h"
#include "linux/pci.h"
#include "linux/utsname.h"
#include <linux/proc_fs.h>
#include "asm/uaccess.h"
#include "umdrv.h"

#define BCM_DRIVER_REVISION	strrchr("$brcm_Revision: Irvine_BSEAVSW_Devel/42 $",':')+2

/*
 * Define Major device number 
 */
#define BRCM_MAJOR 30

int bcmdebuglevel=0;
int bcmdebug=0;


MODULE_PARM(bcmdebug,"0-1i");

#define MODULE_NAME "umdrv"


#define INFINITE 0xFFFFFFFF

/*
 * Typedefs 
 */
typedef unsigned char ui8;
typedef unsigned long ui32;
typedef unsigned short ui16;

typedef volatile unsigned char io8;
typedef volatile unsigned long io32;
typedef volatile unsigned short io16;

typedef struct Int32Control {
  unsigned long        RevID;          	/* (00) */
  unsigned long        testControl;    	/* (04) */
  unsigned long        unused0;        	/* (08) */
  unsigned long        IrqMask;        	/* (0c) */
  unsigned long        IrqStatus;      	/* (10) */
  unsigned char        extIrqConfig; 	/* (17) */
  unsigned char        extIrqStatus; 	/* (16) */
  unsigned char        extIrqClr; 		/* (15) */
  unsigned char        extIrqMask; 		/* (14) */
} Int32Control; 

typedef struct Int64Control {
  unsigned long        IntrW0Status;
  unsigned long        IntrW1Status;
  unsigned long        IntrW0MaskStatus;
  unsigned long        IntrW1MaskStatus;
  unsigned long        IntrW0MaskSet;
  unsigned long        IntrW1MaskSet;
  unsigned long        IntrW0MaskClear;
  unsigned long        IntrW1MaskClear;
} Int64Control;

/* Macros used to implement Linux events. These are nearly identical */
/* to those in sched.h, but we've tweaked them to support a timeout. */
#define __bcmKNIWaitEventInterruptibleTimeout(wq, condition1, condition2, ret, ticks)	\
do {                                                            			\
    wait_queue_t __wait;                                        			\
    init_waitqueue_entry(&__wait, current);                     			\
    add_wait_queue(&wq, &__wait);                               			\
																			\
	/* Be sure to go half asleep before checking condition.	*/				\
	/* Otherwise we have a race condition between when we   */				\
	/* check the condition and when we call schedule().     */				\
	set_current_state(TASK_INTERRUPTIBLE);									\
																			\
	if (!condition1 && !condition2)											\
	{																		\
		if(schedule_timeout(ticks) == 0)									\
			ret = -EIO;														\
        if (signal_pending(current))                           				\
        	ret = -EIO; 	                                    			\
	}																		\
    current->state = TASK_RUNNING;                              			\
    remove_wait_queue(&wq, &__wait);                            			\
} while (0)

#define bcmKNIWaitEventInterruptibleTimeout(wq, condition1, condition2, timeout) 	\
({                                                              			\
    int __ret = 0;                                              			\
	unsigned long ticks;													\
	if(timeout != 0 && timeout != INFINITE)									\
		ticks = (timeout * HZ) / 1000;										\
	else																	\
		ticks = MAX_SCHEDULE_TIMEOUT;										\
    if (!(condition1) && !(condition2))                            			\
        __bcmKNIWaitEventInterruptibleTimeout(wq, condition1, condition2, __ret, ticks);	\
    __ret;                                                      			\
})

/* 
  Since smart card is part of the UPG and to support smart card beast 
  service, we has to create high status and mask here. The high mask 
  and high status is only used by software IRQ that created by Linux 
  kernel. When we report this high status back to application, we have
  to map it to one of the low status. For example, when we receive virtual
  IRQ 40 and 41 for SCA and SCB, we need to map it to UPG IRQ 10.  
  This design is also based on the assumption that we will not receive IRQ
  10 in this user mode driver. We will receive IRQ 40 and 41 instead and
  then map it to 10.
*/
typedef struct
{
	unsigned long   reportedStatus_l;	/* Interrupts that have been reported by driver to calling code */
	unsigned long   reportedStatus_h;	/* software Interrupts that created by Linux kernel and it should map into reportedStatus_l */
	unsigned long   globalmask_l;		/* HW interrupts that the driver controls */
	unsigned long   globalmask_h;		/* HW interrupts that the driver controls */
	union								/* Pointer to the hardware interrupt registers */
	{
		volatile Int32Control *intc32;	/* 32 bit interrupt controller */
		volatile Int64Control *intc64;	/* 64 bit interrupt controller */
	};
}intmgr_t;

static intmgr_t g_intmgr;
static wait_queue_head_t g_waitqueue;
int  brcm_stb_init(void);

#ifdef MODULE
int     init_module(void);
void    cleanup_module(void);
#else
static int 
brcm_start(void)
{
	return(brcm_stb_init(void));
}
__initcall(brcm_start);
#endif

static int      brcm_open(struct inode *inode, struct file *file);
static int      brcm_close(struct inode * inode, struct file * file);
static int      brcm_ioctl(struct inode *inode, struct file * file,
						   unsigned int cmd, unsigned long arg);
static void     brcm_interrupt(int irq, void *dev_id, struct pt_regs *regs);       
static void     intmgr_Init(void);
static void     brcm_enable_irq(intmgr_t *p_intmgr);

#if LINUX_VERSION_CODE <= KERNEL_VERSION(2,3,1)
static struct file_operations   brcm_fops = {
	NULL,		/* llseek */
	NULL,		/* read */
	NULL,		/* write */
	NULL,		/* readdir */
	NULL,		/* poll */
	brcm_ioctl,	/* ioctl */
	NULL,		/* mmap */
	brcm_open,	/* open */
	NULL,		/* flush */
	brcm_close,	/* release */
	NULL,		/* fsync */
	NULL,		/* fasync */
	NULL,		/* check_media_change */
	NULL,		/* revalidate */
	NULL		/* lock */
};
#else
static struct file_operations brcm_fops = {
	owner:      THIS_MODULE,
	ioctl:      brcm_ioctl,
	open:       brcm_open,
	release:    brcm_close
};
#endif

#define INT_REG_WRAP 0x80
#define INT_ENABLE_MASK 0x01

struct sIntName
{
	const char *name;	/* Text name of interrupt */
	int manageInt;		/* 0 - do not mananage this interrupt, 1 - manage this interrupt */
	unsigned long numInter; /* number of interrupts */
};

enum eIntControlType
{
	eIntControlType_32,
	eIntControlType_64
};

struct sChipConfig
{
	const char *chipName;
	struct sIntName *pIntName;
	unsigned long intcAddr;
	unsigned long bgeCpuCtrlAddr;
	unsigned long bgeCpuCtrlValue;
	unsigned int  maxNumIrq;
	enum eIntControlType intcType;
};

#define UPG_IRQ_NUM    10
#define SCA_IRQ_NUM    40 /* Software IRQ created by Linux kernel */
#define SCB_IRQ_NUM    41 /* Software IRQ created by Linux kernel */
#define UPG_IRQ   (1 << (10 - 1))
#define SCA_IRQ   (1 << (40 - 33))
#define SCB_IRQ   (1 << (41 - 33))

struct sIntName interruptName7115[] = {
	{NULL, 0}, /* Linux IRQ #'s are offset by 1 */
	{"DMA_IRQ",   0},
	{"EBI_RX",    0},
	{"EBI_TX",    0},
	{"IDE",       0},
	{"DOCSIS",    0},
	{"DAVIC",     0},
	{"USB_DEV",   0},
	{"USB_HOST",  0},
	{"MODEM",     0},
	{"UPG",       1},  /* 10 */
	{"SPI",       1},
	{"I2C",       1},
	{"DS1",       1},
	{"DS2",       1},
	{"OB1",       1},
	{"OB2",       1},
	{"BGE",       1},
	{"MINITITAN", 1},
	{"XPT_STATUS",1},
	{"XPT_OVFLOW",1}, /* 20 */
	{"XPT_MSG",   1},
	{"XPT_ICAM",  1},
	{"ATP",       1},
	{"ADP",       1},
	{"BTSC",      1},
	{"ENET",      0},
	{"VEC",       1},
	{"PCM",       1},
	{"VDEC0",     1},
	{"VDEC1",     1}, /* 30 */
	{"US",        1},
	{"TIMER",     0}, /* 32 */
	{NULL,        0|INT_REG_WRAP},
	{NULL,        0},
	{NULL,        0},
	{NULL,        0},
	{NULL,        0},
	{NULL,        0},
	{NULL,        0},
	{"SCA",       1}, /* Virtual IRQ 40 created by Linux kernel */
	{"SCB",       1}  /* Virtual IRQ 41 created by Linux kernel */
};

struct sIntName interruptName7315[] = {
	{NULL, 0}, /* Linux IRQ #'s are offset by 1 */
	{"DMA_IRQ",   0},
	{"EBI_RX",    0},
	{"EBI_TX",    0},
	{"IDE0",      0},
	{"IDE1",      0},
	{"ADP_C",     1},
	{"ATP_C",     1},
	{"USB_HOST",  0},
	{"MODEM",     0},
	{"UPG",       1},
	{"SPI",       1},
	{"I2C",       1},
	{NULL,        0},
	{NULL,        0},
	{"MIPS",      0},
	{"POSTED",    0},
	{"BGE",       1},
	{"MINITITAN", 1},
	{"XPT_STATUS",1},
	{"XPT_OVFLOW",1},
	{"XPT_MSG",   1},
	{"XPT_ICAM",  1},
	{"ATPA",      1},
	{"ADPA",      1},
	{"ATPAUD",    1},
	{NULL,        0},
	{"VEC",       1},
	{"UART",      0},
	{NULL,        0},
	{"SDS",       1},
	{NULL,        0},
	{"TIMER",     0}, /* 32 */
	{NULL,        0|INT_REG_WRAP},
	{NULL,        0},
	{NULL,        0},
	{NULL,        0},
	{NULL,        0},
	{NULL,        0},
	{NULL,        0},
	{"SCA",       1}, /* Virtual IRQ 40 created by Linux kernel */
	{"SCB",       1}  /* Virtual IRQ 41 created by Linux kernel */
};      


struct sIntName interruptName7317[] = {
	{NULL, 0}, /* Linux IRQ #'s are offset by 1 */
	{"DMA_IRQ",   0},
	{"EBI_RX",    0},
	{"EBI_TX",    0},
	{"IDE0",      0},
	{"IDE1",      0},
	{"ADP_C",     1},
	{"ATP_C",     1},
	{"USB_HOST",  0},
	{"MODEM",     0},
	{"UPG",       1},
	{"SPI",       1},
	{"I2C",       1},
	{NULL,        0},
	{NULL,        0},
	{"MIPS",      0},
	{"POSTED",    0},
	{"BGE",       1},
	{"MINITITAN", 1},
	{"XPT_STATUS",1},
	{"XPT_OVFLOW",1},
	{"XPT_MSG",   1},
	{"XPT_ICAM",  1},
	{"ATPA",      1},
	{"ADPA",      1},
	{"ATPAUD",    1},
	{NULL,        0},
	{"VEC",       1},
	{"UART",      0},
	{NULL,        0},
	{"SDS",       1},
	{NULL,        0},
	{"TIMER",     0}, /* 32 */
	{NULL,        0|INT_REG_WRAP},
	{NULL,        0},
	{NULL,        0},
	{NULL,        0},
	{NULL,        0},
	{NULL,        0},
	{NULL,        0},
	{"SCA",       1}, /* Virtual IRQ 40 created by Linux kernel */
	{"SCB",       1}  /* Virtual IRQ 41 created by Linux kernel */
};      


struct sIntName interruptName7110[] = {
	{NULL, 0}, /* Linux IRQ #'s are offset by 1 */
	{"DMA_IRQ",   0},
	{"EBI_RX",    0},
	{"EBI_TX",    0},
	{"IDE",       0},
	{"DOCSIS",    0},
	{"DAVIC",     0},
	{"USB_DEV",   0},
	{"USB_HOST",  0},
	{"MODEM",     0},
	{"UPG",       1},
	{"SPI",       1},
	{"I2C",       1},
	{"DS1",       1},
	{"DS2",       1},
	{"OB1",       1},
	{"OB2",       1},
	{"BGE",       1},
	{"MINITITAN", 1},
	{"XPT_STATUS",1},
	{"XPT_OVFLOW",1},
	{"XPT_MSG",   1},
	{"XPT_ICAM",  1},
	{"ATP",       1},
	{"ADP",       1},
	{"BTSC",      1},
	{"ENET",      0},
	{"VEC",       1},
	{"UART",      0},
	{"VDEC0",     1},
	{"VDEC1",     1},
	{"US",        1},
	{"TIMER",     0}, /* 32 */
	{NULL,        0|INT_REG_WRAP},
	{NULL,        0},
	{NULL,        0},
	{NULL,        0},
	{NULL,        0},
	{NULL,        0},
	{NULL,        0},
	{"SCA",       1}, /* Virtual IRQ 40 created by Linux kernel */
	{"SCB",       1}  /* Virtual IRQ 41 created by Linux kernel */
};

struct sIntName interruptName7320[] = {
	{NULL, 0}, /* Linux IRQ #'s are offset by 1 */
	{"DMA",             0},
	{"EBI_RX",          0},
	{"EBI_TX",          0},
	{"IDE0",            0},
	{"IDE1",            0},
	{"SDS_HOST",        0},
	{"USB_HOST2",       0},
	{"USB_HOST1",       0},
	{"MODEM",           0},
	{"UPG",             1},
	{"SPI",             1},
	{"I2C",             1},
	{"PERIPH_POST_DONE",0},
	{"EC_BRIDGE",       0},
	{"BGE_VEC0",        1},
	{"BGE_VEC1",        1},
	{"BGE",             1},
	{"MINITITAN",       1},
	{"XPT_STATUS",      1},
	{"XPT_OVFLOW",      1},
	{"XPT_MSG",         1},
	{"XPT_ICAM",        1},
	{"ATP_A",           1},
	{"ADP_A",           1},
	{"ATP_B",           1},
	{"ADP_B",           1},
	{"ATP_C",           1},
	{"UART",            0},
	{"ADP_C",           1},
	{"SDS_RX1",         1},
	{"SDS_RX2",         1},
	{"TIMER",           0}, /* 32 */
	{NULL,              0|INT_REG_WRAP},
	{NULL,              0},
	{NULL,              0},
	{NULL,              0},
	{NULL,              0},
	{NULL,              0},
	{NULL,              0},
	{"SCA",             1}, /* Virtual IRQ 40 created by Linux kernel */
	{"SCB",             1}  /* Virtual IRQ 41 created by Linux kernel */
};

struct sIntName interruptName7328[] = {
	{NULL, 0}, /* Linux IRQ #'s are offset by 1 */
	{"DMA",             0},
	{"EBI_RX",          0},
	{"EBI_TX",          0},
	{"IDE0",            0},
	{"IDE1",            0},
	{"SDS_HOST",        0},
	{"USB_HOST2",       0},
	{"USB_HOST1",       0},
	{"MODEM",           0},
	{"UPG",             1},
	{"SPI",             1},
	{"I2C",             1},
	{"PERIPH_POST_DONE",0},
	{"EC_BRIDGE",       0},
	{"BGE_VEC0",        1},
	{"BGE_VEC1",        1},
	{"BGE",             1},
	{"MINITITAN",       1},
	{"XPT_STATUS",      1},
	{"XPT_OVFLOW",      1},
	{"XPT_MSG",         1},
	{"XPT_ICAM",        1},
	{"ATP_A",           1},
	{"ADP_A",           1},
	{"ATP_B",           1},
	{"ADP_B",           1},
	{"ATP_C",           1},
	{"UART",            0},
	{"ADP_C",           1},
	{"SDS_RX1",         1},
	{"SDS_RX2",         1},
	{"TIMER",           0}, /* 32 */
	{NULL,              0|INT_REG_WRAP},
	{NULL,              0},
	{NULL,              0},
	{NULL,              0},
	{NULL,              0},
	{NULL,              0},
	{NULL,              0},
	{"SCA",             1}, /* Virtual IRQ 40 created by Linux kernel */
	{"SCB",             1}  /* Virtual IRQ 41 created by Linux kernel */
};


struct sIntName interruptName7038[] = {
	{NULL, 0}, /* Linux IRQ #'s are offset by 1 */
	{"HIF_CPU_INTR",             1},
	{"XPT_STATUS_CPU_INTR",      1},
	{"XPT_OVFL_CPU_INTR",        1},
	{"XPT_MSG_CPU_INTR",         1},
	{"XPT_ICAM_CPU_INTR",        1},
	{"BACH_CPU_INTR",			 1},
	{"IFD_CPU_INTR",			 1},
	{"GFX_CPU_INTR",			 1},
	{"HDMI_CPU_INTR",            1},
	{"VDEC_CPU_INTR",            1},
	{"VEC_CPU_INTR",             1},
	{"BVNB_CPU_INTR",            1},
	{"BVNF_CPU_INTR_0",			 1},
	{"BVNF_CPU_INTR_1",          1},
	{"BVNF_CPU_INTR_2",          1},
	{"ENET_CPU_INTR",            0},
	{"RFM_CPU_INTR",             1},
	{"UPG_TMR_CPU_INTR",         0},
	{"UPG_CPU_INTR",             1},
	{"UPG_BSC_CPU_INTR",         1},
	{"UPG_SPI_CPU_INTR",         1},
	{"UPG_UART0_CPU_INTR",       0},
	{"UPG_SC_CPU_INTR",          1},
	{"SUN_CPU_INTR",             0},
	{"MTP1_CPU_INTR",            1},
	{"MTP2_CPU_INTR",            1},
	{"USB_CPU_INTR",             0},
	/* Added for 7038 B0 */
	{"MC_CPU_INTR",              0},
	{"BVNF_CPU_INTR",            0},
	/* !!! INT_REG_WRAP is used to clear bit counter,
	* and start using next interrupt register */
	{"PCI_INTA_0_CPU_INTR",      0|INT_REG_WRAP},		/* 30 */
	{"PCI_INTA_1_CPU_INTR",      1},	/* 31 */
	{"PCI_INTA_2_CPU_INTR",      0},
	{NULL,                       0},	/* Reserved, 33 */
	{"EXT_IRQ_0_CPU_INTR",       0},
	{"EXT_IRQ_1_CPU_INTR",       0},
	{"EXT_IRQ_2_CPU_INTR",       1},
	{"EXT_IRQ_3_CPU_INTR",       0},
	{"EXT_IRQ_4_CPU_INTR",       1},
	{"PCI_SATA_PCI_INTR",        0}
};

struct sChipConfig chipConfigs[] = {
	{ "7110", interruptName7110, 0xfffe0600, 0xfffc8c10, 0x00000006, sizeof(interruptName7110)/sizeof(struct sIntName), eIntControlType_32 },
	{ "7111", interruptName7110, 0xfffe0600, 0xfffc8c10, 0x00000006, sizeof(interruptName7110)/sizeof(struct sIntName), eIntControlType_32 },
	{ "7112", interruptName7115, 0xfffe0600, 0xfffc8c10, 0x00000006, sizeof(interruptName7115)/sizeof(struct sIntName), eIntControlType_32 },
	{ "7115", interruptName7115, 0xfffe0600, 0xfffc8c10, 0x00000006, sizeof(interruptName7115)/sizeof(struct sIntName), eIntControlType_32 },
	{ "7315", interruptName7315, 0xfffe0600, 0xfffc8c10, 0x00000006, sizeof(interruptName7315)/sizeof(struct sIntName), eIntControlType_32 },
	{ "7314", interruptName7315, 0xfffe0600, 0xfffc8c10, 0x00000006, sizeof(interruptName7315)/sizeof(struct sIntName), eIntControlType_32 },
    { "7317", interruptName7317, 0xfffe0600, 0xfffc8c10, 0x00000006, sizeof(interruptName7317)/sizeof(struct sIntName), eIntControlType_32 },
	{ "7319", interruptName7320, 0xbafe0600, 0xbafc8c10, 0x00000006, sizeof(interruptName7320)/sizeof(struct sIntName), eIntControlType_32 },
	{ "7320", interruptName7320, 0xbafe0600, 0xbafc8c10, 0x00000006, sizeof(interruptName7320)/sizeof(struct sIntName), eIntControlType_32 },
	{ "7328", interruptName7328, 0xbafe0600, 0xbafc8c10, 0x00000006, sizeof(interruptName7328)/sizeof(struct sIntName), eIntControlType_32 },
	{ "7038", interruptName7038, 0xb0001400, 0xb0000200, 0x00000000, sizeof(interruptName7038)/sizeof(struct sIntName), eIntControlType_64 },
};

#define NUM_CHIP_CONFIGS	(sizeof( chipConfigs ) / sizeof( struct sChipConfig ))

static int gChipConfigIndex;


static struct proc_dir_entry *umdrv_dir, *version_file, *interrupt_file;

#define IS_7038() (chipConfigs[gChipConfigIndex].pIntName == interruptName7038) 

/****************************************************************
* void intmgr_Init(void)
****************************************************************/
void intmgr_Init(void)
{
	memset(&g_intmgr,0,sizeof(intmgr_t));
}

/****************************************************************
* brcm_enable_irq(unsigned long mask)
****************************************************************/
static void     brcm_enable_irq(intmgr_t *p_intmgr)
{
	int irq;
	unsigned long mask_h, mask_l;
	unsigned long *pmask;

	if(!IS_7038())
	{
		cli();
		if (p_intmgr->reportedStatus_h & SCA_IRQ)
		{
			enable_irq(SCA_IRQ_NUM);
			p_intmgr->reportedStatus_h &= ~ SCA_IRQ;
		}
		else if (p_intmgr->reportedStatus_h & SCB_IRQ)
		{
			enable_irq(SCB_IRQ_NUM);
			p_intmgr->reportedStatus_h &= ~ SCB_IRQ;
		}
		mask_h = p_intmgr->reportedStatus_h;
		p_intmgr->reportedStatus_h = 0x00;  /* reset all virtual status */
		mask_l = p_intmgr->reportedStatus_l;
		sti();
	} else {
		mask_h = p_intmgr->reportedStatus_h;
		mask_l = p_intmgr->reportedStatus_l;
	}

	for(pmask=&mask_l, irq = 1; irq< chipConfigs[gChipConfigIndex].maxNumIrq; ++irq)
	{
		if( chipConfigs[gChipConfigIndex].pIntName[irq].manageInt & INT_REG_WRAP)
		{
			pmask = &mask_h;
		}

		if( *pmask & 0x1 )
		{
			if( IS_7038() || irq != UPG_IRQ_NUM ) {
				if(bcmdebug)
					printk("bcmdriver: Re-enabling %s irq (linux %d)\n", chipConfigs[gChipConfigIndex].pIntName[irq].name, irq );

				enable_irq(irq);
			}
		}

		*pmask >>= 1;
	}
}

/****************************************************************
* brcm_interrupt
*
* INPUTS:	irq, dev_id,regs - see linux kernel api
* OUTPUTS:	None
* RETURNS:	None
* FUNCTION: 7xxx interrupt handler
* 
****************************************************************/
static void brcm_interrupt(int irq, void *dev_id, struct pt_regs *regs)
{
	if (!IS_7038()) {
		if (irq == UPG_IRQ_NUM) {
			printk("Fatal error! Should not get irq %d\n", irq);
		}
		else if (irq == SCA_IRQ_NUM) {
			g_intmgr.reportedStatus_h |= SCA_IRQ;
		}
		else if (irq == SCB_IRQ_NUM) {
			g_intmgr.reportedStatus_h |= SCB_IRQ;
		}
	}

	if(bcmdebug)
		printk("bcmdriver: Got %s irq (linux %d) %d\n", chipConfigs[gChipConfigIndex].pIntName[irq].name, irq, 
		               chipConfigs[gChipConfigIndex].pIntName[irq].numInter );

	disable_irq(irq);

	chipConfigs[gChipConfigIndex].pIntName[irq].numInter++;

	wake_up_interruptible(&g_waitqueue);
}

/****************************************************************
 * brcm_get_dev_info - Extracts information from the minor device number
****************************************************************/
void brcm_get_dev_info(struct inode *inode, unsigned int *devnum,
					   unsigned int *devmode)
{
	*devnum = MINOR(inode->i_rdev);
	*devmode = 0;
}

/****************************************************************
 * cleanup_module - Called when driver is removed. This will
 *                  only happen when useage count is 0.
****************************************************************/
#ifdef MODULE
void cleanup_module(void)
{
	int i;

	printk("bcmdriver: Cleanup_modules...\n");


	for( i = 1; i < chipConfigs[gChipConfigIndex].maxNumIrq; i++ )
	{
		if( chipConfigs[gChipConfigIndex].pIntName[i].manageInt & INT_ENABLE_MASK )
		{
			if(bcmdebug)
				printk("bcmdriver: Freeing %s irq (linux %d)\n", chipConfigs[gChipConfigIndex].pIntName[i].name, i );
			if (IS_7038() || i != UPG_IRQ_NUM)
				free_irq(i, NULL);
		}

	}
	unregister_chrdev(BRCM_MAJOR, "brcm");

	remove_proc_entry("interrupts", umdrv_dir);
	remove_proc_entry("version", umdrv_dir);
	remove_proc_entry(MODULE_NAME, NULL);


	printk("bcmdriver: Done...\n");
}
#endif


static int proc_read_interrupts(char *buf, char **start, off_t offset,
								int count, int *eof, void *data)
{
	int irq, mask_l, mask_h, len = 0;
	int limit = count - 80; /* Don't print more than this */
	
	mask_l =  g_intmgr.globalmask_l;
	mask_h =  g_intmgr.globalmask_h;
	
	MOD_INC_USE_COUNT;
	
	for (irq=1; 
		irq< chipConfigs[gChipConfigIndex].maxNumIrq && len <= limit; 
		irq++) 
	{
		if( ((irq <= 32) ? mask_l : mask_h) & 0x1 )
		{
			
			len += sprintf(buf+len,"%s %ld\n", chipConfigs[gChipConfigIndex].pIntName[irq].name, 
				chipConfigs[gChipConfigIndex].pIntName[irq].numInter);
		}

		if (irq <= 32)
			mask_l >>= 1;
		else
			mask_h >>= 1;
	}
	
	*eof = 1;
	
	MOD_DEC_USE_COUNT;
	
	return len;
}


static int proc_write_interrupts(struct file *file,
								 const char *buffer,
								 unsigned long count, 
								 void *data)
{
	int irq;
	
	MOD_INC_USE_COUNT;
	
	for (irq=1; irq<= 32; irq++) 
	{
		
		chipConfigs[gChipConfigIndex].pIntName[irq].numInter=0;
	}

	MOD_DEC_USE_COUNT;

	return irq;
}

static int proc_read_version(char *page, char **start,
                             off_t off, int count,
                             int *eof, void *data)
{
        int len;

        MOD_INC_USE_COUNT;
        
        len = sprintf(page, "%s\n",
                      BCM_DRIVER_REVISION);

        MOD_DEC_USE_COUNT;

        return len;
}

/****************************************************************
* brcm_open(struct inode *inode, struct file *file)
****************************************************************/
static int brcm_open(struct inode *inode, struct file *file)
{
	int err = 0;

	unsigned int devnum, devmode;
	brcm_get_dev_info(inode, &devnum, &devmode);

	if(bcmdebug)
		printk("brcm_open devnum =%d devmode=%d\n",devnum,devmode);

	if ( devnum != 0 )
		return -EFAULT;

	MOD_INC_USE_COUNT;
	return err;
}

/****************************************************************
* brcm_close(struct inode *inode, struct file *file)
****************************************************************/
static int brcm_close(struct inode *inode, struct file *file)
{
	int err = 0;
	unsigned int devnum, devmode;

	brcm_get_dev_info(inode, &devnum, &devmode);
	wake_up_interruptible(&g_waitqueue);
	MOD_DEC_USE_COUNT;
	return err;
}

/*************************************************************************************************************
* brcm_ioctl(struct inode *inode, struct file * file,
*               unsigned int cmd, unsigned long arg)
*************************************************************************************************************/
static int brcm_ioctl(struct inode *inode, struct file * file,
					  unsigned int cmd, unsigned long arg)
{
	int result = 0;
	int sizeOfIntStruct;

	if( chipConfigs[gChipConfigIndex].intcType == eIntControlType_64 )
	{
		sizeOfIntStruct = sizeof( tbcm_linux_dd_interrupt );
	}
	else
	{
		/* Do not copy entire structure for legacy applications to keep binary compatibility between
		 * legacy applications and this new version of the driver
		 */
		sizeOfIntStruct = sizeof( tbcm_linux_dd_interrupt ) - 8;
	}
	
	switch(cmd)
	{
	case BRCM_IOCTL_WAIT_FOR_INTERRUPTS: 
		{
			tbcm_linux_dd_interrupt intStruct;
			int timeout;
			ui32 flags;

			if( copy_from_user( &intStruct, (void *)arg, sizeOfIntStruct ) )
			{
				result = -EFAULT;
				printk("bcmdriver: copy_from_user failed!\n");
				break;
			}

			if(bcmdebug)
				printk("bcmdriver: waitfor W0=0x%08lX, W1=0x%08lX with timeOut=%d\n", intStruct.interruptW0mask, ((chipConfigs[gChipConfigIndex].intcType==eIntControlType_64)?intStruct.interruptW1mask:0), intStruct.timeout);

			/* Check and make sure the app isn't wanting an interrupt that we don't control */
			if( (intStruct.interruptW0mask | g_intmgr.globalmask_l) != g_intmgr.globalmask_l )
			{
				printk("bcmdriver: Invalid waitfor interrupt mask unknown interrupts L: 0x%08lX\n", 
					intStruct.interruptW0mask & ~g_intmgr.globalmask_l);

				/* Mask off the invalid bits */
				intStruct.interruptW0mask &= g_intmgr.globalmask_l;
			}

			if( chipConfigs[gChipConfigIndex].intcType == eIntControlType_64 )
			{
				if( (intStruct.interruptW1mask | g_intmgr.globalmask_h) != g_intmgr.globalmask_h )
				{
					printk("bcmdriver: Invalid waitfor interrupt mask unknown interrupts H: 0x%08lX\n",
						intStruct.interruptW1mask & ~g_intmgr.globalmask_h);

					/* Mask off the invalid bits */
					intStruct.interruptW1mask &= g_intmgr.globalmask_h;
				}
			}

			/* Re-enable any interrupts that were previously reported and handled */
			brcm_enable_irq( &g_intmgr);

			if( chipConfigs[gChipConfigIndex].intcType == eIntControlType_64 )
			{
				result = bcmKNIWaitEventInterruptibleTimeout( 
					g_waitqueue,
					(intStruct.interruptW0mask & g_intmgr.intc64->IntrW0Status),
					(intStruct.interruptW1mask & g_intmgr.intc64->IntrW1Status), 
					intStruct.timeout
					);
				intStruct.interruptW0status = g_intmgr.intc64->IntrW0Status & intStruct.interruptW0mask;
				intStruct.interruptW1status = g_intmgr.intc64->IntrW1Status & intStruct.interruptW1mask;
			}
			else
			{
				result = bcmKNIWaitEventInterruptibleTimeout( 
					g_waitqueue,
					(intStruct.interruptW0mask & g_intmgr.intc32->IrqStatus),
					0, 
					intStruct.timeout
					);
				intStruct.interruptW0status = g_intmgr.intc32->IrqStatus & intStruct.interruptW0mask;
				intStruct.interruptW1status = 0;
			}

			/* Smartcard has its own interrupt under 7038, UPG_SC_CPU_INTR */
			g_intmgr.reportedStatus_l = intStruct.interruptW0status;
			g_intmgr.reportedStatus_h = intStruct.interruptW1status;

			if( copy_to_user( (void *)arg, &intStruct, sizeOfIntStruct ) )
			{
				result = -EFAULT;
				printk("bcmdriver: copy_to_user failed!\n");
				break;
			}

			if(bcmdebug)
				printk("bcmdriver: returning interrupt status W0=0x%08lX, W1=0x%08lX\n", intStruct.interruptW0status, intStruct.interruptW1status);
		}
		break;

	case BRCM_IOCTL_DEBUG_LEVEL: 
		{
			bcmdebuglevel = (int)arg;

			/* if(bcmdebug) */
			{
				printk("bcmdriver: debug level is %d\n",  bcmdebuglevel);
			}
		}
		break;

	default:
		result = -EFAULT;
		break;
	}

	return result;
}

/****************************************************************
* brcm_stb_init(void)
****************************************************************/
int brcm_stb_init(void)
{
	ui16 chipid;
	ui32 flag;
	int i;
	int rv = 0;
	unsigned long *pmask;
	unsigned bit;


	/* Leave interrupt disabled while we are handling  it */
	unsigned long flags = SA_INTERRUPT;

	printk("Initializing bcmdriver version $ %s\n", BCM_DRIVER_REVISION);

	intmgr_Init();

	for( gChipConfigIndex = 0; gChipConfigIndex < NUM_CHIP_CONFIGS; gChipConfigIndex++ )
	{
		if( strncmp(&system_utsname.machine[0],chipConfigs[gChipConfigIndex].chipName,4) == 0 )
		{
			break;
		}
	}

	if( gChipConfigIndex == NUM_CHIP_CONFIGS )
	{
		printk("Unknown Platform .......... %s \n",&system_utsname.machine[0]);
		goto error;
	}

	g_intmgr.intc32 = ((Int32Control *const)chipConfigs[gChipConfigIndex].intcAddr);
	if( chipConfigs[gChipConfigIndex].intcType == eIntControlType_32 )
	{
		printk("Chip revision DEV_ID=0x%08x\n", g_intmgr.intc32->RevID);
	}
	
	/* Set BGE interrupt settings */
	*(unsigned long *)(chipConfigs[gChipConfigIndex].bgeCpuCtrlAddr) |= chipConfigs[gChipConfigIndex].bgeCpuCtrlValue;

	/* Make sure to init this first, as we may get an interrupt as soon as we request it */
	init_waitqueue_head(&g_waitqueue);

	printk("chipConfigs[%d].maxNumIrq = %d\n", gChipConfigIndex, chipConfigs[gChipConfigIndex].maxNumIrq);

	/* Request interrupts that we manage */
	for( bit=0, pmask=&g_intmgr.globalmask_l, i = 1; i < chipConfigs[gChipConfigIndex].maxNumIrq; i++,bit++ )
	{
		if( chipConfigs[gChipConfigIndex].pIntName[i].manageInt & INT_REG_WRAP)
		{
			bit = 0;
			pmask = &g_intmgr.globalmask_h;
		}

		if( chipConfigs[gChipConfigIndex].pIntName[i].manageInt & INT_ENABLE_MASK )
		{
			if ( IS_7038() || i != UPG_IRQ_NUM) {
				if(bcmdebug)
					printk("bcmdriver: Requesting %s irq (linux %d)\n", chipConfigs[gChipConfigIndex].pIntName[i].name, i );

			   request_irq(i, brcm_interrupt, flags, chipConfigs[gChipConfigIndex].pIntName[i].name, NULL);
			}


			*pmask |= 1UL << bit;
		}
	}
	printk("Global Interrupt Mask Low: 0x%08X, High: 0x%08X\n", g_intmgr.globalmask_l, g_intmgr.globalmask_h);

	/* Register driver last in case of errors during initialization */
	if(register_chrdev(BRCM_MAJOR,"brcm", &brcm_fops))
	{
		printk("bcmdriver: unable to get major %d\n", BRCM_MAJOR);
		goto error;
	}

	/* create directory */
	umdrv_dir = proc_mkdir(MODULE_NAME, NULL);
	if(umdrv_dir == NULL) {
		rv = -ENOMEM;
		goto error;
	}
	umdrv_dir->owner = THIS_MODULE;

	version_file = create_proc_read_entry("version",
                                              0444, umdrv_dir,
                                              proc_read_version,
                                              NULL);
	if(version_file == NULL) {
		rv  = -ENOMEM;
		goto no_version;
	}
	version_file->owner = THIS_MODULE;


	interrupt_file = create_proc_entry("interrupts", 0644, umdrv_dir);
	if(interrupt_file == NULL) {
		rv = -ENOMEM;
		goto no_version;
	}
	interrupt_file->read_proc = proc_read_interrupts;
	interrupt_file->write_proc = proc_write_interrupts;
	interrupt_file->owner = THIS_MODULE;


	printk("Initialization complete...\n");

	return 0;

no_interrupt:
        remove_proc_entry("version", umdrv_dir);

no_version:
        remove_proc_entry(MODULE_NAME, NULL);

error:
	return rv;
}

/****************************************************************
 * init_module - Called when the user or kernel loads the
 *               module into memory.
 ****************************************************************/
#ifdef MODULE
int init_module(void)
{
	int ret=0;

	ret = brcm_stb_init();


	return ret;
}
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,4,18)
MODULE_LICENSE("Proprietary");
#endif


