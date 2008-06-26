/***************************************************************************
 *     Copyright (c) 2002-05 Broadcom Corporation
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
 * $brcm_Workfile: bcm71xx_ide.c $
 *
 * Module Description:
 *    Broadcom IDE Controller routines for Linux IDE driver.
 *
 * Revision History:
 *    03/20/02 jli Initial revision. Version 1.0
 *
 * $brcm_Log: /Linux/kernel/src/bcm97xxx_2418/drivers/ide/bcm71xx_ide.c $
 * 
 * Bumped version to 2.0 for 2.6.12 port 06/27/05 ttruong
 * 
 * SanJose_Linux_Devel/9   11/13/02 9:38a ttruong
 * Undo John Li checkin yet again
 * 
 * SanJose_Linux_Devel/5   10/11/02 4:26p jli
 * Udma4 for BCM7320-0002 or newer.
 * 
 * SanJose_Linux_Devel/4   9/25/02 5:15p jli
 * Auto config udma4/3. 80c cable detect. Cleanup warnings.
 * 
 * SanJose_Linux_Devel/3   9/18/02 6:53p ttruong
 * Fixed based address -jli
 * 
 * SanJose_Linux_Devel\4   6/20/02 2:14p jfisher
 * Add DP522 config
 * 
 * SanJose_Linux_Devel\3   6/18/02 4:53p jli
 * Bcm7110 can do udma4.
 * 
 * SanJose_Linux_Devel\2   5/14/02 9:17a eddieshi
 * updated for 7320
 ***************************************************************************/

#define VERSION     "2.0"
#define VER_STR     "v" VERSION " " __DATE__ " " __TIME__

#include <linux/config.h>
#include <linux/version.h>
#include <linux/module.h>
#include <linux/ioport.h>
#include <linux/ide.h>
#include <linux/pci.h>
#include <linux/types.h>

#include <asm/pci.h>
#include <asm/ptrace.h>
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,0)
#include <asm/hdreg.h>
// THT: Already automatically defined in config #define CONFIG_BLK_DEV_IDE_MODES
#include <../drivers/ide/ide_modes.h>
#else
#define ide_ioreg_t unsigned long
#include <linux/delay.h>
#endif
#include <asm/scatterlist.h>


#include <asm/brcmstb/common/brcmstb.h>
#if defined (CONFIG_MIPS_BCM7440A0)
#include <asm/brcmstb/brcm97440a0/bchp_ide.h>
#elif defined (CONFIG_MIPS_BCM7440B0)
#include <asm/brcmstb/brcm97440b0/bchp_ide.h>
#endif
#include "bcm71xx_ide.h"

#ifdef CONFIG_MIPS_BCM7440
#define mips_io_port_base (0)
#endif

#if LINUX_VERSION_CODE <= KERNEL_VERSION(2,4,18)
static int  bcm71xx_ide_dmaproc(ide_dma_action_t func, ide_drive_t * drive, int ide_channel);
#define read_c0_prid()	read_32bit_cp0_register(CP0_PRID)
#define read_c0_config() read_32bit_cp0_register(CP0_CONFIG)

#elif LINUX_VERSION_CODE < KERNEL_VERSION(2,6,0)
/* 2.4.25, 2.4.29 */
#define dmaproc(op, drive) op(drive)

#else
/* Nothing for 2.6.0 */
#endif

static void DumpRegs(uint32 * pBase);

static int  bcm71xx_ide_tune_chipset(ide_drive_t *drive, uint8 speed, int ide_channel);
//static void bcm71xx_ide_tuneproc(ide_drive_t *drive, uint8 pio, int ide_channel);



extern char * ide_xfer_verbose (uint8 xfer_rate);

//#define TRACE(x)			printk x
#define TRACE(x)

typedef struct {
	uint32  UNUSED_0;        /* base+0x0000 */
	uint32  IdeCmd;
		#define IO_ENABLE       0x0001
		#define BMIDE_ENABLE    0x0004
	uint32  IdeProgIf;
		#define NATIVE_MODE     0x0010
	uint32  UNUSED_1;
	uint32  IdePriCs0Addr;   /* base+0x0010 */
	uint32  IdePriCs1Addr;
	uint32  IdeSecCs0Addr;
	uint32  IdeSecCs1Addr;
	uint32  IdeBmideAddr;    /* base+0x0020 */
	uint32  UNUSED_3[7];
	uint32  PioTiming;       /* base+0x0040 */
	uint32  DmaTiming;
	uint32  PioControl;
	uint32  UNUSED_4[2];
	uint32  UdmaControl;     /* base+0x0054 */
	uint32  UdmaMode;
	uint32  UNUSED_5[2];
	uint32  IfControl;       /* base+0x0064 */
	uint32  MbistControl;
	uint32  UNUSED_6;
	uint32  IdePriDteA;      /* base+0x0070 */
	uint32  IdePriDteB;
	uint32  UNUSED_7[2];
	uint32  IntMask;         /* base+0x0080 */
	uint32  IntStatus;
	uint32  UNUSED_8[94];    /* base+0x0088..0x01fc */
	uint32  IdePriData;      /* base+0x0200 */
	uint32  IdePriCylLow;    /* base+0x0204 */
	uint32  UNUSED_9[14];    /* base+0x0208..0x023c */
	char    UNUSED_BYTE1;    /* base+0x0240 */
	char    UNUSED_BYTE2;    /* base+0x0241 */
	char    IdePriDevCStat;  /* base+0x0242 */
	char    UNUSED_BYTE3;    /* base+0x0243 */
	uint32  UNUSED_10[47];   /* base+0x0244..0x02fc */
	uint32  PriBmCmdStatus;  /* base+0x0300 */
	uint32  PriBmDescTableAddr;
	uint32  SecBmCmdStatus;
	uint32  SecBmDescTableAddr;

} IdeRegisters;

// IDE Controller registers
volatile IdeRegisters * IDEC;

volatile uint32 BcmChipId;

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0)

/* --------------------------------------------------------------------------
	Name: RequestRegion
Abstract: Make the region busy? 
		  Doesn't matter for our MemoryMapped I/O
 -------------------------------------------------------------------------- */
struct resource* 
RequestRegion(ide_ioreg_t from, unsigned int extent, const char *name)
{
	struct resource* res;
	TRACE(("RequestRegion [%08X - %08X] %s\n", (uint)from, (uint)(from + extent - 1), name));

   res = (struct resource*) kmalloc(sizeof(struct resource), GFP_KERNEL);

	if (!res)
		return NULL;
	res->name = name;
	res->start = from;
	res->end = from + extent - 1;
	res->flags = IORESOURCE_IO;

	//if (insert_resource(&ioport_resource, &res)) {
	//    printk("RequestRegion failed\n");
	//}
	return res;
	
}

