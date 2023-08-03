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

IMG_MAIN_SRC = $(PROJECT_ROOT)/../..
IMG_MAIN_TABLE = $(IMG_MAIN_SRC)/chan_table
IMG_MAIN_CMD = $(IMG_MAIN_SRC)/cmd

IMG_MAIN_INC = $(IMG_MAIN_SRC)/inc
IMG_MAIN_CMD_INC = $(IMG_MAIN_CMD)/inc
IMG_MAIN_CMD_TABLE = $(IMG_MAIN_CMD)/table

IMG_FRW_SRC = $(PROJECT_ROOT)/../../../src
IMG_FRW_CMD = $(IMG_FRW_SRC)/cmd

IMG_FRW_INC = $(IMG_FRW_SRC)/inc
IMG_FRW_CMD_INC = $(IMG_FRW_CMD)/inc

#$(info $(srctree))
#$(info $(PROJECT_ROOT))
#$(info $(CURDIR))
#$(info $(IMG_MAIN_SRC))

#$(info $(CURDIR)/../..$(IMG_MAIN_SRC)/$(AMBA_CHIP_ID))

ifeq ($(wildcard $(CURDIR)/../..$(IMG_MAIN_SRC)/$(AMBA_CHIP_ID)),)
IMG_MAIN_SOC_NAME = comsoc
else
IMG_MAIN_SOC_NAME = $(AMBA_CHIP_ID)
endif

ifeq ($(wildcard $(CURDIR)/../..$(IMG_MAIN_SRC)/chan_table/$(AMBA_CHIP_ID)),)
IMG_CHAN_TABLE_SOC_NAME = comsoc
else
IMG_CHAN_TABLE_SOC_NAME = $(AMBA_CHIP_ID)
endif

#$(info $(IMG_MAIN_SOC_NAME))
#$(info $(IMG_CHAN_TABLE_SOC_NAME))

EXTRA_SRCVPATH += $(IMG_MAIN_SRC)
EXTRA_SRCVPATH += $(IMG_MAIN_SRC)/$(IMG_MAIN_SOC_NAME)
EXTRA_SRCVPATH += $(IMG_MAIN_TABLE)
EXTRA_SRCVPATH += $(IMG_MAIN_TABLE)/$(IMG_CHAN_TABLE_SOC_NAME)
EXTRA_SRCVPATH += $(IMG_MAIN_CMD)

SRCS := AmbaImgSensorDrv_Chan.c
SRCS += AmbaImgMessage_Chan.c
SRCS += AmbaImgMainStatistics.c
SRCS += AmbaImgMainSync.c
SRCS += AmbaImgMainAe.c
SRCS += AmbaImgMainAwb.c
SRCS += AmbaImgMainAdj.c
SRCS += AmbaImgMainAf.c
SRCS += AmbaImgMain.c

SRCS += AmbaImgMainAe_Platform.c
SRCS += AmbaImgMainSync_Platform.c

SRCS += AmbaImgFramework_ChanTable.c
SRCS += AmbaImgFramework_ChanTable_Platform.c

SRCS += AmbaImgMainAvm.c
SRCS += AmbaImgMainFov.c
SRCS += AmbaImgMainEis.c

ifdef CONFIG_BUILD_IMGFRW_EFOV
SRCS += AmbaImgMainEFov.c
endif

SRCS += AmbaImgFrwCmdApp.c
SRCS += AmbaImgFrwCmdFlow.c

EXTRA_INCVPATH += $(IMG_MAIN_INC)
EXTRA_INCVPATH += $(IMG_MAIN_INC)/$(IMG_MAIN_SOC_NAME)
EXTRA_INCVPATH += $(IMG_MAIN_CMD_INC)
EXTRA_INCVPATH += $(IMG_MAIN_CMD_TABLE)

EXTRA_INCVPATH += $(IMG_FRW_INC)
EXTRA_INCVPATH += $(IMG_FRW_CMD_INC)

EXTRA_INCVPATH += $(srctree)/vendors/ambarella/inc
EXTRA_INCVPATH += $(srctree)/vendors/ambarella/inc/dsp
EXTRA_INCVPATH += $(srctree)/vendors/ambarella/inc/dsp/$(AMBA_CHIP_ID)
EXTRA_INCVPATH += $(srctree)/vendors/ambarella/inc/io
EXTRA_INCVPATH += $(srctree)/vendors/ambarella/inc/io/$(AMBA_CHIP_ID)
EXTRA_INCVPATH += $(srctree)/vendors/ambarella/inc/perif

EXTRA_INCVPATH += $(srctree)/svc/comsvc/misc
EXTRA_INCVPATH += $(srctree)/svc/comsvc/imgproc/inc/$(AMBA_CHIP_ID)
EXTRA_INCVPATH += $(srctree)/svc/comsvc/imgproc/imgaaa/inc/aaa
EXTRA_INCVPATH += $(srctree)/svc/comsvc/imgproc/imgaaa/inc/iqparam

EXTRA_INCVPATH += $(srctree)/svc/comsvc/eis/inc
EXTRA_INCVPATH += $(srctree)/svc/comsvc/imumanager/inc

EXTRA_INCVPATH += $(srctree)/svc/comsvc/af/inc

EXTRA_INCVPATH += $(srctree)/svc/comsvc/qnx/shell/public/svc/comsvc
EXTRA_INCVPATH += $(srctree)/svc/comsvc/shell/inc
EXTRA_INCVPATH += $(srctree)/svc/comsvc/qnx/print/public/svc/comsvc
EXTRA_INCVPATH += $(srctree)/svc/comsvc/print/

NAME=imgfrw_aaa

include $(MKFILES_ROOT)/qtargets.mk
