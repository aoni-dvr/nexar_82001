
#KBUILD_CFLAGS := $(subst -O1,-O0,$(KBUILD_CFLAGS))
#KBUILD_CFLAGS := $(subst -O2,-O0,$(KBUILD_CFLAGS))

KBUILD_CFLAGS += -mgeneral-regs-only -mstrict-align
KBUILD_AFLAGS += -mgeneral-regs-only -mstrict-align

KBUILD_CFLAGS := $(subst -fPIE ,,$(KBUILD_CFLAGS))
KBUILD_CFLAGS += -no-pie

obj-y += libbld_nand.a
obj-y += libbld_emmc.a
obj-y += libbld_spinor.a

COMMOBJ		:= AmbaBLD_Boot_A64.o AmbaBLD_HwStack_A64.o AmbaBLD_Shell.o AmbaDramTraining.o AmbaDramTrainingFunc.o AmbaDramTrainingLog.o AmbaDramTrainingAssembly.o
COMMOBJ		+= AmbaBLD_MmuMap.o AmbaBLD_PsciOnEntry_A64.o AmbaBLD_handler.o AmbaBLD_BootCore1.o
COMMOBJ		+= sh/jump.o sh/armv8_helper.o sh/armcache.o
COMMOBJ		+= AmbaBLD_ChipVersion.o
ifndef CONFIG_BUILD_QNX_IPL
COMMOBJ         += AmbaBLD_UserCallBack.o
endif

ifdef CONFIG_ATF_HAVE_BL2
COMMOBJ         += ../../../common/secure/AmbaDramTrain.o
endif

libbld_nand-objs    := ${COMMOBJ}
libbld_nand-objs    += AmbaBLD_NAND.o

libbld_emmc-objs    := ${COMMOBJ}
libbld_emmc-objs    += AmbaBLD_eMMC.o

libbld_spinor-objs  := ${COMMOBJ}
libbld_spinor-objs  += AmbaBLD_SpiNOR.o

$(foreach o,$(libbld_nand-objs) $(libbld_emmc-objs) $(libbld_spinor-objs),$(obj)/$(o)): $(objtree)/$(obj)/AmbaBLD_cfg.h

$(objtree)/$(obj)/AmbaBLD_cfg.h: $(srctree)/$(src)/AmbaBLD_cfg.h.in $(srctree)/$(src)/gen_AmbaBLD_cfg.h.sh
	$(srctree)/$(src)/gen_AmbaBLD_cfg.h.sh $< $@

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

asflags-y := \
        -I$(src)/inc \
        -I$(srctree)/vendors/ambarella/inc \
        -I$(srctree)/vendors/ambarella/inc/io \
        -I$(srctree)/soc/io/src/common/inc \
        -I$(srctree)/soc/io/src/common/inc/arm \
        -I$(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc \
        -I$(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc/rtsl \
        -I$(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc/csl \
        -I$(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc/reg \
        -I$(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc/reg/debugport

asflags-y += -I$(srctree)/$(src)/sh/include

install-files += libbld_nand.a
install-files += libbld_emmc.a
install-files += libbld_spinor.a