void 
ReleaseRegion(ide_ioreg_t from, unsigned int extent)
{
	// Do nothing, there is a memory leak here if it is called, but
	// we limit the number of hwifs to those that are in use, so
	// this should never be called.
	printk("ReleaseRegion [%08X - %08X]\n", (uint)from, (uint)(from + extent - 1));
}

#else
/* 2.4.x */

/* --------------------------------------------------------------------------
	Name: CheckRegion
Abstract: The kernel check_region function that this call would typically 
		  vector to attempts to kmalloc memory. We don't need this since 
		  our memory-mapped I/O registers don't consume kernel memory 
 -------------------------------------------------------------------------- */
int 
CheckRegion(ide_ioreg_t from, unsigned int extent)
{
	TRACE(("CheckRegion [%08X - %08X]\n", (uint)from, (uint)(from + extent - 1)));

	// Should check the upper bound of the registers also...
	if ( from < (ide_ioreg_t)IDEC || extent > 0xFFFF) { 
		return -EBUSY;
	}
	return 0;
}

/* --------------------------------------------------------------------------
	Name: RequestRegion
Abstract: Make the region busy? 
		  Doesn't matter for our MemoryMapped I/O
 -------------------------------------------------------------------------- */
void 
RequestRegion(ide_ioreg_t from, unsigned int extent, const char *name)
{
	TRACE(("RequestRegion [%08X - %08X] %s\n", (uint)from, (uint)(from + extent - 1), name));
	return;
}

#if LINUX_VERSION_CODE <= KERNEL_VERSION(2,4,18)

/* --------------------------------------------------------------------------
	Name: RequestIrq
Abstract: 
 -------------------------------------------------------------------------- */
static int 
RequestIrq(unsigned int irq, void (*handler)(int, void *, struct pt_regs *),
			unsigned long flags, const char *device, void *dev_id)
{
//printk("-->%s\n", __FUNCTION__);
	TRACE(("RequestIrq: %d\n", irq)); 


	return request_irq(irq, handler, flags, device, dev_id);
	
}
#endif
#endif // Linux < 2.6.0

/* --------------------------------------------------------------------------
	Name: bcm7110_swdma_error
Abstract: 
 -------------------------------------------------------------------------- */
static int 
bcm7110_swdma_error(ide_drive_t *drive)
{
	printk("%s: single-word DMA not supported\n", drive->name);
	return 0;
}

/* --------------------------------------------------------------------------
	Name: bcm71xx_ide_selectproc
Abstract: 
 -------------------------------------------------------------------------- */
/*
static void
bcm71xx_ide_selectproc(ide_drive_t *drive)
{
	TRACE(("*** bcm71xx_ide_selectproc ***\n"));
}
*/

/* --------------------------------------------------------------------------
	Name: bcm71xx_Pri_tune_chipset
Abstract: 
 -------------------------------------------------------------------------- */
static int
bcm71xx_tune_chipset(ide_drive_t *drive, uint8 speed)
{
	ide_hwif_t *hwif	= HWIF(drive);
	int ide_channel = hwif->index;

	return ( bcm71xx_ide_tune_chipset(drive, speed, ide_channel) );
}

 

/* --------------------------------------------------------------------------
	Name: bcm71xx_ide_tune_chipset
Abstract: 
 -------------------------------------------------------------------------- */
