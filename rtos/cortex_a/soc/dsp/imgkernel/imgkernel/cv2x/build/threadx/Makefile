obj-y += libdsp_imgkernel.a
IK_FOLDER := $(srctree)/soc/dsp/imgkernel/imgkernel/cv2x

include $(IK_FOLDER)/src/files.mk
include $(IK_FOLDER)/src/arch/files.mk
include $(IK_FOLDER)/src/context/files.mk
include $(IK_FOLDER)/src/executer/files.mk

libdsp_imgkernel-objs += $(IK_EI_OBJ)
libdsp_imgkernel-objs += $(IK_CI_ARCH_OBJ)
libdsp_imgkernel-objs += $(IK_CI_CTX_OBJ)
libdsp_imgkernel-objs += $(IK_CI_EXE_OBJ)
libdsp_imgkernel-objs += $(IK_CI_CFG_OBJ)

ccflags-y := -I$(srctree)/vendors/ambarella/inc
ccflags-y += -I$(srctree)/vendors/ambarella/inc/dsp
ccflags-y += -I$(srctree)/vendors/ambarella/inc/dsp/$(AMBA_CHIP_ID)
ccflags-y += -I$(srctree)/vendors/ambarella/inc/io
ccflags-y += -I$(srctree)/vendors/ambarella/inc/io/$(AMBA_CHIP_ID)
ccflags-y += -I$(srctree)/soc/dsp/visual/inc/LL/auto
ccflags-y += -I$(srctree)/soc/dsp/visual/inc/LL/auto/$(AMBA_CHIP_ID)
ccflags-y += -I$(srctree)/soc/dsp/visual/inc/HL/auto
ccflags-y += -I$(srctree)/soc/dsp/visual/inc/HL/auto/$(AMBA_CHIP_ID)
ccflags-y += -I$(srctree)/svc/comsvc/misc
ccflags-y += -I$(srctree)/svc/comsvc/print
ccflags-y += -I$(srctree)/soc/dsp/imgkernel/imgkernel/cv2x/inc
ccflags-y += -I$(srctree)/soc/dsp/imgkernel/imgkernel/cv2x/inc/arch
ccflags-y += -I$(srctree)/soc/dsp/imgkernel/imgkernel/cv2x/inc/context
ccflags-y += -I$(srctree)/soc/dsp/imgkernel/imgkernel/cv2x/inc/executer
ccflags-y += -I$(srctree)/soc/dsp/imgkernel/imgkernel/cv2x/inc/ikc

ccflags-y += -std=c99

install-files := libdsp_imgkernel.a
