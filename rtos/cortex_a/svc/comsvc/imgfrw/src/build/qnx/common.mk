ifndef QCONFIG
QCONFIG=qconfig.mk
endif
include $(QCONFIG)

include $(objtree)/include/config/auto.conf

# Compiling options
#CCFLAGS += -O0 \
#	       -Winline \
#	       -fomit-frame-pointer -fno-strength-reduce \
#	       -D_FILE_OFFSET_BITS=64

IMG_FRW_SRC = $(PROJECT_ROOT)/../..
IMG_FRW_CMD = $(IMG_FRW_SRC)/cmd

IMG_FRW_INC = $(IMG_FRW_SRC)/inc
IMG_FRW_CMD_INC = $(IMG_FRW_CMD)/inc
IMG_FRW_CMD_TABLE = $(IMG_FRW_CMD)/table

#$(info $(srctree))
#$(info $(PROJECT_ROOT))
#$(info $(CURDIR))
#$(info $(IMG_FRW_SRC))

#$(info $(CURDIR)/../..$(IMG_FRW_SRC)/$(AMBA_CHIP_ID))

ifeq ($(wildcard $(CURDIR)/../..$(IMG_FRW_SRC)/$(AMBA_CHIP_ID)),)
IMG_SRC_SOC_NAME = comsoc
else
IMG_SRC_SOC_NAME = $(AMBA_CHIP_ID)
endif

#$(info $(IMG_SRC_SOC_NAME))

EXTRA_SRCVPATH += $(IMG_FRW_SRC)
EXTRA_SRCVPATH += $(IMG_FRW_SRC)/$(IMG_SRC_SOC_NAME)
EXTRA_SRCVPATH += $(IMG_FRW_CMD)

SRCS := AmbaImgVar.c
#SRCS += AmbaImgMem.c
SRCS += AmbaImgChannel.c
SRCS += AmbaImgEvent.c
SRCS += AmbaImgMessage.c
SRCS += AmbaImgStatistics.c
SRCS += AmbaImgSystem.c
SRCS += AmbaImgSensorDrv.c
SRCS += AmbaImgSensorHAL.c
SRCS += AmbaImgSensorSync.c

SRCS += AmbaImgSystem_Platform.c
SRCS += AmbaImgSensorHAL_Platform.c
SRCS += AmbaImgSensorSync_Platform.c

SRCS += AmbaImgFrwCmdMain.c
SRCS += AmbaImgFrwCmd.c
SRCS += AmbaImgFrwCmdVar.c
SRCS += AmbaImgFrwCmdSensor.c
SRCS += AmbaImgFrwCmdSystem.c

EXTRA_INCVPATH += $(IMG_FRW_INC)
EXTRA_INCVPATH += $(IMG_FRW_INC)/$(IMG_SRC_SOC_NAME)
EXTRA_INCVPATH += $(IMG_FRW_CMD_INC)
EXTRA_INCVPATH += $(IMG_FRW_CMD_TABLE)

EXTRA_INCVPATH += $(srctree)/vendors/ambarella/inc
EXTRA_INCVPATH += $(srctree)/vendors/ambarella/inc/dsp
EXTRA_INCVPATH += $(srctree)/vendors/ambarella/inc/dsp/$(AMBA_CHIP_ID)
EXTRA_INCVPATH += $(srctree)/vendors/ambarella/inc/io
EXTRA_INCVPATH += $(srctree)/vendors/ambarella/inc/io/$(AMBA_CHIP_ID)

EXTRA_INCVPATH += $(srctree)/svc/comsvc/qnx/shell/public/svc/comsvc
EXTRA_INCVPATH += $(srctree)/svc/comsvc/shell/inc
EXTRA_INCVPATH += $(srctree)/svc/comsvc/qnx/print/public/svc/comsvc
EXTRA_INCVPATH += $(srctree)/svc/comsvc/print/

NAME=imgfrw_src

include $(MKFILES_ROOT)/qtargets.mk