static int
bcm71xx_ide_tune_chipset(ide_drive_t *drive, uint8 xferspeed, int ide_channel)
{
	/* unit is the drive select number, 0 or 1 */
	uint8 unit          = (drive->select.b.unit & 0x01);
	u32 regTimingPIO   = 0x0;
	u32 regTimingDMA   = 0x0;
	u32 regCtlPIO      = 0x0;
	u32 regCtlStatUDMA = 0x0;
	int ret            = 0;
	int err            = 0;
	u8 speed;

	/* 
		Actual time is computed as (xxXXX + 1 ) * (1/33MHz) 
		example cwPio = 3  ==>  (3 + 1) * 30ns = 120ns
	*/
	uint8 cwPIO, cwDMA;		  /* command width for PIO and DMA */
	uint8 modePIO;            /* PIO mode */
	uint8 rwPIO, rwDMA;		  /* recover width for PIO and DMA */
	uint8 enUDMA;             /* UDMA enable for primary channel */
	uint8 modeUDMA;           /* UDMA mode */

	/* read the registers responsible for timing in PIO and DMA modes */
	regTimingPIO   = IDEC->PioTiming; 
	regTimingDMA   = IDEC->DmaTiming; 
	regCtlPIO      = IDEC->PioControl;
	regCtlStatUDMA = IDEC->UdmaControl;

	/* Assume this function gets called for each drive, or unit, on the IDE channel. */
	/* retrieve current settings for command and command recovery timing for PIO */
	if (ide_channel % 2) {
		cwPIO = unit ? GET_FIELD(regTimingPIO, IDE, PIO_TIME, SS_PIO_CMD) :
					   GET_FIELD(regTimingPIO, IDE, PIO_TIME, SM_PIO_CMD); 

		rwPIO = unit ? GET_FIELD(regTimingPIO, IDE, PIO_TIME, SS_PIO_CMD_REC) : 
					   GET_FIELD(regTimingPIO, IDE, PIO_TIME, SM_PIO_CMD_REC);

		modePIO = unit ? GET_FIELD(regCtlPIO, IDE, PIO_CNTL_MODE, SS_PIO_ACC_MD) :
						 GET_FIELD(regCtlPIO, IDE, PIO_CNTL_MODE, SM_PIO_ACC_MD);

		/* retrieve current settings for command and command recovery timing for DMA */
		cwDMA = unit ? GET_FIELD(regTimingDMA, IDE, DMA_TIME, SS_DMA_CMD) :
					   GET_FIELD(regTimingDMA, IDE, DMA_TIME, SM_DMA_CMD);
		rwDMA = unit ? GET_FIELD(regTimingDMA, IDE, DMA_TIME, SS_DMA_CMD_REC) :
					   GET_FIELD(regTimingDMA, IDE, DMA_TIME, SM_DMA_CMD_REC);

		/* retrieve current settings for UDMA.  The mode defines timing for UDMA */
		enUDMA = unit ? GET_FIELD(regCtlStatUDMA, IDE, UDMA_CNTL_STAT, SS_UDMA) :
						GET_FIELD(regCtlStatUDMA, IDE, UDMA_CNTL_STAT, SM_UDMA);
		modeUDMA = unit ? GET_FIELD(regCtlStatUDMA, IDE, UDMA_CNTL_STAT, SS_UDMA_MODE) :
						  GET_FIELD(regCtlStatUDMA, IDE, UDMA_CNTL_STAT, SM_UDMA_MODE);
	}
	else {
		cwPIO = unit ? GET_FIELD(regTimingPIO, IDE, PIO_TIME, PS_PIO_CMD) :
					   GET_FIELD(regTimingPIO, IDE, PIO_TIME, PM_PIO_CMD); 

		rwPIO = unit ? GET_FIELD(regTimingPIO, IDE, PIO_TIME, PS_PIO_CMD_REC) : 
					   GET_FIELD(regTimingPIO, IDE, PIO_TIME, PM_PIO_CMD_REC);

		modePIO = unit ? GET_FIELD(regCtlPIO, IDE, PIO_CNTL_MODE, PS_PIO_ACC_MD) :
						 GET_FIELD(regCtlPIO, IDE, PIO_CNTL_MODE, PM_PIO_ACC_MD);

		/* retrieve current settings for command and command recovery timing for DMA */
		cwDMA = unit ? GET_FIELD(regTimingDMA, IDE, DMA_TIME, PS_DMA_CMD) :
					   GET_FIELD(regTimingDMA, IDE, DMA_TIME, PM_DMA_CMD);
		rwDMA = unit ? GET_FIELD(regTimingDMA, IDE, DMA_TIME, PS_DMA_CMD_REC) :
					   GET_FIELD(regTimingDMA, IDE, DMA_TIME, PM_DMA_CMD_REC);

		/* retrieve current settings for UDMA.  The mode defines timing for UDMA */
		enUDMA = unit ? GET_FIELD(regCtlStatUDMA, IDE, UDMA_CNTL_STAT, PS_UDMA) :
						GET_FIELD(regCtlStatUDMA, IDE, UDMA_CNTL_STAT, PM_UDMA);
		modeUDMA = unit ? GET_FIELD(regCtlStatUDMA, IDE, UDMA_CNTL_STAT, PS_UDMA_MODE) :
						  GET_FIELD(regCtlStatUDMA, IDE, UDMA_CNTL_STAT, PM_UDMA_MODE);

	}


	if (xferspeed == 255)	{ /* PIO auto-tuning */
		speed = XFER_PIO_0 + ide_get_best_pio_mode(drive, 255, 5, NULL);
	}
	else {
		speed = ide_rate_filter(2, xferspeed); // tht 2 == UDMA4
	}


	switch (speed) {
		case XFER_UDMA_4:
		case XFER_UDMA_3:
		case XFER_UDMA_2:
		case XFER_UDMA_1:
		case XFER_UDMA_0:

		enUDMA = IDE_UDMA_CNTL_STAT_SS_UDMA_ENABLE;  /* pri/sec master/slave macro are the same */
		modeUDMA = speed & 0xF;
		cwDMA = 0x2; rwDMA = 0x0;                    /* can do mdma2 too */
		cwPIO = 0x2; rwPIO = 0x0; modePIO = 0x4;     /* can do pio4 too */
		break;

		case XFER_MW_DMA_2:
			cwDMA = 0x2; rwDMA = 0x0;
			enUDMA = IDE_UDMA_CNTL_STAT_SS_UDMA_DISABLE; /* can't do udma */
			cwPIO = 0x2; rwPIO = 0x0; modePIO = 0x4;     /* can do pio4 too */
			break;   
		case XFER_MW_DMA_1:
			cwDMA = 0x2; rwDMA = 0x1;
			enUDMA = IDE_UDMA_CNTL_STAT_SS_UDMA_DISABLE; /* can't do udma */
			cwPIO = 0x2; rwPIO = 0x0; modePIO = 0x4;     /* can do pio4 too */
			break;   
		case XFER_MW_DMA_0:
			cwDMA = 0x7; rwDMA = 0x7;
			enUDMA = IDE_UDMA_CNTL_STAT_SS_UDMA_DISABLE; /* can't do udma */
			cwPIO = 0x2; rwPIO = 0x0; modePIO = 0x4;     /* can do pio4 too */
			break;   

		/* assumption is that we don't support Single-Word DMA */
		case XFER_SW_DMA_2:
		case XFER_SW_DMA_1:
		case XFER_SW_DMA_0:
			return bcm7110_swdma_error(drive);
			break;

		case XFER_PIO_4:
			cwPIO = 0x2; rwPIO = 0x0; modePIO = 0x4;
			enUDMA = IDE_UDMA_CNTL_STAT_SS_UDMA_DISABLE; /* can't do udma */
			break;
		case XFER_PIO_3:
			cwPIO = 0x2; rwPIO = 0x2; modePIO = 0x3;
			enUDMA = IDE_UDMA_CNTL_STAT_SS_UDMA_DISABLE; /* can't do udma */
			break;
		case XFER_PIO_2:
			cwPIO = 0x3; rwPIO = 0x4; modePIO = 0x2;
			enUDMA = IDE_UDMA_CNTL_STAT_SS_UDMA_DISABLE; /* can't do udma */
			break;
		case XFER_PIO_1:
			cwPIO = 0x4; rwPIO = 0x7; modePIO = 0x1;
			enUDMA = IDE_UDMA_CNTL_STAT_SS_UDMA_DISABLE; /* can't do udma */
			break;
		case XFER_PIO_0:
			cwPIO = 0x5; rwPIO = 0xD; modePIO = 0x0;
			enUDMA = IDE_UDMA_CNTL_STAT_SS_UDMA_DISABLE; /* can't do udma */
			break;

		default:
			ret = 1;
	}


	if (ret) {
		return ret;
	}

	/* modify settings for command and command recovery timing for PIO */
	if (ide_channel % 2) {
		unit ? SET_FIELD(regTimingPIO, IDE, PIO_TIME, SS_PIO_CMD, cwPIO) :
			   SET_FIELD(regTimingPIO, IDE, PIO_TIME, SM_PIO_CMD, cwPIO);

		unit ? SET_FIELD(regTimingPIO, IDE, PIO_TIME, SS_PIO_CMD_REC, rwPIO) :
			   SET_FIELD(regTimingPIO, IDE, PIO_TIME, SM_PIO_CMD_REC, rwPIO);

		unit ? SET_FIELD(regCtlPIO, IDE, PIO_CNTL_MODE, SS_PIO_ACC_MD, modePIO) :
			   SET_FIELD(regCtlPIO, IDE, PIO_CNTL_MODE, SM_PIO_ACC_MD, modePIO);

		/* modify settings for command and command recovery timing for DMA */
		unit ? SET_FIELD(regTimingDMA, IDE, DMA_TIME, SS_DMA_CMD, cwDMA) :
			   SET_FIELD(regTimingDMA, IDE, DMA_TIME, SM_DMA_CMD, cwDMA);
		unit ? SET_FIELD(regTimingDMA, IDE, DMA_TIME, SS_DMA_CMD_REC, rwDMA) :
			   SET_FIELD(regTimingDMA, IDE, DMA_TIME, SM_DMA_CMD_REC, rwDMA);

		/* modify settings for UDMA.  The mode defines timing for UDMA */
		unit ? SET_FIELD(regCtlStatUDMA, IDE, UDMA_CNTL_STAT, SS_UDMA, enUDMA) :
			   SET_FIELD(regCtlStatUDMA, IDE, UDMA_CNTL_STAT, SM_UDMA, enUDMA);
		unit ? SET_FIELD(regCtlStatUDMA, IDE, UDMA_CNTL_STAT, SS_UDMA_MODE, modeUDMA) :
			   SET_FIELD(regCtlStatUDMA, IDE, UDMA_CNTL_STAT, SM_UDMA_MODE, modeUDMA);
	}
	else {
		unit ? SET_FIELD(regTimingPIO, IDE, PIO_TIME, PS_PIO_CMD, cwPIO) :
			   SET_FIELD(regTimingPIO, IDE, PIO_TIME, PM_PIO_CMD, cwPIO);

		unit ? SET_FIELD(regTimingPIO, IDE, PIO_TIME, PS_PIO_CMD_REC, rwPIO) :
			   SET_FIELD(regTimingPIO, IDE, PIO_TIME, PM_PIO_CMD_REC, rwPIO);

		unit ? SET_FIELD(regCtlPIO, IDE, PIO_CNTL_MODE, PS_PIO_ACC_MD, modePIO) :
			   SET_FIELD(regCtlPIO, IDE, PIO_CNTL_MODE, PM_PIO_ACC_MD, modePIO);

		/* modify settings for command and command recovery timing for DMA */
		unit ? SET_FIELD(regTimingDMA, IDE, DMA_TIME, PS_DMA_CMD, cwDMA) :
			   SET_FIELD(regTimingDMA, IDE, DMA_TIME, PM_DMA_CMD, cwDMA);
		unit ? SET_FIELD(regTimingDMA, IDE, DMA_TIME, PS_DMA_CMD_REC, rwDMA) :
			   SET_FIELD(regTimingDMA, IDE, DMA_TIME, PM_DMA_CMD_REC, rwDMA);

		/* modify settings for UDMA.  The mode defines timing for UDMA */
		unit ? SET_FIELD(regCtlStatUDMA, IDE, UDMA_CNTL_STAT, PS_UDMA, enUDMA) :
			   SET_FIELD(regCtlStatUDMA, IDE, UDMA_CNTL_STAT, PM_UDMA, enUDMA);
		unit ? SET_FIELD(regCtlStatUDMA, IDE, UDMA_CNTL_STAT, PS_UDMA_MODE, modeUDMA) :
			   SET_FIELD(regCtlStatUDMA, IDE, UDMA_CNTL_STAT, PM_UDMA_MODE, modeUDMA);
	}


	/* write the registers we just modified above */
	IDEC->PioTiming   = regTimingPIO; 
	IDEC->DmaTiming   = regTimingDMA; 
	IDEC->PioControl  = regCtlPIO;
	IDEC->UdmaControl = regCtlStatUDMA;

	if (speed > XFER_PIO_4) {
		// Now that the chipset is configured, do the drive
		err = ide_config_drive_speed(drive, speed);
		drive->current_speed = speed;
	} 

	printk("%s: Configured for %s %s\n", 
			drive->name, ide_xfer_verbose(speed), err ? "[Error]" : "");

	//DumpRegs((uint32 *)IDEC);

	return err;
}

