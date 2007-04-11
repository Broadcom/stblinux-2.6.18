/***********************************************************************/
/*                                                                     */
/*   MODULE:  bcmupg.h                                                 */
/*   DATE:    02/15/02                                                 */
/*   PURPOSE: Definitions for UPG block                                */
/*                                                                     */
/***********************************************************************/


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

#ifndef BCMUPG_H
#define BCMUPG_H

#ifdef _cplusplus
extern "C" {
#endif

#define BCM7110_UPG			/* define this for 7110, else 7110 */

/* UPG Broadcom default register base */
#define UPG_IRB_BASE		(UPG_ADR_BASE+0x0000)    /* infrared blaster registers */
#define UPG_CONFIG_BASE		(UPG_ADR_BASE+0x004f)    /* pm1 configuration registers */
#define UPG_GIO_BASE		(UPG_ADR_BASE+0x0050)    /* gpio registers */
#define UPG_MGIO_BASE		(UPG_ADR_BASE+0x0070)    /* muxed gpio registers */
#define UPG_LDK_BASE		(UPG_ADR_BASE+0x0090)    /* led & keypad registers */
#define UPG_UART_A_BASE		(UPG_ADR_BASE+0x00b0)    /* uart A registers */
#define UPG_UART_B_BASE		(UPG_ADR_BASE+0x00c0)    /* uart B registers */
#define UPG_RFTUNE_BASE		(UPG_ADR_BASE+0x00d0)    /* rf tuner registers */
#define UPG_KBD_BASE		(UPG_ADR_BASE+0x00e0)    /* keyboard/remote receiver registers */
#define UPG_MGIOSEL_BASE	(UPG_ADR_BASE+0x00f0)    /* mgpio select registers */
#define UPG_INTC_BASE		(UPG_ADR_BASE+0x00fc)    /* interrupt registers */
#define UPG_SPI_BASE		(UPG_ADR_BASE+0x0100)    /* master spi registers */
#define UPG_EMGIO_BASE		(UPG_ADR_BASE+0x0160)    /* extended muxed gpio registers */
#define UPG_ICAP_BASE		(UPG_ADR_BASE+0x0170)    /* input capture registers */
#define UPG_I2C_BASE		(UPG_ADR_BASE+0x0180)    /* master i2c registers */
#define UPG_UART_C_BASE		(UPG_ADR_BASE+0x01a0)    /* uart C registers */
#define UPG_UART_D_BASE		(UPG_ADR_BASE+0x01a8)    /* uart D registers */
#define UPG_PWM_BASE		(UPG_ADR_BASE+0x01b0)    /* pwm generators registers */
#define UPG_SCA_BASE		(UPG_ADR_BASE+0x01c0)    /* smart card registers */
#define UPG_SCB_BASE		(UPG_ADR_BASE+0x01e0)    /* smart card B registers */

/* UART registers */
#define UARTA_RXSTAT     	0x00
#define UARTA_RXDATA     	0x01
#define UARTA_CONTROL    	0x03
#define UARTA_BAUDHI     	0x04
#define UARTA_BAUDLO     	0x05
#define UARTA_TXSTAT     	0x06
#define UARTA_TXDATA     	0x07

/* RXSTAT bit definitions */
#define PARERR				0x20
#define FRAMEERR			0x10
#define OVERRUNERR			0x08
#define RXDATARDY			0x04
#define RXINTEN				0x02
/* CONTROL bit definitions */
#define BITM8				0x10
#define	PAREN				0x08
#define	TXEN				0x04
#define	RXEN				0x02
#define	PODD				0x01
/* TXSTAT bit definitions */
#define	TXINTEN				0x04
#define	TXIDLE				0x02
#define	TXDREGEMT			0x01

/* Peripheral Interrupt and Status register's offset.	*/
#define	PM1_IRQEN_HI		0x00
#define PM1_IRQEN_LO		0x01
#define PM1_IRQSTAT_HI		0x02
#define PM1_IRQSTAT_LO		0x03

#ifndef _ASMLANGUAGE
typedef enum {
    LED_1A = 1,
    LED_2B,
    LED_3C,
    LED_4D
}LEDDEFS;

#define LEDON				1
#define LEDOFF				0

/**********************************************************************
  Uart Register Structure
 **********************************************************************/
typedef struct UartChannel {
  volatile unsigned char rxstat;
#define PARERR		0x20
#define FRAMEERR	0x10
#define OVERRUNERR	0x08
#define RXDATARDY	0x04
#define RXINTEN		0x02
  volatile unsigned char rxdata;
  volatile unsigned char unused0;
  volatile unsigned char control;
#define BITM8		0x10
#define	PAREN		0x08
#define	TXEN		0x04
#define	RXEN		0x02
#define	PODD		0x01
  volatile unsigned char baudh;
  /* When divide SysClk/2/(1+baudword) we should get 32*bit-rate
   */
  volatile unsigned char baudl;
  volatile unsigned char txstat;
#define	TXINTEN		0x04
#define	TXIDLE		0x02
#define	TXDREGEMT	0x01
  volatile unsigned char txdata;

  volatile unsigned long unused1;
  volatile unsigned long unused2;
} UartChannel;

#define UARTA ((volatile UartChannel *) UPG_UART_A_BASE)
#define UARTB ((volatile UartChannel *) UPG_UART_B_BASE)


/* UPG interrupt control registers */
#define PM1_IRQEN_HI     0x00
#define PM1_IRQEN_LO     0x01
#define PM1_IRQSTAT_HI   0x02
#define PM1_IRQSTAT_LO   0x03

typedef struct
{
   char irqen_h;
   char irqen_l;
   char irqstat_h;
#define UPG_IIC_IRQ			0x01
#define UPG_SPI_IRQ			0x02
#define UPG_GIO_IRQ			0x04
#define UPG_ICAP_IRQ		0x08
#define UPG_UD_IRQ			0x10
   char irqstat_l;
#define UPG_IRR_IRQ			0x01
#define UPG_LDK_IRQ			0x02
#define UPG_IRB_IRQ			0x04
#define UPG_UB_IRQ			0x08
#define UPG_UA_IRQ			0x10
#define UPG_UC_IRQ			0x20
#define UPG_SCA_IRQ			0x40
#define UPG_SCB_IRQ			0x80
} UpgIntc;

#define UPG_INTC		((volatile UpgIntc *) UPG_INTC_BASE)

#endif /* _ASMLANGUAGE */

/******************************************************************
 * Baud Rate Table
 * XTALFREQ / baud rate / 16
 ******************************************************************/
#define BAUD_1200_HI    0x01
#define BAUD_1200_LO    0xB2
#define BAUD_2400_HI    0x00
#define BAUD_2400_LO    0xD9
#define BAUD_4800_HI    0x00
#define BAUD_4800_LO    0x6C
#define BAUD_9600_HI    0x00
#define BAUD_9600_LO    0x36
#define BAUD_19200_HI   0x00
#define BAUD_19200_LO   0x1B
#define BAUD_38400_HI   0x00
#define BAUD_38400_LO   0x0D
#define BAUD_57600_HI   0x00
#define BAUD_57600_LO   0x09
#define BAUD_115200_HI  0x00
#define BAUD_115200_LO  0x04

/**********************************************************************
 * General purpose UPG device
 **********************************************************************/
/*
 * PM1_CONFIG register
 */
#define UPG_PM1_CONFIG				(UPG_CONFIG_BASE-UPG_ADR_BASE)
#define PM1_CONFIG_LEGACY			0x01
#define PM1_CONFIG_RFTUNE_NORMAL	0x02
#define PM1_CONFIG_I2C_TUN_MASK		0x0C
#define PM1_CONFIG_I2C_TUN_SHIFT	0x02

/*
 * RFTUNE register
 */
#define UPG_RFTUNE_REG				(UPG_RFTUNE_BASE-UPG_ADR_BASE)
#define UPG_RFTUNE_RFTE0			0x01
#define UPG_RFTUNE_RFTE1			0x02
#define UPG_RFTUNE_RFTD				0x04
#define UPG_RFTUNE_RFTCK			0x08

/******************************************************************
 * SPI Master Controller
 ******************************************************************/
#define SPI_SPCR0_LSB    			0x00
#define SPI_SPCR0_MSB    			0x01
#define SPI_SPCR1_LSB    			0x02
#define SPI_SPCR1_MSB    			0x03
#define SPI_NEWQP    				0x04
#define SPI_ENDQP    				0x05
#define SPI_SPCR2    				0x06
#define SPI_STATUS    				0x08
#define SPI_CPTPQ    				0x09
#define SPI_TXRAM    				0x10
#define SPI_RXRAM    				0x30
#define SPI_CDRAM    				0x50

/*
 * SPCR0_MSB
 */
#define SPI_SPCR0_MSB_MSTR			0x80
#define SPI_SPCR0_MSB_BITS_MASK		0x3C
#define SPI_SPCR0_MSB_BITS_SHIFTS	0x02
#define SPI_SPCR0_MSB_CPOL			0x02
#define SPI_SPCR0_MSB_CPHA			0x01

/*
 * SPCR2
 */
#define SPI_SPCR2_SPE				0x40
#define SPI_SPCR2_SPIFIE			0x20
#define SPI_SPCR2_WREN				0x10
#define SPI_SPCR2_WRT0				0x08
#define SPI_SPCR2_LOOPQ				0x04
#define SPI_SPCR2_HIE				0x02
#define SPI_SPCR2_HALT				0x01

/*
 * STATUS
 */
#define SPI_STATUS_HALTA			0x02
#define SPI_STATUS_SPIF				0x01

/*
 * CDRAM
 */
#define SPI_CDRAM_CONT				0x80
#define SPI_CDRAM_BITSE				0x40
#define SPI_CDRAM_DT				0x20
#define SPI_CDRAM_DSCK				0x10
#define SPI_CDRAM_PCS_MASK			0x07

#define SPI_CDRAM_PCS_PCS0			0x01
#define SPI_CDRAM_PCS_PCS1			0x02
#define SPI_CDRAM_PCS_PCS2			0x04

#define SPI_CDRAM_PCS_DISABLE_ALL	(SPI_CDRAM_PCS_PCS0 | SPI_CDRAM_PCS_PCS1 | SPI_CDRAM_PCS_PCS2)
/*
 * SPI command byte
 */
#define SPI_CMD_3125_IB				0x28
#define SPI_CMD_3125_OB				0x28
#define SPI_CMD_3125_US				0x68

#define SPI_CMD_3037				0x0C

#define SPI_CMD_3250_IB				0x28
#define SPI_CMD_3250_OB				0x3C
#define SPI_CMD_3250_US				0x0C

#define SPI_CMD_WRITE				0x01

/* Write commands */
#define SPI_CMD_3125_IB_WRITE	((SPI_CMD_3125_IB << 1) | SPI_CMD_WRITE)
#define SPI_CMD_3125_OB_WRITE	((SPI_CMD_3125_OB << 1) | SPI_CMD_WRITE)
#define SPI_CMD_3125_US_WRITE	((SPI_CMD_3125_US << 1) | SPI_CMD_WRITE)
#define SPI_CMD_3037_WRITE		((SPI_CMD_3037    << 1) | SPI_CMD_WRITE)
#define SPI_CMD_3250_IB_WRITE	((SPI_CMD_3250_IB << 1) | SPI_CMD_WRITE)
#define SPI_CMD_3250_OB_WRITE	((SPI_CMD_3250_OB << 1) | SPI_CMD_WRITE)
#define SPI_CMD_3250_US_WRITE	((SPI_CMD_3250_US << 1) | SPI_CMD_WRITE)

/* Read commands */
#define SPI_CMD_3125_IB_READ	(SPI_CMD_3125_IB << 1)
#define SPI_CMD_3125_OB_READ	(SPI_CMD_3125_OB << 1)
#define SPI_CMD_3125_US_READ	(SPI_CMD_3125_US << 1)
#define SPI_CMD_3037_READ		(SPI_CMD_3037 << 1)
#define SPI_CMD_3250_IB_READ	(SPI_CMD_3250_IB << 1)
#define SPI_CMD_3250_OB_READ	(SPI_CMD_3250_OB << 1)
#define SPI_CMD_3250_US_READ	(SPI_CMD_3250_US << 1)

/**********************************************************************
 * IIC_Registers
 **********************************************************************/
#define IIC_CHIP_ADDRESS            0x00 /* Chip Address Register */
#define IIC_DATA_IN_BYTE0           0x01 /* DATA_IN_0 Register */
#define IIC_DATA_IN_BYTE1           0x02 /* DATA_IN_1 Register */
#define IIC_DATA_IN_BYTE2           0x03 /* DATA_IN_2 Register */
#define IIC_DATA_IN_BYTE3           0x04 /* DATA_IN_3 Register */
#define IIC_DATA_IN_BYTE4           0x05 /* DATA_IN_4 Register */
#define IIC_DATA_IN_BYTE5           0x06 /* DATA_IN_5 Register */
#define IIC_DATA_IN_BYTE6           0x07 /* DATA_IN_6 Register */
#define IIC_DATA_IN_BYTE7           0x08 /* DATA_IN_7 Register */
#define IIC_CNT_REG                 0x09 /* Counter Register */
#define IIC_CTL_REG                 0x0A /* Control Register */
#define IIC_GO                      0x0B /* Enable Register */
#define IIC_DATA_OUT_BYTE0          0x0C /* DATA_OUT_0 Register */
#define IIC_DATA_OUT_BYTE1          0x0D /* DATA_OUT_1 Register */
#define IIC_DATA_OUT_BYTE2          0x0E /* DATA_OUT_2 Register */
#define IIC_DATA_OUT_BYTE3          0x0F /* DATA_OUT_3 Register */
#define IIC_DATA_OUT_BYTE4          0x10 /* DATA_OUT_4 Register */
#define IIC_DATA_OUT_BYTE5          0x11 /* DATA_OUT_5 Register */
#define IIC_DATA_OUT_BYTE6          0x12 /* DATA_OUT_6 Register */
#define IIC_DATA_OUT_BYTE7          0x13 /* DATA_OUT_7 Register */

/****************************************************************
 * Upg GPIO relative offsets.
 ****************************************************************/
#define REG_UPG_GIO_ODMSB      ( 0x00 )
#define REG_UPG_GIO_ODHSB      ( 0x01 )
#define REG_UPG_GIO_OUTHSB     ( 0x02 )
#define REG_UPG_GIO_OUTLSB     ( 0x03 )
#define REG_UPG_GIO_DIRMSB     ( 0x04 )
#define REG_UPG_GIO_OUTMSB     ( 0x05 )
#define REG_UPG_GIO_DIRHSB     ( 0x06 )
#define REG_UPG_GIO_DIRLSB     ( 0x07 )
#define REG_UPG_GIO_ODLSB      ( 0x08 )
#define REG_UPG_GIO_ECMSB      ( 0x09 )
#define REG_UPG_GIO_ECHSB      ( 0x0A )
#define REG_UPG_GIO_ECLSB      ( 0x0B )
#define REG_UPG_GIO_EIMSB      ( 0x0C )
#define REG_UPG_GIO_EIHSB      ( 0x0D )
#define REG_UPG_GIO_EILSB      ( 0x0E )
#define REG_UPG_GIO_MASKMSB    ( 0x0F )
#define REG_UPG_GIO_MASKHSB    ( 0x10 )
#define REG_UPG_GIO_MASKLSB    ( 0x11 )
#define REG_UPG_GIO_RSTMSB     ( 0x12 )
#define REG_UPG_GIO_RSTHSB     ( 0x13 )
#define REG_UPG_GIO_RSTLSB     ( 0x14 )
#define REG_UPG_GIO_STATMSB    ( 0x15 )
#define REG_UPG_GIO_STATHSB    ( 0x16 )
#define REG_UPG_GIO_STATLSB    ( 0x17 )
#define REG_UPG_GIO_OD2MSB	   ( 0x18 )
#define REG_UPG_GIO_OUT2MSB	   ( 0x19 )
#define REG_UPG_GIO_DIR2MSB	   ( 0x1A )
#define REG_UPG_GIO_EC2MSB	   ( 0x1B )
#define REG_UPG_GIO_EI2MSB	   ( 0x1C )
#define REG_UPG_GIO_MASK2MSB   ( 0x1D )
#define REG_UPG_GIO_RST2MSB	   ( 0x1E )
#define REG_UPG_GIO_STAT2MSB   ( 0x1F )

/* These MGIO registers are offsets from GPIO base.  This allows
   us to use the GPIO device to access these registers without
   having to create another device. */   
#define REG_UPG_MGIO_ODMSB     ( 0x20 )
#define REG_UPG_MGIO_OD2MSB    ( 0x21 )
#define REG_UPG_MGIO_ODHSB     ( 0x22 )
#define REG_UPG_MGIO_ODLSB     ( 0x23 )
#define REG_UPG_MGIO_OUTMSB    ( 0x24 )
#define REG_UPG_MGIO_OUT2MSB   ( 0x25 )
#define REG_UPG_MGIO_OUTHSB    ( 0x26 )
#define REG_UPG_MGIO_OUTLSB    ( 0x27 )
#define REG_UPG_MGIO_DIRMSB    ( 0x28 )
#define REG_UPG_MGIO_DIR2MSB   ( 0x29 )
#define REG_UPG_MGIO_DIRHSB    ( 0x2A )
#define REG_UPG_MGIO_DIRLSB    ( 0x2B )
#define REG_UPG_MGIO_ECMSB     ( 0x2C )
#define REG_UPG_MGIO_EC2MSB    ( 0x2D )
#define REG_UPG_MGIO_ECHSB     ( 0x2E )
#define REG_UPG_MGIO_ECLSB     ( 0x2F )
#define REG_UPG_MGIO_EIMSB     ( 0x30 )
#define REG_UPG_MGIO_EI2MSB    ( 0x31 )
#define REG_UPG_MGIO_EIHSB     ( 0x32 )
#define REG_UPG_MGIO_EILSB     ( 0x33 )
#define REG_UPG_MGIO_MASKMSB   ( 0x34 )
#define REG_UPG_MGIO_MASK2MSB  ( 0x35 )
#define REG_UPG_MGIO_MASKHSB   ( 0x36 )
#define REG_UPG_MGIO_MASKLSB   ( 0x37 )
#define REG_UPG_MGIO_RSTMSB	   ( 0x38 )
#define REG_UPG_MGIO_RST2MSB   ( 0x39 )
#define REG_UPG_MGIO_RSTHSB	   ( 0x3A )
#define REG_UPG_MGIO_RSTLSB	   ( 0x3B )
#define REG_UPG_MGIO_STATMSB   ( 0x3C )
#define REG_UPG_MGIO_STAT2MSB  ( 0x3D )
#define REG_UPG_MGIO_STATHSB   ( 0x3E )
#define REG_UPG_MGIO_STATLSB   ( 0x3F )

/* These MGIOSEL registers are offsets from GPIO base.  
   This allows us to use the GPIO device to access these 
   registers without having to create another device. */   
#define REG_UPG_MGIOSEL_BYTE0  ( 0xA0 )
#define REG_UPG_MGIOSEL_BYTE1  ( 0xA1 )
#define REG_UPG_MGIOSEL_BYTE2  ( 0xA2 )
#define REG_UPG_MGIOSEL_BYTE3  ( 0xA3 )
#define REG_UPG_MGIOSEL_BYTE4  ( 0xA4 )
#define REG_UPG_MGIOSEL_BYTE5  ( 0xA5 )

/* These MGIOEXT registers are offsets from GPIO base.  
   This allows us to use the GPIO device to access these 
   registers without having to create another device. */   
#define REG_UPG_MGIOEXT_OUTHSB  ( 0x110 )
#define REG_UPG_MGIOEXT_OUTLSB  ( 0x111 )
#define REG_UPG_MGIOEXT_DIRHSB  ( 0x112 )
#define REG_UPG_MGIOEXT_DIRLSB  ( 0x113 )
#define REG_UPG_MGIOEXT_ODHSB   ( 0x114 )
#define REG_UPG_MGIOEXT_ODLSB   ( 0x115 )
#define REG_UPG_MGIOEXT_ECHSB   ( 0x116 )
#define REG_UPG_MGIOEXT_ECLSB   ( 0x117 )
#define REG_UPG_MGIOEXT_EIHSB   ( 0x118 )
#define REG_UPG_MGIOEXT_EILSB   ( 0x119 )
#define REG_UPG_MGIOEXT_MASKHSB ( 0x11A )
#define REG_UPG_MGIOEXT_MASKLSB ( 0x11B )
#define REG_UPG_MGIOEXT_RSTHSB  ( 0x11C )
#define REG_UPG_MGIOEXT_RSTLSB  ( 0x11D )
#define REG_UPG_MGIOEXT_STATHSB ( 0x11E )
#define REG_UPG_MGIOEXT_STATLSB ( 0x11F )
   
/****************************************************************
 * UPG Interrupt and Status Registers
 ****************************************************************/

/* UPG Interrupt Enable and Status Register Address */
/* offsets from PMRegIntStatBase  */
/* NOTE:	UPG prefix was maintained to more closely match the data sheets */
/*			even though the module is now call the Peripheral Module */
#define REG_UPG_IRQEN_HI		0x0000
#define REG_UPG_IRQEN_LO		0x0001
#define REG_UPG_IRQSTAT_HI		0x0002
#define REG_UPG_IRQSTAT_LO		0x0003

/* UPG Interrupt Enable/Status Bitfield definitions */
/* UPG REG_UPG_IRQEN_LO and REG_UPG_IRQSTAT_LO */	   
#define UPG_IRR_IRQ			0x01	/* Keyboard/remot receiver */
#define UPG_LDK_IRQ			0x02	/* LED/keypad */
#define UPG_IRB_IRQ			0x04	/* Infrared Blaster */
#define UPG_UB_IRQ			0x08	/* UART B */
#define UPG_UA_IRQ			0x10	/* UART A */
#define UPG_UC_IRQ			0x20	/* UART C */
#define UPG_SCA_IRQ			0x40	/* Smart card A */
#ifdef BCM7110_UPG
#define	UPG_IRQ_LO_ALL		0x7F	/* all interrupts at IRQ_LO */
#else
#define UPG_SCB_IRQ			0x80	/* Smart card B */
#define	UPG_IRQ_LO_ALL		0xFF	/* all interrupts at IRQ_LO */
#endif

/* UPG REG_UPG_IRQEN_HI and REG_UPG_IRQSTAT_HI */
#define UPG_IIC_IRQ			0x01	/* IIC */
#define UPG_SPI_IRQ			0x02	/* SPI */

#ifdef BCM7110_UPG
#define UPG_GIO_IRQ			0x04	/* GPIO */
#define UPG_ICAP_IRQ		0x08	/* Input capture */
#define UPG_UD_IRQ			0x10	/* UART D */
#define	UPG_IRQ_HI_ALL		0x1F	/* all interrupts at IRQ_HI */
#else
#define	UPG_IRQ_HI_ALL		0x03	/* all interrupts at IRQ_HI */
#endif

/************************************************************/
/* IR Blaster register definitions										*/
/************************************************************/
#define BLAST_NUMSEQ            0x00000000      /* Blaster number sequence      */
#define BLAST_CONTROL           0x00000001      /* Blaster control              */
#define BLAST_INDXPRE           0x00000002
#define BLAST_PRIMPRE           0x00000003
#define BLAST_REPINDX           0x00000004
#define BLAST_REPNUM            0x00000005
#define BLAST_CARRHI            0x00000006
#define BLAST_CARRLO            0x00000007
#define BLAST_SEQ               0x00000008
#define BLAST_MOD               0x00000030

/* Bit and mask definitions for control and number of sequences register */
#define IRB_CARRINH         0x0001  /* Carrier inhibit (Flash IR)
                                    0 = carrier modulated (PWM IR) */
#define IRB_COFFLEV         0x0002  /* logic level of carrier off */
#define IRB_IRQINH          0x0004  /* disable interrupt when set */
#define IRB_CLI             0x0008  /* clear interrupt */
#define IRB_RST             0x0010  /* reset IR blaster */
#define IRB_PAGE            0x0020  /* sequence page 0 or 1 select (0==page 0) */
#define IRB_INTENSIFY       0x0040  /* infra-red intensity level */
#define IRB_BLAST           0x0080  /* go initiate IR blasting sequence */
#define IRB_NUMSEQ          0x7F00  /* mask for number of sequences */
#define IRB_MAX_SEQ         80      /* max number of IRB sequences ver 1000 */
#define IRB_MAX_SEQ0        40      /* max number of IRB sequences ver 0000 */
#define IRB_PAGE_SIZE       40      /* page size in bytes */

/****************************************************************
 * Upg KBD relative offsets.
 *****************************************************************/								  
#define REG_UPG_KBD_STATUS     ( 0x00 )
#define REG_UPG_KBD_DATA3      ( 0x04 )
#define REG_UPG_KBD_DATA2      ( 0x05 )
#define REG_UPG_KBD_DATA1      ( 0x06 )
#define REG_UPG_KBD_DATA0      ( 0x07 )
#define REG_UPG_KBD_CMD        ( 0x08 )
#define REG_UPG_KBD_CONTROL    REG_UPG_KBD_CMD  /* alias */

/* CIR registers */
#define	REG_UPG_KBD_CIR_ADDR	(0x09)
#define	REG_UPG_KBD_CIR_MSB		(0x0a)
#define	REG_UPG_KBD_CIR_LSB		(0x0b)

/*
 * Definitions in Control register 
 *	lcwang added KBD_CONTROL_CIR_ENABLE for consumer remote controller.
 */
#define KBD_CONTROL_TWIRP_ENABLE       0x01
#define KBD_CONTROL_SEJIN_ENABLE       0x02
#define KBD_CONTROL_REMOTE_A_ENABLE    0x04
#define KBD_CONTROL_REMOTE_B_ENABLE    0x08
#define KBD_CONTROL_CIR_ENABLE         0x10
#define KBD_CONTROL_KBD_IRQEN          0x20
#define KBD_CONTROL_ALT_TABLE          0x40
#define KBD_CONTROL_TEST_MODE          0x80
#define KBD_CONTROL_MASK               0xff	/* it was 0xef */

/*
 * Definitions in Status register - these can be used as masks.
 */
#define KBD_STATUS_IRQ                 0x01
#define KBD_STATUS_RFLAG               0x02
#define KBD_STATUS_TWIRP               0x04
#define KBD_STATUS_DEVICE              0x1c
#define KBD_STATUS_MASK                0x1f

#define KBD_IS_TWIRP(x)      ((((x) & KBD_STATUS_DEVICE) >> 2) == 1)
#define KBD_IS_SEJIN(x)      ((((x) & KBD_STATUS_DEVICE) >> 2) == 2)
#define KBD_IS_REMOTE_A(x)   ((((x) & KBD_STATUS_DEVICE) >> 2) == 3)
#define KBD_IS_REMOTE_B(x)   ((((x) & KBD_STATUS_DEVICE) >> 2) == 4)

#define KBD_STATUS_DEVICE_SHIFTS			2

/****************************************************************
 * Upg LED register relative offsets.
 * The REG in the front is used to identify that these constants
 * are register offsets.
 ****************************************************************/
#define REG_UPG_LED_KEYROW32     ( 0x00 )
#define REG_UPG_LED_KEYROW10     ( 0x01 )
#define REG_UPG_LED_PRESCHI      ( 0x02 )
#define REG_UPG_LED_PRESCLO      ( 0x03 )
#define REG_UPG_LED_DUTYOFF      ( 0x04 )
#define REG_UPG_LED_DUTYON       ( 0x05 )
#define REG_UPG_LED_DIGIT2       ( 0x06 )
#define REG_UPG_LED_DIGIT1       ( 0x07 )
#define REG_UPG_LED_DIGIT4       ( 0x08 )
#define REG_UPG_LED_DIGIT3       ( 0x09 )
#define REG_UPG_LED_DEBOUNCE     ( 0x0a )
#define REG_UPG_LED_STATE        ( 0x0b )  /* Don't use these */
#define REG_UPG_LED_IRQEN        ( 0x0d )  /* use the ones below */

/*
 * This register defines are used in the code.  It is easier to
 * think of test registers as STATUS and CONTROL than in other
 * random names.
 */
#define REG_UPG_LED_STATUS       REG_UPG_LED_STATE  /* alias */
#define REG_UPG_LED_CONTROL      REG_UPG_LED_IRQEN  /* alias */

/*
 * Definitions in Control register 
 */
#define	LED_CONTROL_DISABLE	0x00	/* disable LED CONTROL */
#define LED_CONTROL_SWR     0x01  /* software reset */
#define LED_CONTROL_IRQEN   0x02  /* enable irq */

/*
 * Definition for Status register:  bits 0=ON, 1=OFF   
 */
#define LED_STATUS_MESS    0x01  /* message avaible */
#define LED_STATUS_BYPS    0x02  /* operation is in progress */
#define LED_STATUS_DIGI    0x04  /* digital operation */
#define LED_STATUS_REMT    0x08  /* remote data detected */

/****************************************************************
 * Upg PWM relative offsets.
 ****************************************************************/
#define REG_UPG_PWM_CTRL      		0x00
#define REG_UPG_PWM1_CWORD_MSB      0x02
#define REG_UPG_PWM1_CWORD_LSB      0x03
#define REG_UPG_PWM2_CWORD_MSB      0x04
#define REG_UPG_PWM2_CWORD_LSB      0x05

/* PWM_CTRL register */
#define UPG_PWM1_START				0x01
#define UPG_PWM1_OEB				0x02
#define UPG_PWM1_FORCE_HIGH			0x04
#define UPG_PWM1_OPEN_DRAINB		0x08
#define UPG_PWM2_START				0x10
#define UPG_PWM2_OEB				0x20
#define UPG_PWM2_FORCE_HIGH			0x40
#define UPG_PWM2_OPEN_DRAINB		0x80

/****************************************************************
 * Upg UART register relative offsets.
 * The REG in the front is used to identify that these constants
 * are register offsets.
 ****************************************************************/
#define REG_UPG_UART_RCVSTATUS  ( 0x00 )
#define REG_UPG_UART_RCVDATA    ( 0x01 )
#define REG_UPG_UART_CONTROL    ( 0x03 )
#define REG_UPG_UART_BAUDHI     ( 0x04 )
#define REG_UPG_UART_BAUDLO     ( 0x05 )
#define REG_UPG_UART_XMTSTATUS  ( 0x06 )
#define REG_UPG_UART_XMTDATA    ( 0x07 )

/*
 * Definitions in Control register 
 */
#define UART_CONTROL_PODD        0x01  /* odd parity */
#define UART_CONTROL_RXE         0x02  /* receiver enable */
#define UART_CONTROL_TXE         0x04  /* transmitter enable */
#define UART_CONTROL_PAREN       0x08  /* parity enable */
#define UART_CONTROL_BITM8       0x10  /* 8 bits character */
#define UART_CONTROL_LOOP        0x20  /* loop back */

/*
 * Definition for Status register:
 */
#define UART_RCVSTATUS_RIE       0x02  /* receiver interrupt enable */
#define UART_RCVSTATUS_RDA       0x04  /* receiver data register full flag */
#define UART_RCVSTATUS_OVRN      0x08  /* data overrun error */
#define UART_RCVSTATUS_FE        0x10  /* framing error */
#define UART_RCVSTATUS_PE        0x20  /* parity error */

/*
 * UART transmitter status and control register definitions 
 */
#define UART_XMTSTATUS_TDRE      0x01   /* transmit data register empty flag */
#define UART_XMTSTATUS_IDLE      0x02   /* transmit in idle state */
#define UART_XMTSTATUS_TIE       0x04   /* transmit interrupt enable */

/**********************************************************************
 * IIC_CHIP_ADDRESS
 **********************************************************************/
/* reserved0 [31:08] -  */
#define IIC_CHIP_ADDRESS_reserved0_MASK                 0xFFFFFF00UL
#define IIC_CHIP_ADDRESS_reserved0_ALIGN                0
#define IIC_CHIP_ADDRESS_reserved0_BITS                 24
#define IIC_CHIP_ADDRESS_reserved0_SHIFT                8

/* CHIP_ADDRESS [07:01] - unsigned */
#define IIC_CHIP_ADDRESS_CHIP_ADDRESS_MASK              0x000000FEUL
#define IIC_CHIP_ADDRESS_CHIP_ADDRESS_ALIGN             0
#define IIC_CHIP_ADDRESS_CHIP_ADDRESS_BITS              7
#define IIC_CHIP_ADDRESS_CHIP_ADDRESS_SHIFT             1

/* reserved1 [00:00] -  */
#define IIC_CHIP_ADDRESS_reserved1_MASK                 0x00000001UL
#define IIC_CHIP_ADDRESS_reserved1_ALIGN                0
#define IIC_CHIP_ADDRESS_reserved1_BITS                 1
#define IIC_CHIP_ADDRESS_reserved1_SHIFT                0

/**********************************************************************
 * IIC_DATA_IN_BYTE0
 **********************************************************************/
/* reserved0 [31:08] -  */
#define IIC_DATA_IN_BYTE0_reserved0_MASK                0xFFFFFF00UL
#define IIC_DATA_IN_BYTE0_reserved0_ALIGN               0
#define IIC_DATA_IN_BYTE0_reserved0_BITS                24
#define IIC_DATA_IN_BYTE0_reserved0_SHIFT               8

/* DATA_IN_0 [07:00] - unsigned */
#define IIC_DATA_IN_BYTE0_DATA_IN_0_MASK                0x000000FFUL
#define IIC_DATA_IN_BYTE0_DATA_IN_0_ALIGN               0
#define IIC_DATA_IN_BYTE0_DATA_IN_0_BITS                8
#define IIC_DATA_IN_BYTE0_DATA_IN_0_SHIFT               0

/**********************************************************************
 * IIC_DATA_IN_BYTE1
 **********************************************************************/
/* reserved0 [31:08] -  */
#define IIC_DATA_IN_BYTE1_reserved0_MASK                0xFFFFFF00UL
#define IIC_DATA_IN_BYTE1_reserved0_ALIGN               0
#define IIC_DATA_IN_BYTE1_reserved0_BITS                24
#define IIC_DATA_IN_BYTE1_reserved0_SHIFT               8

/* DATA_IN_1 [07:00] - unsigned */
#define IIC_DATA_IN_BYTE1_DATA_IN_1_MASK                0x000000FFUL
#define IIC_DATA_IN_BYTE1_DATA_IN_1_ALIGN               0
#define IIC_DATA_IN_BYTE1_DATA_IN_1_BITS                8
#define IIC_DATA_IN_BYTE1_DATA_IN_1_SHIFT               0

/**********************************************************************
 * IIC_DATA_IN_BYTE2
 **********************************************************************/
/* reserved0 [31:08] -  */
#define IIC_DATA_IN_BYTE2_reserved0_MASK                0xFFFFFF00UL
#define IIC_DATA_IN_BYTE2_reserved0_ALIGN               0
#define IIC_DATA_IN_BYTE2_reserved0_BITS                24
#define IIC_DATA_IN_BYTE2_reserved0_SHIFT               8

/* DATA_IN_2 [07:00] - unsigned */
#define IIC_DATA_IN_BYTE2_DATA_IN_2_MASK                0x000000FFUL
#define IIC_DATA_IN_BYTE2_DATA_IN_2_ALIGN               0
#define IIC_DATA_IN_BYTE2_DATA_IN_2_BITS                8
#define IIC_DATA_IN_BYTE2_DATA_IN_2_SHIFT               0

/**********************************************************************
 * IIC_DATA_IN_BYTE3
 **********************************************************************/
/* reserved0 [31:08] -  */
#define IIC_DATA_IN_BYTE3_reserved0_MASK                0xFFFFFF00UL
#define IIC_DATA_IN_BYTE3_reserved0_ALIGN               0
#define IIC_DATA_IN_BYTE3_reserved0_BITS                24
#define IIC_DATA_IN_BYTE3_reserved0_SHIFT               8

/* DATA_IN_3 [07:00] - unsigned */
#define IIC_DATA_IN_BYTE3_DATA_IN_3_MASK                0x000000FFUL
#define IIC_DATA_IN_BYTE3_DATA_IN_3_ALIGN               0
#define IIC_DATA_IN_BYTE3_DATA_IN_3_BITS                8
#define IIC_DATA_IN_BYTE3_DATA_IN_3_SHIFT               0

/**********************************************************************
 * IIC_DATA_IN_BYTE4
 **********************************************************************/
/* reserved0 [31:08] -  */
#define IIC_DATA_IN_BYTE4_reserved0_MASK                0xFFFFFF00UL
#define IIC_DATA_IN_BYTE4_reserved0_ALIGN               0
#define IIC_DATA_IN_BYTE4_reserved0_BITS                24
#define IIC_DATA_IN_BYTE4_reserved0_SHIFT               8

/* DATA_OUT_4 [07:00] - unsigned */
#define IIC_DATA_IN_BYTE4_DATA_OUT_4_MASK               0x000000FFUL
#define IIC_DATA_IN_BYTE4_DATA_OUT_4_ALIGN              0
#define IIC_DATA_IN_BYTE4_DATA_OUT_4_BITS               8
#define IIC_DATA_IN_BYTE4_DATA_OUT_4_SHIFT              0

/**********************************************************************
 * IIC_DATA_IN_BYTE5
 **********************************************************************/
/* reserved0 [31:08] -  */
#define IIC_DATA_IN_BYTE5_reserved0_MASK                0xFFFFFF00UL
#define IIC_DATA_IN_BYTE5_reserved0_ALIGN               0
#define IIC_DATA_IN_BYTE5_reserved0_BITS                24
#define IIC_DATA_IN_BYTE5_reserved0_SHIFT               8

/* DATA_IN_5 [07:00] - unsigned */
#define IIC_DATA_IN_BYTE5_DATA_IN_5_MASK                0x000000FFUL
#define IIC_DATA_IN_BYTE5_DATA_IN_5_ALIGN               0
#define IIC_DATA_IN_BYTE5_DATA_IN_5_BITS                8
#define IIC_DATA_IN_BYTE5_DATA_IN_5_SHIFT               0

/**********************************************************************
 * IIC_DATA_IN_BYTE6
 **********************************************************************/
/* reserved0 [31:08] -  */
#define IIC_DATA_IN_BYTE6_reserved0_MASK                0xFFFFFF00UL
#define IIC_DATA_IN_BYTE6_reserved0_ALIGN               0
#define IIC_DATA_IN_BYTE6_reserved0_BITS                24
#define IIC_DATA_IN_BYTE6_reserved0_SHIFT               8

/* DATA_IN_6 [07:00] - unsigned */
#define IIC_DATA_IN_BYTE6_DATA_IN_6_MASK                0x000000FFUL
#define IIC_DATA_IN_BYTE6_DATA_IN_6_ALIGN               0
#define IIC_DATA_IN_BYTE6_DATA_IN_6_BITS                8
#define IIC_DATA_IN_BYTE6_DATA_IN_6_SHIFT               0

/**********************************************************************
 * IIC_DATA_IN_BYTE7
 **********************************************************************/
/* reserved0 [31:08] -  */
#define IIC_DATA_IN_BYTE7_reserved0_MASK                0xFFFFFF00UL
#define IIC_DATA_IN_BYTE7_reserved0_ALIGN               0
#define IIC_DATA_IN_BYTE7_reserved0_BITS                24
#define IIC_DATA_IN_BYTE7_reserved0_SHIFT               8

/* DATA_IN_7 [07:00] - unsigned */
#define IIC_DATA_IN_BYTE7_DATA_IN_7_MASK                0x000000FFUL
#define IIC_DATA_IN_BYTE7_DATA_IN_7_ALIGN               0
#define IIC_DATA_IN_BYTE7_DATA_IN_7_BITS                8
#define IIC_DATA_IN_BYTE7_DATA_IN_7_SHIFT               0

/**********************************************************************
 * IIC_CNT_REG
 **********************************************************************/
/* reserved0 [31:08] -  */
#define IIC_CNT_REG_reserved0_MASK                      0xFFFFFF00UL
#define IIC_CNT_REG_reserved0_ALIGN                     0
#define IIC_CNT_REG_reserved0_BITS                      24
#define IIC_CNT_REG_reserved0_SHIFT                     8

/* CNT_REG2 [07:04] - unsigned */
#define IIC_CNT_REG_CNT_REG2_MASK                       0x000000F0UL
#define IIC_CNT_REG_CNT_REG2_ALIGN                      0
#define IIC_CNT_REG_CNT_REG2_BITS                       4
#define IIC_CNT_REG_CNT_REG2_SHIFT                      4

/* CNT_REG1 [03:00] - unsigned */
#define IIC_CNT_REG_CNT_REG1_MASK                       0x0000000FUL
#define IIC_CNT_REG_CNT_REG1_ALIGN                      0
#define IIC_CNT_REG_CNT_REG1_BITS                       4
#define IIC_CNT_REG_CNT_REG1_SHIFT                      0

/**********************************************************************
 * IIC_CTL_REG
 **********************************************************************/
/* reserved0 [31:08] -  */
#define IIC_CTL_REG_reserved0_MASK                      0xFFFFFF00UL
#define IIC_CTL_REG_reserved0_ALIGN                     0
#define IIC_CTL_REG_reserved0_BITS                      24
#define IIC_CTL_REG_reserved0_SHIFT                     8

/* SCL_DIV [07:07] - boolean */
#define IIC_CTL_REG_SCL_DIV_MASK                        0x00000080UL
#define IIC_CTL_REG_SCL_DIV_ALIGN                       0
#define IIC_CTL_REG_SCL_DIV_BITS                        1
#define IIC_CTL_REG_SCL_DIV_SHIFT                       7

/* INT_EN [06:06] - boolean */
#define IIC_CTL_REG_INT_EN_MASK                         0x00000040UL
#define IIC_CTL_REG_INT_EN_ALIGN                        0
#define IIC_CTL_REG_INT_EN_BITS                         1
#define IIC_CTL_REG_INT_EN_SHIFT                        6

/* SCL_SEL [05:04] - unsigned */
#define IIC_CTL_REG_SCL_SEL_MASK                        0x00000030UL
#define IIC_CTL_REG_SCL_SEL_ALIGN                       0
#define IIC_CTL_REG_SCL_SEL_BITS                        2
#define IIC_CTL_REG_SCL_SEL_SHIFT                       4

/* DELAY_DIS [03:03] - boolean */
#define IIC_CTL_REG_DELAY_DIS_MASK                      0x00000008UL
#define IIC_CTL_REG_DELAY_DIS_ALIGN                     0
#define IIC_CTL_REG_DELAY_DIS_BITS                      1
#define IIC_CTL_REG_DELAY_DIS_SHIFT                     3

/* DEGLITCH_DIS [02:02] - boolean */
#define IIC_CTL_REG_DEGLITCH_DIS_MASK                   0x00000004UL
#define IIC_CTL_REG_DEGLITCH_DIS_ALIGN                  0
#define IIC_CTL_REG_DEGLITCH_DIS_BITS                   1
#define IIC_CTL_REG_DEGLITCH_DIS_SHIFT                  2

/* DTF [01:00] - unsigned */
#define IIC_CTL_REG_DTF_MASK                            0x00000003UL
#define IIC_CTL_REG_DTF_ALIGN                           0
#define IIC_CTL_REG_DTF_BITS                            2
#define IIC_CTL_REG_DTF_SHIFT                           0

/**********************************************************************
 * IIC_GO
 **********************************************************************/
/* reserved0 [31:03] -  */
#define IIC_GO_reserved0_MASK                           0xFFFFFFF8UL
#define IIC_GO_reserved0_ALIGN                          0
#define IIC_GO_reserved0_BITS                           29
#define IIC_GO_reserved0_SHIFT                          3

/* NO_ACK [02:02] - boolean */
#define IIC_GO_NO_ACK_MASK                              0x00000004UL
#define IIC_GO_NO_ACK_ALIGN                             0
#define IIC_GO_NO_ACK_BITS                              1
#define IIC_GO_NO_ACK_SHIFT                             2

/* INTRP [01:01] - boolean */
#define IIC_GO_INTRP_MASK                               0x00000002UL
#define IIC_GO_INTRP_ALIGN                              0
#define IIC_GO_INTRP_BITS                               1
#define IIC_GO_INTRP_SHIFT                              1

/* ENABLE [00:00] - boolean */
#define IIC_GO_ENABLE_MASK                              0x00000001UL
#define IIC_GO_ENABLE_ALIGN                             0
#define IIC_GO_ENABLE_BITS                              1
#define IIC_GO_ENABLE_SHIFT                             0

/**********************************************************************
 * IIC_DATA_OUT_BYTE0
 **********************************************************************/
/* reserved0 [31:08] -  */
#define IIC_DATA_OUT_BYTE0_reserved0_MASK               0xFFFFFF00UL
#define IIC_DATA_OUT_BYTE0_reserved0_ALIGN              0
#define IIC_DATA_OUT_BYTE0_reserved0_BITS               24
#define IIC_DATA_OUT_BYTE0_reserved0_SHIFT              8

/* DATA_OUT_0 [07:00] - unsigned */
#define IIC_DATA_OUT_BYTE0_DATA_OUT_0_MASK              0x000000FFUL
#define IIC_DATA_OUT_BYTE0_DATA_OUT_0_ALIGN             0
#define IIC_DATA_OUT_BYTE0_DATA_OUT_0_BITS              8
#define IIC_DATA_OUT_BYTE0_DATA_OUT_0_SHIFT             0

/**********************************************************************
 * IIC_DATA_OUT_BYTE1
 **********************************************************************/
/* reserved0 [31:08] -  */
#define IIC_DATA_OUT_BYTE1_reserved0_MASK               0xFFFFFF00UL
#define IIC_DATA_OUT_BYTE1_reserved0_ALIGN              0
#define IIC_DATA_OUT_BYTE1_reserved0_BITS               24
#define IIC_DATA_OUT_BYTE1_reserved0_SHIFT              8

/* DATA_OUT_1 [07:00] - unsigned */
#define IIC_DATA_OUT_BYTE1_DATA_OUT_1_MASK              0x000000FFUL
#define IIC_DATA_OUT_BYTE1_DATA_OUT_1_ALIGN             0
#define IIC_DATA_OUT_BYTE1_DATA_OUT_1_BITS              8
#define IIC_DATA_OUT_BYTE1_DATA_OUT_1_SHIFT             0

/**********************************************************************
 * IIC_DATA_OUT_BYTE2
 **********************************************************************/
/* reserved0 [31:08] -  */
#define IIC_DATA_OUT_BYTE2_reserved0_MASK               0xFFFFFF00UL
#define IIC_DATA_OUT_BYTE2_reserved0_ALIGN              0
#define IIC_DATA_OUT_BYTE2_reserved0_BITS               24
#define IIC_DATA_OUT_BYTE2_reserved0_SHIFT              8

/* DATA_OUT_2 [07:00] - unsigned */
#define IIC_DATA_OUT_BYTE2_DATA_OUT_2_MASK              0x000000FFUL
#define IIC_DATA_OUT_BYTE2_DATA_OUT_2_ALIGN             0
#define IIC_DATA_OUT_BYTE2_DATA_OUT_2_BITS              8
#define IIC_DATA_OUT_BYTE2_DATA_OUT_2_SHIFT             0

/**********************************************************************
 * IIC_DATA_OUT_BYTE3
 **********************************************************************/
/* reserved0 [31:08] -  */
#define IIC_DATA_OUT_BYTE3_reserved0_MASK               0xFFFFFF00UL
#define IIC_DATA_OUT_BYTE3_reserved0_ALIGN              0
#define IIC_DATA_OUT_BYTE3_reserved0_BITS               24
#define IIC_DATA_OUT_BYTE3_reserved0_SHIFT              8

/* DATA_OUT_3 [07:00] - unsigned */
#define IIC_DATA_OUT_BYTE3_DATA_OUT_3_MASK              0x000000FFUL
#define IIC_DATA_OUT_BYTE3_DATA_OUT_3_ALIGN             0
#define IIC_DATA_OUT_BYTE3_DATA_OUT_3_BITS              8
#define IIC_DATA_OUT_BYTE3_DATA_OUT_3_SHIFT             0

/**********************************************************************
 * IIC_DATA_OUT_BYTE4
 **********************************************************************/
/* reserved0 [31:08] -  */
#define IIC_DATA_OUT_BYTE4_reserved0_MASK               0xFFFFFF00UL
#define IIC_DATA_OUT_BYTE4_reserved0_ALIGN              0
#define IIC_DATA_OUT_BYTE4_reserved0_BITS               24
#define IIC_DATA_OUT_BYTE4_reserved0_SHIFT              8

/* DATA_OUT_4 [07:00] - unsigned */
#define IIC_DATA_OUT_BYTE4_DATA_OUT_4_MASK              0x000000FFUL
#define IIC_DATA_OUT_BYTE4_DATA_OUT_4_ALIGN             0
#define IIC_DATA_OUT_BYTE4_DATA_OUT_4_BITS              8
#define IIC_DATA_OUT_BYTE4_DATA_OUT_4_SHIFT             0

/**********************************************************************
 * IIC_DATA_OUT_BYTE5
 **********************************************************************/
/* reserved0 [31:08] -  */
#define IIC_DATA_OUT_BYTE5_reserved0_MASK               0xFFFFFF00UL
#define IIC_DATA_OUT_BYTE5_reserved0_ALIGN              0
#define IIC_DATA_OUT_BYTE5_reserved0_BITS               24
#define IIC_DATA_OUT_BYTE5_reserved0_SHIFT              8

/* DATA_OUT_5 [07:00] - unsigned */
#define IIC_DATA_OUT_BYTE5_DATA_OUT_5_MASK              0x000000FFUL
#define IIC_DATA_OUT_BYTE5_DATA_OUT_5_ALIGN             0
#define IIC_DATA_OUT_BYTE5_DATA_OUT_5_BITS              8
#define IIC_DATA_OUT_BYTE5_DATA_OUT_5_SHIFT             0

/**********************************************************************
 * IIC_DATA_OUT_BYTE6
 **********************************************************************/
/* reserved0 [31:08] -  */
#define IIC_DATA_OUT_BYTE6_reserved0_MASK               0xFFFFFF00UL
#define IIC_DATA_OUT_BYTE6_reserved0_ALIGN              0
#define IIC_DATA_OUT_BYTE6_reserved0_BITS               24
#define IIC_DATA_OUT_BYTE6_reserved0_SHIFT              8

/* DATA_OUT_6 [07:00] - unsigned */
#define IIC_DATA_OUT_BYTE6_DATA_OUT_6_MASK              0x000000FFUL
#define IIC_DATA_OUT_BYTE6_DATA_OUT_6_ALIGN             0
#define IIC_DATA_OUT_BYTE6_DATA_OUT_6_BITS              8
#define IIC_DATA_OUT_BYTE6_DATA_OUT_6_SHIFT             0

/**********************************************************************
 * IIC_DATA_OUT_BYTE7
 **********************************************************************/
/* reserved0 [31:08] -  */
#define IIC_DATA_OUT_BYTE7_reserved0_MASK               0xFFFFFF00UL
#define IIC_DATA_OUT_BYTE7_reserved0_ALIGN              0
#define IIC_DATA_OUT_BYTE7_reserved0_BITS               24
#define IIC_DATA_OUT_BYTE7_reserved0_SHIFT              8

/* DATA_OUT_7 [07:00] - unsigned */
#define IIC_DATA_OUT_BYTE7_DATA_OUT_7_MASK              0x000000FFUL
#define IIC_DATA_OUT_BYTE7_DATA_OUT_7_ALIGN             0
#define IIC_DATA_OUT_BYTE7_DATA_OUT_7_BITS              8
#define IIC_DATA_OUT_BYTE7_DATA_OUT_7_SHIFT             0

/**********************************************************************
 *Enum: ACK_NOACK
 **********************************************************************/
#define IIC_ACK_NOACK_ACK                               0
#define IIC_ACK_NOACK_NO_ACK                            1

/**********************************************************************
 *Enum: DTF_SEL
 **********************************************************************/
#define IIC_DTF_SEL_WRITE_ONLY                          0
#define IIC_DTF_SEL_READ_ONLY                           1
#define IIC_DTF_SEL_COMB_RD_WR                          2
#define IIC_DTF_SEL_COMB_WR_RD                          3

/**********************************************************************
 *Enum: ENABLE_DISABLE
 **********************************************************************/
#define IIC_ENABLE_DISABLE_ENABLE                       1
#define IIC_ENABLE_DISABLE_DISABLE                      0

/**********************************************************************
 *Enum: READ_WRITE
 **********************************************************************/
#define IIC_READ_WRITE_READ                             1
#define IIC_READ_WRITE_WRITE                            0

#ifdef _cplusplus
}
#endif

#endif
