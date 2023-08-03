obj-y += libdsp_imgkernel.a
IK_FOLDER := $(srctree)/soc/dsp/imgkernel/imgkernel/cv5x

include $(IK_FOLDER)/src/files.mk
include $(IK_FOLDER)/src/arch/files.mk
include $(IK_FOLDER)/src/context/files.mk
include $(IK_FOLDER)/src/config/files.mk
include $(IK_FOLDER)/src/executer/files.mk

libdsp_imgkernel-objs := $(IK_EI_OBJ)
libdsp_imgkernel-objs += $(IK_CI_ARCH_OBJ)
libdsp_imgkernel-objs += $(IK_CI_CTX_OBJ)
libdsp_imgkernel-objs += $(IK_CI_EXE_OBJ)
libdsp_imgkernel-objs += $(IK_CI_CFG_OBJ)

ccflags-y := -I$(srctree)/vendors/ambarella/inc
ccflags-y += -I$(srctree)/vendors/ambarella/inc/dsp
ccflags-y += -I$(srctree)/vendors/ambarella/inc/dsp/$(AMBA_CHIP_ID)
ccflags-y += -I$(srctree)/vendors/ambarella/inc/io
ccflags-y += -I$(srctree)/vendors/ambarella/inc/io/$(AMBA_CHIP_ID)
ccflags-y += -I$(srctree)/soc/dsp/visual/inc/LL
#ccflags-y += -I$(srctree)/soc/dsp/visual/inc/LL/$(AMBA_CHIP_ID)
ccflags-y += -I$(srctree)/soc/dsp/visual/inc/HL
#ccflags-y += -I$(srctree)/soc/dsp/visual/inc/HL/$(AMBA_CHIP_ID)
ccflags-y += -I$(srctree)/svc/comsvc/misc
ccflags-y += -I$(srctree)/svc/comsvc/print
ccflags-y += -I$(srctree)/soc/dsp/imgkernel/imgkernel/cv5x/src
ccflags-y += -I$(srctree)/soc/dsp/imgkernel/imgkernel/cv5x/inc
ccflags-y += -I$(srctree)/soc/dsp/imgkernel/imgkernel/cv5x/inc/common
ccflags-y += -I$(srctree)/soc/dsp/imgkernel/imgkernel/cv5x/inc/arch
ccflags-y += -I$(srctree)/soc/dsp/imgkernel/imgkernel/cv5x/inc/config
ccflags-y += -I$(srctree)/soc/dsp/imgkernel/imgkernel/cv5x/inc/context
ccflags-y += -I$(srctree)/soc/dsp/imgkernel/imgkernel/cv5x/inc/executer
ccflags-y += -I$(srctree)/soc/dsp/imgkernel/imgkernel/cv5x/inc/ikc
ccflags-y += -I$(srctree)/soc/dsp/imgkernel/imgkernel/cv5x/inc/iks
ccflags-y += -std=c99

install-files := libdsp_imgkernel.a