/* --------------------------------------------------------------------------
	Name: bcm71xx_tuneproc
Abstract: Tune the primary/secondary ide PIO mode
 -------------------------------------------------------------------------- */
static void
bcm71xx_tuneproc(ide_drive_t *drive, uint8 pio)
{
	ide_hwif_t *hwif	= HWIF(drive);
	int ide_channel = hwif->index;

	if(pio == 255)
		(void) bcm71xx_ide_tune_chipset(drive, 255, ide_channel);
	else
			(void) bcm71xx_ide_tune_chipset(drive, XFER_PIO_0 + pio, ide_channel);
}


#if 0
/* --------------------------------------------------------------------------
	Name: bcm71xx_ide_tuneproc
Abstract: Tune the PIO mode
 -------------------------------------------------------------------------- */
static void
bcm71xx_ide_tuneproc(ide_drive_t *drive, uint8 pio, int ide_channel)
{
	uint32 speed;
	ide_pio_data_t pioData;


printk("bcm71xx_ide_tuneproc: Before: ide%d, pio=%d\n", ide_channel, pio);
	if (pio == 255) {
		// Autotune PIO mode
		pio = ide_get_best_pio_mode(drive, 255, 4, &pioData);
	}
	printk("bcm71xx_ide_tuneproc: After: ide%d, pio=%d\n", ide_channel, pio);
	switch (pio) {
		case 4:  speed = XFER_PIO_4; break;
		case 3:  speed = XFER_PIO_3; break;
		case 2:  speed = XFER_PIO_2; break;
		case 1:  speed = XFER_PIO_1; break;
		default: speed = XFER_PIO_0; break;
	}
	bcm71xx_ide_tune_chipset(drive, speed, ide_channel);

	if (! drive->init_speed) {
		drive->init_speed = speed;
	}
}

#endif

/* --------------------------------------------------------------------------
	Name: GetFastestSpeed
Abstract: 
 -------------------------------------------------------------------------- */
static uint8 
GetFastestSpeed(ide_drive_t *drive)
{
	struct hd_driveid *id = drive->id;
		uint ultra = 0;
		uint mword = 0;
	uint8 speed = 0;
	uint8 ultra66 = 0;

	// Can only use UDMA if drive->media == ide_disk ???

	// Figure out which fields are valid
	if (id->field_valid & 4) {
		ultra = id->dma_ultra;                     // UltraDMA supported?
		ultra66 = (uint8)(HWIF(drive)->udma_four); // 80-pin cable attached?
		if (ultra66 == 0)
			printk("%s: No 80-pin cable attached\n", HWIF(drive)->name);
	}
	if (id->field_valid & 2) {
		mword = id->dma_mword;  // Multi-word DMA supported?
	}

	// Figure out the fastest DMA speed we can run at
	// Udma4 capable chips: 7110 rev 0 and newer
	//                      7115 rev 1 and newer
	//                      7315 rev 1 and newer
	//                      7320 rev 2 and newer
	if ((ultra & 0x0010) && (ultra66)) {
		if ( (BcmChipId == 0x71150000) ||
			 (BcmChipId == 0x73150000) ||
			 (BcmChipId == 0x73200000) ||
			 (BcmChipId == 0x73200001)
		   )
			speed = XFER_UDMA_3;
		else
			speed = XFER_UDMA_4;
	} else if ((ultra & 0x0008) && (ultra66)) {
		speed = XFER_UDMA_3;
	} else if (ultra & 0x0004) {
		speed = XFER_UDMA_2;
	} else if (ultra & 0x0002) {
		speed = XFER_UDMA_1;
	} else if (ultra & 0x0001) {
		speed = XFER_UDMA_0;
	} else if (mword & 0x0004) {
		speed = XFER_MW_DMA_2;
	} else if (mword & 0x0002) {
		speed = XFER_MW_DMA_1;
	} else if (mword & 0x0001) {
		speed = XFER_MW_DMA_0;
	}

	return speed;

}

