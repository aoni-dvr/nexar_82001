obj-y += libbst_ddr4_nand.a
obj-y += libbst_ddr4_emmc.a
obj-y += libbst_ddr4_spinor.a
obj-y += libbst_ddr4_spinand.a

obj-y += libbst_lpddr4_nand.a
obj-y += libbst_lpddr4_emmc.a
obj-y += libbst_lpddr4_spinor.a
obj-y += libbst_lpddr4_spinand.a

libbst_common-objs := AmbaBST.o rsa.o
libbst_common-objs += ../../../common/secure/AmbaBST_SecureBoot.o
libbst_common-objs += ../../../common/secure/crc32.o
ifdef CONFIG_BST_DRAM_TRAINING
libbst_common-objs += dram_training_lpddr4.o
endif

libbst_ddr4_nand-objs      := $(libbst_common-objs) AmbaBST_InitDDR4.o AmbaBST_NAND.o
libbst_ddr4_emmc-objs      := $(libbst_common-objs) AmbaBST_InitDDR4.o AmbaBST_eMMC.o
libbst_ddr4_spinor-objs    := $(libbst_common-objs) AmbaBST_InitDDR4.o AmbaBST_SpiNOR.o
libbst_ddr4_spinand-objs   := $(libbst_common-objs) AmbaBST_InitDDR4.o AmbaBST_SpiNAND.o

libbst_lpddr4_nand-objs    := $(libbst_common-objs) AmbaBST_InitLPDDR4.o AmbaBST_NAND.o
libbst_lpddr4_emmc-objs    := $(libbst_common-objs) AmbaBST_InitLPDDR4.o AmbaBST_eMMC.o
libbst_lpddr4_spinor-objs  := $(libbst_common-objs) AmbaBST_InitLPDDR4.o AmbaBST_SpiNOR.o
libbst_lpddr4_spinand-objs := $(libbst_common-objs) AmbaBST_InitLPDDR4.o AmbaBST_SpiNAND.o

ccflags-y := \
        -I$(src)/inc \
        -I$(srctree)/vendors/ambarella/inc

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

install-files += libbst_ddr4_nand.a
install-files += libbst_ddr4_emmc.a
install-files += libbst_ddr4_spinor.a
install-files += libbst_ddr4_spinand.a

install-files += libbst_lpddr4_nand.a
install-files += libbst_lpddr4_emmc.a
install-files += libbst_lpddr4_spinor.a
install-files += libbst_lpddr4_spinand.a
