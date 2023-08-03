
obj-y += libbld_nand.a
obj-y += libbld_emmc.a
obj-y += libbld_spinor.a

COMMOBJ		:= AmbaBLD_Boot_A64.o AmbaBLD_HwStack_A64.o AmbaBLD_Shell.o AmbaDdrc_Write_Training_A64.o  AmbaDdrc_CA_Training_A64.o
COMMOBJ		+= AmbaBLD_MmuMap.o AmbaBLD_PsciOnEntry_A64.o AmbaBLD_handler.o
ifdef CONFIG_DRAM_TYPE_DDR4
COMMOBJ		+= AmbaDdrc_DLL_Training_A64.o
else
COMMOBJ		+= AmbaDdrc_Read_Training_A64.o
endif
ifndef CONFIG_BUILD_QNX_IPL
COMMOBJ         += AmbaBLD_UserCallBack.o
endif

ifdef CONFIG_ATF_HAVE_BL2
COMMOBJ         += ../../../common/secure/AmbaDramTrain.o
endif

libbld_nand-objs    := ${COMMOBJ}
libbld_nand-objs    += AmbaBLD_NAND.o

ifdef CONFIG_ENABLE_SPINAND_BOOT
libbld_nand-objs    += AmbaDramTraining_SpiNAND.o
else
libbld_nand-objs    += AmbaDramTraining_NAND.o
endif

libbld_emmc-objs    := ${COMMOBJ}
libbld_emmc-objs    += AmbaBLD_eMMC.o AmbaDramTraining_eMMC.o

libbld_spinor-objs  := ${COMMOBJ}
libbld_spinor-objs  += AmbaBLD_SpiNOR.o AmbaDramTraining_SpiNOR.o

ccflags-y := \
        -I$(src)/inc \
        -I$(srctree)/bsp/$(AMBA_CHIP_ID) \
        -I$(srctree)/vendors/ambarella/inc \
        -I$(srctree)/vendors/ambarella/inc/io \
        -I$(srctree)/vendors/ambarella/inc/io/$(AMBA_CHIP_ID) \
        -I$(srctree)/soc/io/src/common/inc \
        -I$(srctree)/soc/io/src/common/inc/arm \
        -I$(srctree)/soc/io/src/common/inc/rtsl \
        -I$(srctree)/soc/io/src/common/inc/csl \
        -I$(srctree)/soc/io/src/common/inc/reg \
        -I$(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc \
        -I$(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc/rtsl \
        -I$(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc/csl \
        -I$(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc/reg \
        -I$(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc/reg/debugport \
        -I$(srctree)/bsp/$(AMBA_CHIP_ID)/$(AMBA_BSP_NAME)

asflags-y := \
        -I$(src)/inc \
        -I$(srctree)/vendors/ambarella/inc \
        -I$(srctree)/vendors/ambarella/inc/io \
        -I$(srctree)/vendors/ambarella/inc/io/$(AMBA_CHIP_ID) \
        -I$(srctree)/soc/io/src/common/inc \
        -I$(srctree)/soc/io/src/common/inc/arm \
        -I$(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc \
        -I$(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc/rtsl \
        -I$(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc/csl \
        -I$(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc/reg \
        -I$(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc/reg/debugport

install-files += libbld_nand.a
install-files += libbld_emmc.a
install-files += libbld_spinor.a