/* --------------------------------------------------------------------------
	Name: ConfigureDma
Abstract: Configure the drive/chipset for the fastest DMA mode
 -------------------------------------------------------------------------- */
static int 
ConfigureDma(ide_drive_t *drive, int ide_channel)
{
	uint8 speed;
	struct hd_driveid *id = drive->id;
	ide_hwif_t *hwif = HWIF(drive);

	if (id && (id->capability & 1) && hwif->autodma) {
		/* Consult the list of known "bad" drives */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,0)
		if (hwif->dmaproc(ide_dma_bad_drive, drive)) {
			return hwif->dmaproc(ide_dma_off, drive);
		}
#else
		if (__ide_dma_bad_drive(drive)) {
			return hwif->ide_dma_off_quietly(drive);
		}
#endif

		speed = GetFastestSpeed(drive);
		bcm71xx_ide_tune_chipset(drive, speed, ide_channel);

		if (! drive->init_speed) {
			drive->init_speed = speed;
		}

		if (speed > 0) {
			drive->using_dma = 1;
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,0)
			return hwif->dmaproc(ide_dma_on, drive);
#else
		//if (ide_dma_enable(drive))
			return hwif->ide_dma_on(drive);
		// Else PIO
#endif
		} 
	}
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,0)
	return hwif->dmaproc(ide_dma_off_quietly, drive);
#else
	return hwif->ide_dma_off_quietly(drive);
#endif
}

#if LINUX_VERSION_CODE <= KERNEL_VERSION(2,4,18)

/* THT 2.4.18 codes */

/* --------------------------------------------------------------------------
	Name: bcm71xx_Pri_dmaproc
Abstract: Hook the standard dmaproc so we can do 71xx specific configuration
 -------------------------------------------------------------------------- */
static int 
bcm71xx_dmaproc(ide_dma_action_t func, ide_drive_t * drive)
{
	ide_hwif_t *hwif	= HWIF(drive);
	int ide_channel = hwif->index;

		return ( bcm71xx_ide_dmaproc(func, drive, ide_channel) );
}


/* --------------------------------------------------------------------------
	Name: bcm71xx_ide_dmaproc
Abstract: Hook the standard dmaproc so we can do 71xx specific configuration
 -------------------------------------------------------------------------- */
static int 
bcm71xx_ide_dmaproc(ide_dma_action_t func, ide_drive_t * drive, int ide_channel)
{
	//TRACE(("*** bcm71xx_ide_dmaproc [%d]***\n", (int)func));

	switch (func) {
		case ide_dma_check:
			return ConfigureDma(drive, ide_channel);
		default:
			break;
	}
	// Anything we didn't specifically handle gets passed back to the 
	// default proc
	return ide_dmaproc(func, drive);
}

#elif LINUX_VERSION_CODE < KERNEL_VERSION(2,6,0)
/* 2.4.25, 2.4.29 codes */
static int
bcm71xx_ide_dma_check_proc(ide_drive_t* drive)
{
	
	return ConfigureDma(drive, drive->hwif->index);
}


#else
/* 2.6.x */
static int bcm71xx_ide_dma_check(ide_drive_t *drive) 
{
	ide_hwif_t *hwif	= HWIF(drive);
	int ide_channel = hwif->index;

	return ConfigureDma(drive, ide_channel);
}

#endif

/* --------------------------------------------------------------------------
	Name: DumpRegs
Abstract: Dump the IDE registers
 -------------------------------------------------------------------------- */
/* */
static void 
DumpRegs(uint32* in_pData32)
{
	int i;
	uint8 * pBase = (uint8*) in_pData32;
	uint16* pData16;
	uint8* pData8;
	uint32* pData32 = in_pData32;

	for (i = 0; i < 9; i++) {
		printk("%08X => %08X\n", (uint)pData32, *pData32);
		pData32++;
	}
	pData32 = (uint32*) (pBase + (0x40));
	for (i = 0; i < 18; i++) {
		printk("%08X => %08X\n", (uint)pData32, *pData32);
		pData32++;
	}

#ifndef CONFIG_MIPS_BCM7440
	pData16 = (uint16*) (pBase + (0x200));
	for (i = 0; i < 4; i++) {
		printk("%08X => %04X\n", (uint)pData16, *pData16);
		pData16++;
	}
#else
	pData8 = (uint8*) (pBase + (0x200));
	for (i = 0; i < 8; i++) {
		printk("%08X => %02X\n", (uint)pData8, *pData8);
		pData8++;		
	}
#endif

	pData32 = (uint32*) (pBase + (0x300));
	for (i = 0; i < 4; i++) {
		printk("%08X => %08X\n", (uint)pData32, *pData32);
		pData32++;
	}

	pData32 = (uint32*) (pBase + (0x242));
	printk("%08x => %08X\n", pData32, *pData32);
#ifdef BCHP_IDE_EndianCtrl
	printk("Endian Ctrl: @%08x = %08x\n", BCM_PHYS_TO_K1(BCHP_PHYSICAL_OFFSET+BCHP_IDE_EndianCtrl),
		*(volatile unsigned long*) BCM_PHYS_TO_K1(BCHP_PHYSICAL_OFFSET+BCHP_IDE_EndianCtrl));
#endif
}

void
dump_ide(void)
{
	if (IDEC) {
		DumpRegs((uint32 *)IDEC);
	}
}

#if LINUX_VERSION_CODE <= KERNEL_VERSION(2,4,18)
/* Nothing */

#elif LINUX_VERSION_CODE < KERNEL_VERSION(2,6,0)
/* 2.4.25, 2.4.29 */
/* Do nothing, we already done these in bcm71xx_ide_init_hwif */

static int 
bcm71xx_ide_default_irq (ide_ioreg_t base)
{
printk("bcm71xx_ide_default_irq should never be called\n");
	return 0;
}

static ide_ioreg_t 
bcm71xx_ide_default_io_base (int index)
{
 printk("bcm71xx_ide_default_io_base should never be called\n"); 
	return 0;
}

static void 
bcm71xx_ide_init_hwif_ports (
	hw_regs_t *hw, 
	ide_ioreg_t data_port,
	ide_ioreg_t ctrl_port, 
	int *irq)
{
printk("bcm71xx_ide_init_hwif_ports should never be called\n");

}

/* Phony device to avoid crash */
static ide_pci_device_t bcm71xx_cds = {

};

#else
/* 2.6.x */
#if  defined( CONFIG_BLK_DEV_SVWKS)
  #if defined(__MIPSEB__) 
	#define bcm71xx_ioswabl(x) (x)  //le32_to_cpu(x)
  #else
	#define bcm71xx_ioswabl(x) (x) //(le32_to_cpu(x))
  #endif
