# SPDX-License-Identifier: GPL-2.0
#
# linux/drivers/mtd/devices/Makefile
#

obj-$(CONFIG_MTD_DOCG3)		+= docg3.o
obj-$(CONFIG_MTD_SLRAM)		+= slram.o
obj-$(CONFIG_MTD_PHRAM)		+= phram.o
obj-$(CONFIG_MTD_PMC551)	+= pmc551.o
obj-$(CONFIG_MTD_MS02NV)	+= ms02-nv.o
obj-$(CONFIG_MTD_MTDRAM)	+= mtdram.o
obj-$(CONFIG_MTD_LART)		+= lart.o
obj-$(CONFIG_MTD_BLOCK2MTD)	+= block2mtd.o
obj-$(CONFIG_MTD_DATAFLASH)	+= mtd_dataflash.o
obj-$(CONFIG_MTD_MCHP23K256)	+= mchp23k256.o
obj-$(CONFIG_MTD_MCHP48L640)	+= mchp48l640.o
obj-$(CONFIG_MTD_SPEAR_SMI)	+= spear_smi.o
obj-$(CONFIG_MTD_SST25L)	+= sst25l.o
obj-$(CONFIG_MTD_BCM47XXSFLASH)	+= bcm47xxsflash.o
obj-$(CONFIG_MTD_ST_SPI_FSM)    += st_spi_fsm.o
obj-$(CONFIG_MTD_POWERNV_FLASH)	+= powernv_flash.o
ifeq ($(CONFIG_ARCH_AMBARELLA_AMBALINK),y)
obj-$(CONFIG_MTD_AMBARELLA_SPI_NOR)	+= ambarella_spi_nor.o
ambarella_spi_nor-objs := ambarella_spinor.o spinor.o
endif
CFLAGS_docg3.o			+= -I$(src)
