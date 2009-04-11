#
# Makefile for the Broadcom NAND MTD
#

obj-$(CONFIG_MTD_BRCMNAND)		+=  bcm7xxx-nand.o brcmnand_base.o brcmnand_bbt.o
obj-$(CONFIG_MTD_BRCMNAND_EDU)		+= edu.o
obj-$(CONFIG_MTD_BRCMNAND_CORRECTABLE_ERR_HANDLING)	+= brcmnand_cet.o
obj-$(CONFIG_MTD_BRCMNAND_USE_ISR)	+= brcmnand_isr.o
EXTRA_CFLAGS	+= -I $(TOPDIR)/include/asm-generic