#else
  #define bcm71xx_ioswabl(x) (x)  //le32_to_cpu(x)
#endif

static u32 bcm71xx_inl (unsigned long port)
{

	u32 val;
#if defined(CONFIG_MIPS_BCM7440)
	if ((port & 0xfffff000) != 0xb1700000) {
		printk( "%s: INVALID IDE PORT ADDRESS 0x%x (should be 0x%x)!!\n",
			__FUNCTION__, port, (0xb1700000 | (port & 0xfff)) );
		return 0xffffffff;
	}
#endif
	val = bcm71xx_ioswabl(*(volatile u32 *)(mips_io_port_base + port));
//printk("bcm71xx_inl(%08x)=%08x\n", mips_io_port_base+port, val);
	return val;
}

 static void bcm71xx_outl (u32 val, unsigned long port)
{
#if defined(CONFIG_MIPS_BCM7440)
	if ((port & 0xfffff000) != 0xb1700000) {
		printk( "%s: INVALID IDE PORT ADDRESS 0x%x (should be 0x%x)!!\n",
			__FUNCTION__, port, (0xb1700000 | (port & 0xfff)) );
	}
#endif
//printk("-->bcm71xx_outl(%08x, %08x\n", val, port);
	*(volatile u32 *)(mips_io_port_base + (port)) = bcm71xx_ioswabl(val);
//printk("<--bcm71xx_outl(%08x, %08x\n", bcm71xx_ioswabl(val), mips_io_port_base + (port));
}

void bcm71xx_insl (unsigned long port, void *addr, u32 count)
{
//printk("----------->bcm71xx_insl\n");
	while (count--) {
		*(u32 *)addr = bcm71xx_inl(port);
		addr += 4;
	}
}

void bcm71xx_outsl (unsigned long port, const void *addr, u32 count)
{
//printk("----------->bcm71xx_outsl\n");
	while (count--) {
		bcm71xx_outl(*(u32 *)addr, port);
		addr += 4;
	}
}

u16 bcm71xx_inw (unsigned long port)
{
u16 val;

	port = /*__swizzle_addr_w */ (port);
#if defined(CONFIG_MIPS_BCM7440)
	if ((port & 0xfffff000) != 0xb1700000) {
		printk( "%s: INVALID IDE PORT ADDRESS 0x%x (should be 0x%x)!!\n",
			__FUNCTION__, port, (0xb1700000 | (port & 0xfff)) );
		return 0xffff;
	}
#endif
	val = /*ioswabw*/(*(volatile u16 *)(mips_io_port_base + port));
//printk("bcm71xx_inw(%08x) = %04x\n", port, val);
	return val;
}

void bcm71xx_insw (unsigned long port, void *addr, u32 count)
{
	while (count--) {
		*(u16 *)addr = bcm71xx_inw(port);
		addr += 2;
	}
}

void bcm71xx_outw (u16 val, unsigned long port)
{
#if defined(CONFIG_MIPS_BCM7440)
	if ((port & 0xfffff000) != 0xb1700000) {
		printk( "%s: INVALID IDE PORT ADDRESS 0x%x (should be 0x%x)!!\n",
			__FUNCTION__, port, (0xb1700000 | (port & 0xfff)) );
	}
#endif
	do {
		*(volatile u16 *)(mips_io_port_base + /*__swizzle_addr_w */(port)) =
			/*ioswabw*/(val);
	} while(0);
}

void bcm71xx_outsw (unsigned long port, const void *addr, u32 count)
{
	while (count--) {
		bcm71xx_outw(*(u16 *)addr, port);
		addr += 2;
	}
}

static u8 bcm71xx_inb (unsigned long port)
{
	u8 val;

#if defined(CONFIG_MIPS_BCM7440)
	if ((port & 0xfffff000) != 0xb1700000) {
		printk( "%s: INVALID IDE PORT ADDRESS 0x%x (should be 0x%x)!!\n",
			__FUNCTION__, port, (0xb1700000 | (port & 0xfff)) );
		return 0xff;
	}
#endif
	val = *(volatile u8 *)(mips_io_port_base + port);

//printk("bcm71xx_inb(%08x) = %04x, mips_io_port_base 0x%x\n", port, val, mips_io_port_base);
	return val;
}

static void bcm71xx_outb (u8 val, unsigned long port)
{
#if defined(CONFIG_MIPS_BCM7440)
	if ((port & 0xfffff000) != 0xb1700000) {
		printk( "%s: INVALID IDE PORT ADDRESS 0x%x (should be 0x%x)!!\n",
			__FUNCTION__, port, (0xb1700000 | (port & 0xfff)) );
	}
#endif
	do {
		*(volatile u8 *)(mips_io_port_base + (port)) = (val);
	} while(0);
}

static void bcm71xx_outbsync (ide_drive_t *drive, u8 addr, unsigned long port)
{
	bcm71xx_outb(addr, port);
}

static void bcm71xx_hwif_iops (ide_hwif_t *hwif)
{
//printk("-->bcm71xx_hwif_iops\n");
	// Already called before bcm71xx_ide_probe() get called 
	// default_hwif_iops(hwif);

#ifdef CONFIG_MIPS_BCM7440
	hwif->OUTB	= bcm71xx_outb;
	/* Most systems will need to override OUTBSYNC, alas however
	   this one is controller specific! */
	hwif->OUTBSYNC	= bcm71xx_outbsync;
	hwif->OUTW	= bcm71xx_outw;
	hwif->OUTL	= bcm71xx_outl;
	hwif->OUTSW	= bcm71xx_outsw;
	hwif->OUTSL	= bcm71xx_outsl;
	hwif->INB	= bcm71xx_inb;
	hwif->INW	= bcm71xx_inw;
	hwif->INL	= bcm71xx_inl;
	hwif->INSW	= bcm71xx_insw;
	hwif->INSL	= bcm71xx_insl;
#else
	hwif->OUTL	= bcm71xx_outl;
	//hwif->OUTSL	= bcm71xx_outsl;
	hwif->INL	= bcm71xx_inl;
	//hwif->INSL	= bcm71xx_insl;
	//hwif->OUTW	= bcm71xx_outw;
	//hwif->OUTSW	= bcm71xx_outsw;
	//hwif->INW	= bcm71xx_inw;
	//hwif->INSW	= bcm71xx_insw;
#endif
//printk("<--bcm71xx_hwif_iops\n");
}
//#endif // #if defined(__MIPSEB__

/* Chipset definition */
static ide_pci_device_t bcm71xx_cds = {
		.name		= "Broadcom BCM7XXX IDE",
#if defined( CONFIG_MIPS_BCM7320 ) || defined( CONFIG_MIPS_BCM7317_IDE1 )
		.channels	= 2,	
#else
		.channels	= 1,	
#endif
		.autodma	= AUTODMA,
		//.enablebits	= {{0x00,0x00,0x00}, {0x00,0x00,0x00}},
		.bootable	= ON_BOARD,
		.extra		= 0,	
};


