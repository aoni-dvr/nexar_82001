ifndef QCONFIG
QCONFIG=qconfig.mk
endif
include $(QCONFIG)

include $(objtree)/include/config/auto.conf

IMG_AAA_SRC = $(PROJECT_ROOT)/../../src/aaa
IMG_IQ_SRC = $(PROJECT_ROOT)/../../src/iqparam_handler
IMG_WBCALIB_SRC = $(PROJECT_ROOT)/../../src/wbcalib

IMG_PROC_INC = $(PROJECT_ROOT)/../../../inc/$(AMBA_CHIP_ID)
IMG_AAA_INC = $(PROJECT_ROOT)/../../inc/aaa
IMG_IQ_INC = $(PROJECT_ROOT)/../../inc/iqparam
IMG_WBCALIB_INC = $(PROJECT_ROOT)/../../inc/wbcalib

EXTRA_SRCVPATH += $(IMG_AAA_SRC)
EXTRA_SRCVPATH += $(IMG_IQ_SRC)
EXTRA_SRCVPATH += $(IMG_WBCALIB_SRC)

# aaa
SRCS := Amba_AdjCtrl.c
SRCS += Amba_AeAwbAdj_Control.c
SRCS += Amba_Image.c
SRCS += Amba_ImgProcTest.c
SRCS += Amba_AdjTableSelectSample.c
SRCS += Amba_ExifImg.c

# iq handler
SRCS += AmbaIQParamHandlerSample.c

# wb calib
SRCS += Amba_AwbCalib.c
SRCS += Amba_AwbCalibTskSample.c

EXTRA_INCVPATH += $(IMG_PROC_INC)
EXTRA_INCVPATH += $(IMG_AAA_INC)
EXTRA_INCVPATH += $(IMG_IQ_INC)
EXTRA_INCVPATH += $(IMG_WBCALIB_INC)

EXTRA_INCVPATH += $(srctree)/svc/comsvc/misc

EXTRA_INCVPATH += $(srctree)/vendors/ambarella/inc
EXTRA_INCVPATH += $(srctree)/vendors/ambarella/inc/dsp
EXTRA_INCVPATH += $(srctree)/vendors/ambarella/inc/dsp/$(AMBA_CHIP_ID)
EXTRA_INCVPATH += $(srctree)/vendors/ambarella/inc/io
EXTRA_INCVPATH += $(srctree)/vendors/ambarella/inc/io/$(AMBA_CHIP_ID)
EXTRA_INCVPATH += $(srctree)/vendors/ambarella/inc/perif

EXTRA_INCVPATH += $(srctree)/svc/comsvc/qnx/shell/public/svc/comsvc
EXTRA_INCVPATH += $(srctree)/svc/comsvc/shell/inc
EXTRA_INCVPATH += $(srctree)/svc/comsvc/qnx/print/public/svc/comsvc
EXTRA_INCVPATH += $(srctree)/svc/comsvc/print/

ifeq ($(CONFIG_BUILD_COMMON_SERVICE_AF),y)
EXTRA_INCVPATH += $(srctree)/svc/comsvc/af/inc/
endif

NAME=imgaaa

include $(MKFILES_ROOT)/qtargets.mk
