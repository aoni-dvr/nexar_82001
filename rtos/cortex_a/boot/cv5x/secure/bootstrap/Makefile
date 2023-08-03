#obj-y += libbst_ddr4_nand.a
#obj-y += libbst_ddr4_emmc.a
#obj-y += libbst_ddr4_spinor.a
#obj-y += libbst_ddr4_spinand.a
#
obj-y += libbst_lpddr4_nand.a
obj-y += libbst_lpddr4_emmc.a
obj-y += libbst_lpddr4_spinor.a
obj-y += libbst_lpddr4_spinand.a

obj-y += libbst_lpddr5_nand.a
obj-y += libbst_lpddr5_emmc.a
obj-y += libbst_lpddr5_spinor.a
obj-y += libbst_lpddr5_spinand.a

libbst_common-objs := AmbaBST.o AmbaBST_LoadPref.o auth.o utils.o
libbst_common-objs += ../../../common/secure/AmbaBST_SecureBoot.o
libbst_common-objs += ../../../common/secure/crc32.o
ifdef CONFIG_BST_LARGE_SIZE
KBUILD_CFLAGS += -mgeneral-regs-only -Os -fno-builtin
KBUILD_CFLAGS := $(subst -ffunction-sections ,,$(KBUILD_CFLAGS))
KBUILD_CFLAGS := $(subst -fdata-sections ,,$(KBUILD_CFLAGS))
KBUILD_CFLAGS := $(subst -fno-inline-functions-called-once ,,$(KBUILD_CFLAGS))
KBUILD_CFLAGS := $(subst -fPIE ,,$(KBUILD_CFLAGS))
KBUILD_CFLAGS += -no-pie
libbst_common-objs += uartp.o
ifdef CONFIG_ENABLE_SECURITY
ifeq ($(CONFIG_SOC_CV5)$(CONFIG_SOC_CV52),y)
libbst_common-objs += back_2nd_stage.o
endif
endif
ifdef CONFIG_BST_WDELAY_UPDATE_DRIFT
libbst_common-objs += dram_adjust_lpddr5.o
endif
ifdef CONFIG_BST_DRAM_TRAINING
ifdef CONFIG_DRAM_TYPE_LPDDR5
libbst_common-objs += dram_training_lpddr5.o
else
libbst_common-objs += dram_training_lpddr4.o
endif
endif
endif

libbst_lpddr4_nand-objs    := $(libbst_common-objs) AmbaBST_InitLPDDR4.o AmbaBST_NAND.o
libbst_lpddr4_emmc-objs    := $(libbst_common-objs) AmbaBST_InitLPDDR4.o AmbaBST_eMMC.o
libbst_lpddr4_spinor-objs  := $(libbst_common-objs) AmbaBST_InitLPDDR4.o AmbaBST_SpiNOR.o
libbst_lpddr4_spinand-objs := $(libbst_common-objs) AmbaBST_InitLPDDR4.o AmbaBST_SpiNAND.o

libbst_lpddr5_nand-objs    := $(libbst_common-objs) AmbaBST_InitLPDDR5.o AmbaBST_NAND.o
libbst_lpddr5_emmc-objs    := $(libbst_common-objs) AmbaBST_InitLPDDR5.o AmbaBST_eMMC.o
libbst_lpddr5_spinor-objs  := $(libbst_common-objs) AmbaBST_InitLPDDR5.o AmbaBST_SpiNOR.o
libbst_lpddr5_spinand-objs := $(libbst_common-objs) AmbaBST_InitLPDDR5.o AmbaBST_SpiNAND.o

ifdef CONFIG_BST_LARGE_SIZE
libbst_lpddr4_nand-objs    += sram_fifo_nand.o
libbst_lpddr4_emmc-objs    += sram_fifo_emmc.o
libbst_lpddr4_spinor-objs  += sram_fifo_nor.o
libbst_lpddr4_spinand-objs += sram_fifo_nand.o

libbst_lpddr5_nand-objs    += sram_fifo_nand.o
libbst_lpddr5_emmc-objs    += sram_fifo_emmc.o
libbst_lpddr5_spinor-objs  += sram_fifo_nor.o
libbst_lpddr5_spinand-objs += sram_fifo_nand.o
endif

ccflags-y := \
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

install-files += libbst_lpddr4_nand.a
install-files += libbst_lpddr4_emmc.a
install-files += libbst_lpddr4_spinor.a
install-files += libbst_lpddr4_spinand.a

install-files += libbst_lpddr5_nand.a
install-files += libbst_lpddr5_emmc.a
install-files += libbst_lpddr5_spinor.a
install-files += libbst_lpddr5_spinand.a