#endif /* Kernel 2.4.25 or later */	

/* --------------------------------------------------------------------------
	Name: bcm71xx_ide_init_hwif
Abstract: 
 -------------------------------------------------------------------------- */
static void
bcm71xx_ide_init_hwif(int ide_channel)
{
	int i;
	uint8 * pData;
	ide_hwif_t * hwif;
	hw_regs_t  * hw;
	int CS0_Offset;
	int CS1_Offset;
	ulong BmStatAddr;
	int os_irq;


	if  ((ide_channel % 2) == 0) {
		/* Primary channel */
			CS0_Offset = IDEC->IdePriCs0Addr;
			CS1_Offset = IDEC->IdePriCs1Addr;
			BmStatAddr = (ulong)&(IDEC->PriBmCmdStatus);
			os_irq     = BCM_LINUX_IDE0_IRQ;
			
		} else {
		/* Secondary channel */
			CS0_Offset = IDEC->IdeSecCs0Addr;
			CS1_Offset = IDEC->IdeSecCs1Addr;
			BmStatAddr = (ulong)&(IDEC->SecBmCmdStatus);
#ifdef CONFIG_MIPS_BCM7317
		   os_irq     = BCM_LINUX_IDE1_IRQ; /* jli- shared irq */
#else
			os_irq     = BCM_LINUX_IDE0_IRQ; /* jli- shared irq */
#endif // 7317

	}

	hwif = &ide_hwifs[ide_channel];
	hw   = &hwif->hw;

#if LINUX_VERSION_CODE > KERNEL_VERSION(2,6,0) // && defined(__MIPSEB__)
	bcm71xx_hwif_iops(hwif);
#endif

	hwif->tuneproc 	 = bcm71xx_tuneproc;
	hwif->speedproc  = bcm71xx_tune_chipset;
	//hwif->selectproc = bcm71xx_ide_selectproc;
	hwif->ide_dma_check = bcm71xx_ide_dma_check;

	// Let user turn off DMA from command-line switches
	if (noautodma) {
		// Autotune PIO modes
		hwif->drives[0].autotune = 1;
		hwif->drives[1].autotune = 1;
	} else {
		// Auto-enable DMA
		hwif->autodma = 1;
	}

	// We support UDMA 66 jli- not sure if 80c cable connected till probe_hwif
	hwif->udma_four = 0;
	// We're not a PCI device, but we need this field so the code in ide-dma.c
	// can be used. I could've modified ide-dma.c in a number of places, or 
	// include/linux/ide.h in one place.
	hwif->pci_dev   = NULL; 

#if LINUX_VERSION_CODE > KERNEL_VERSION(2,4,18)
	hwif->hold = 1; // tht prevent ide_register_hw from re-initializing IO ports

#if LINUX_VERSION_CODE > KERNEL_VERSION(2,6,0)
	hwif->chipset = ide_bcm7xxx;
#endif
	hwif->cds = &bcm71xx_cds;
#endif

	//hwif->noprobe   = 1;
	//hwif->present   = 1;
	//hwif->hwif_data   // private storage

	hwif->serialized = 1; /* jli- serialize primary/secondary I/O */

	pData = (uint8 *)IDEC;
	for (i = IDE_DATA_OFFSET; i <= IDE_STATUS_OFFSET; i++) {
		hw->io_ports[i] = (ide_ioreg_t)(pData + CS0_Offset + i);
	}
	hw->io_ports[IDE_CONTROL_OFFSET] = (ide_ioreg_t)(pData + CS1_Offset + 2);

	// The first 8 (0-7) IRQs are MIPS. Second level (peripheral) interrupts 
	// start at 8 and are mapped appropriately by the interrupt handler
	hw->irq = os_irq;

	i = ide_register_hw(hw, NULL);

	if (i == -1) {
		printk("Unable to register Broadcom IDE%01X interface\n", ide_channel);
		return;
	}

	printk("%s: Broadcom IDE Device @hwif=%p ", hwif->name, hwif);
	printk(VER_STR "\n");


	if (hwif->autodma) {
		// Setup DMA now that we've registered our device
		ide_setup_dma(hwif, BmStatAddr, 8);
		// Replace default dmaproc so we can configure our chipset for 
		// faster modes. 


#if LINUX_VERSION_CODE <= KERNEL_VERSION(2,4,18) 

			hwif->dmaproc = &bcm71xx_dmaproc;

#elif LINUX_VERSION_CODE < KERNEL_VERSION(2,6,0)
	// THT: 2.425 does not define dmaproc any more
	hwif->ide_dma_check = bcm71xx_ide_dma_check_proc;
#else
	/* 2.6.x, dma_check already done above */

#endif
	}

	return;
}

/* --------------------------------------------------------------------------
	Name: bcm71xx_ide_probe
Abstract: Probe for and initialize the IDE controller
 -------------------------------------------------------------------------- */
#include <asm/cpu.h>
static int
bcm71xx_ide_probe(void)
{
	unsigned long flags;
	int count=0;
	int id0, id1;
	uint32 uChipId, ProcId;
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,0)
	extern struct ide_ops * ide_ops;

#endif

//printk("-->%s\n", __FUNCTION__);
	ide_hwif_t * hwif;
	ide_hwif_t * mate;

#if defined( CONFIG_MIPS_BCM7440 ) && defined( CONFIG_BLK_DEV_IDE_SATA )
	hwif = &ide_hwifs[2];
	mate = &ide_hwifs[3];
#else
	hwif = &ide_hwifs[0];
	mate = &ide_hwifs[1];
#endif
	
	if (hwif->chipset != ide_unknown || mate->chipset != ide_unknown)
		return 1;
	
	local_irq_save(flags);
	
#ifdef CONFIG_MIPS_BCM7440
	{
		volatile unsigned long* pSundryRev = (volatile unsigned long*) 0xb0404000;

		uChipId = *pSundryRev;
	}
	   
#else
	// Grab the Chip ID    
	ProcId = read_c0_prid();
	if ((ProcId & 0xFF00) == PRID_IMP_5KC) {
		uChipId = *(uint32 *)0xBAFE0600;
	} else {
		uChipId = *(uint32 *)0xFFFE0600;
	}
#endif

	// Set the base address for the IDE registers
	switch (uChipId >> 16) {
		case 0x7110:
		case 0x7115:
		case 0x7314:
		case 0x7315:
		case 0x7317:
		case 0x7318:
		case 0x7320:
		case 0x7440:

			BcmChipId = uChipId;
			// Look for the first free HWIF id. It should always be 0
			// No longer true: THT! for 7440
			id0 = id1 = 0;
			while (id0 < MAX_HWIFS && ide_hwifs[id0].io_ports[IDE_DATA_OFFSET] != 0) {
				++id0;
			}

			if (id0 >= MAX_HWIFS) {
				return 2;
			}

	
			IDEC = (IdeRegisters *)IDE_BASE_7xxx;
		 
			if ((uChipId>>16) == 0x7315 || (uChipId>>16) == 0x7317 || (uChipId>>16) == 0x7320 ||
			(uChipId>>16) == 0x7314 || (uChipId>>16) == 0x7318 || (uChipId>> 16) == 0x7440
		) {

#ifdef CONFIG_DP522
				*(uint8 *)0xBAFE0057 &= 0xbf; /* jli-debug */
				*(uint8 *)0xBAFE0053 |= 0x40; /* jli-debug unreset ide rst */
#endif

#ifdef CONFIG_MIPS_BCM7317_IDE1
				/* 
				 * tht: Turn on 2nd IDE controller, as it is muxed 
				 * with the 656
				 */
				*(volatile uint8 *)0xFFFE8005 |= 0x80; /* GPIO_SEL2[7] = 1 */
				*(volatile uint8 *)0xFFFE8006 &= 0x7F; /* GPIO_SEL1[7] = 0 */
#endif

				// Look for the second free HWIF id. It should always be 1
				id1 = id0 + 1;
				while (id1 < MAX_HWIFS && ide_hwifs[id1].io_ports[IDE_DATA_OFFSET] != 0) {
					++id1;
				}
				if (id1 >= MAX_HWIFS) {
					id1 = 0;
				}
			}

			break;

		default:
			// Don't do anything if it's not a 71xx/73xx
			return 1;
	}

	// Override the functions in arch/mips/lib/ide-no.c that
	// the kernel calls. 
#if LINUX_VERSION_CODE <= KERNEL_VERSION(2,4,18)
	ide_ops->ide_check_region   = &CheckRegion;
	ide_ops->ide_request_region = &RequestRegion;
	ide_ops->ide_request_irq    = &RequestIrq;
#elif LINUX_VERSION_CODE < KERNEL_VERSION(2,6,0) /* 2.4.25, 2.4.29 */
	ide_ops->ide_default_irq = bcm71xx_ide_default_irq;
	ide_ops->ide_default_io_base = bcm71xx_ide_default_io_base;
	ide_ops->ide_init_hwif_ports = bcm71xx_ide_init_hwif_ports;
	
#else
	/* 2.6.x nothing */
#endif

	//ide_ops->ide_free_irq       = &FreeIrq;
	//ide_ops->ide_release_region = &ReleaseRegion;

	// Setup the IDE configuration
	IDEC->IdeCmd             = (IO_ENABLE | BMIDE_ENABLE);
	IDEC->IdeProgIf          = 0x8F00;

	IDEC->IdePriCs0Addr      = 0x200;
	IDEC->IdePriCs1Addr      = 0x240;
	IDEC->IdeSecCs0Addr      = 0x280;
	IDEC->IdeSecCs1Addr      = 0x2C0;
	IDEC->IdeBmideAddr       = 0x300;

	IDEC->PioTiming          = 0x99999999;
	IDEC->DmaTiming          = 0xFFFFFFFF;
	IDEC->PioControl         = 0xF0F0;
	IDEC->UdmaControl        = 0;  //0x0F;  // THT Was 0;
	IDEC->UdmaMode           = 0x040F;	   /* Burst Control */
	IDEC->IfControl          = 0x12;       /* 4burst & udma DA0..2 low */
	IDEC->MbistControl       = 0x22;
	IDEC->IntMask            = 0x22;
	IDEC->IntStatus          = 0x22;
#if defined(CONFIG_MIPS_BCM7320) || defined(CONFIG_MIPS_BCM7317_IDE1)
	IDEC->PriBmCmdStatus     = 0x640000; // tht for 2.6 from 40000H
	IDEC->PriBmDescTableAddr = 0;
	IDEC->SecBmCmdStatus     = 0x640000;
	IDEC->SecBmDescTableAddr = 0;
#else
	IDEC->PriBmCmdStatus     = 0x240000; // tht for 2.6 from 40000H
	IDEC->PriBmDescTableAddr = 0;
	IDEC->SecBmCmdStatus     = 0x240000;
	IDEC->SecBmDescTableAddr = 0;
#endif


	// Set endian swap based on the processor mode
	if (read_c0_config() & 0x00008000) {
		// Big-Endian MIPS
		// PR 12032 Swap dma descriptor when in big endian mode
		IDEC->IfControl |= 0x00120000;  // bit 20-21 pio swap, bit 17 desc table swap
#ifdef BCHP_IDE_EndianCtrl
	{
		/* On 7440 and later chip, Bus Master in Big Endian, LE being the default reset value */
		volatile unsigned long* pEndianCtrl = (volatile unsigned long*) 
			BCM_PHYS_TO_K1(BCHP_PHYSICAL_OFFSET+BCHP_IDE_EndianCtrl);
		*pEndianCtrl &= 0xfffffffe;
	}
#endif

	} else {    // Little-Endian MIPS
		// Endian 4 byte swap on a 32-bit boundary for DMA transfers
		// 0123 => 3210
		IDEC->IfControl |= 0x000C0000;
		// Don't swap PIO transfers - we're LE MIPS reading LE data

	}


	bcm71xx_ide_init_hwif(id0);

#if defined(CONFIG_MIPS_BCM7320) || defined(CONFIG_MIPS_BCM7317_IDE1)
	if (id1) {
		bcm71xx_ide_init_hwif(id1);
	}
#endif

#ifdef CONFIG_MIPS_BCM7440
   /*
	* HACK HACK HACK HACK
	* We have to call probe_hwif ourselves, since the PCI/SATA already done it,
	* without us onboard
	*/
	//probe_hwif(&ide_hwifs[2]);
#endif


	/* Linux did not wait for NOT busy when probing for drives.
	   So wait for it here. */
	while (IDEC->IdePriDevCStat & 0x80) {
		mdelay(50);
		if (count++ > 400) { printk("No IDE drive detected.\n"); break; }
	}
	local_irq_restore(flags);

	//printk("IDEC at end of bcm71xx_ide_init_hwif[0,1], IDEC=%08p\n", IDEC);
	//DumpRegs((uint32 *)IDEC);
	//printk("End of dump.  Probing for drives\n");

//printk("<-- %s\n", __FUNCTION__);
	return 0;

}

/* Can be called directly from ide.c. */
int bcm71xx_ide_init(void)
{
	if (bcm71xx_ide_probe()) {
		printk(KERN_ERR "bcm71xx_ide: ide interfaces already in use!\n");
		return -EBUSY;
	}
	return 0;
}

#ifdef MODULE /* To avoid double calls */
module_init(bcm71xx_ide_init);
#endif

MODULE_AUTHOR("Ton Truong");
MODULE_DESCRIPTION("PCI driver module for Brcm IDE");
MODULE_LICENSE("GPL");

