#### Linux
ifeq ($(CONFIG_BUILD_COMSVC_IMGFRW),y)

ifneq ($(OBJ_NAME),)
ifneq ($(wildcard $(srctree)/$(src)/$(preflix)imgfrw/src),)
include $(srctree)/$(src)/$(preflix)imgfrw/src/build/linux/Makefile
endif
endif

ifeq ($(CONFIG_BUILD_IMGFRW_AAA),y)
ifneq ($(OBJ_NAME),)
ifneq ($(wildcard $(srctree)/$(src)/$(preflix)imgfrw/main),)
include $(srctree)/$(src)/$(preflix)imgfrw/main/build/linux/Makefile
endif
endif
endif

ifeq ($(OBJ_NAME),)
preflix = ../../
OBJ_NAME = libimgfrw
obj-y := $(OBJ_NAME).a
install-files += $(OBJ_NAME).a
build_imgfrw = IMGFRW
$(info $(build_imgfrw))

# imgfrw src
IMG_FRW_SRC = $(preflix)src/

IMG_FRW_CMD = $(IMG_FRW_SRC)cmd/

IMG_FRW_INC = $(src)/$(IMG_FRW_SRC)inc
IMG_FRW_CMD_INC = $(src)/$(IMG_FRW_CMD)inc
IMG_FRW_CMD_TABLE = $(src)/$(IMG_FRW_CMD)table

COMSVC_PATH = $(srctree)/svc/comsvc
VENDOR_SSP_INC = $(srctree)/vendors/ambarella/inc

ifeq ($(wildcard $(srctree)/$(src)/$(IMG_FRW_SRC)$(AMBA_CHIP_ID)),)
IMG_FRW_SOC_NAME = comsoc
else
IMG_FRW_SOC_NAME = $(AMBA_CHIP_ID)
endif

$(OBJ_NAME)-objs += $(IMG_FRW_SRC)AmbaImgVar.o \
                    $(IMG_FRW_SRC)AmbaImgChannel.o \
                    $(IMG_FRW_SRC)AmbaImgEvent.o \
                    $(IMG_FRW_SRC)AmbaImgMessage.o \
                    $(IMG_FRW_SRC)AmbaImgStatistics.o \
                    $(IMG_FRW_SRC)AmbaImgSystem.o \
                    $(IMG_FRW_SRC)AmbaImgSensorDrv.o \
                    $(IMG_FRW_SRC)AmbaImgSensorHAL.o \
                    $(IMG_FRW_SRC)AmbaImgSensorSync.o \

$(OBJ_NAME)-objs += $(IMG_FRW_SRC)$(IMG_FRW_SOC_NAME)/AmbaImgSystem_Platform.o \
                    $(IMG_FRW_SRC)$(IMG_FRW_SOC_NAME)/AmbaImgSensorHAL_Platform.o \
                    $(IMG_FRW_SRC)$(IMG_FRW_SOC_NAME)/AmbaImgSensorSync_Platform.o

$(OBJ_NAME)-objs += $(IMG_FRW_CMD)AmbaImgFrwCmdMain.o \
                    $(IMG_FRW_CMD)AmbaImgFrwCmd.o \
                    $(IMG_FRW_CMD)AmbaImgFrwCmdVar.o \
                    $(IMG_FRW_CMD)AmbaImgFrwCmdSensor.o \
                    $(IMG_FRW_CMD)AmbaImgFrwCmdSystem.o

ccflags-y += -DSOC_NAME=$(AMBA_CHIP_ID)

ccflags-y += -I$(IMG_FRW_INC)
ccflags-y += -I$(IMG_FRW_INC)/$(IMG_FRW_SOC_NAME)
ccflags-y += -I$(IMG_FRW_CMD_INC)
ccflags-y += -I$(IMG_FRW_CMD_TABLE)

ccflags-y += -I$(COMSVC_PATH)/misc \
             -I$(COMSVC_PATH)/print \
             -I$(COMSVC_PATH)/shell/inc

ccflags-y += -I$(VENDOR_SSP_INC) \
             -I$(VENDOR_SSP_INC)/dsp \
             -I$(VENDOR_SSP_INC)/dsp/$(AMBA_CHIP_ID) \
             -I$(VENDOR_SSP_INC)/io \
             -I$(VENDOR_SSP_INC)/io/$(AMBA_CHIP_ID)

# imgfrw aaa
IMG_MAIN_SRC = $(preflix)main/

IMG_MAIN_CMD = $(IMG_MAIN_SRC)cmd/

IMG_MAIN_INC = $(src)/$(IMG_MAIN_SRC)inc
IMG_MAIN_CMD_INC = $(src)/$(IMG_MAIN_CMD)inc
IMG_MAIN_CMD_TABLE = $(src)/$(IMG_MAIN_CMD)table

COMSVC_PATH = $(srctree)/svc/comsvc
VENDOR_SSP_INC = $(srctree)/vendors/ambarella/inc

ifeq ($(wildcard $(srctree)/$(src)/$(IMG_MAIN_SRC)$(AMBA_CHIP_ID)),)
IMG_SRC_SOC_NAME = comsoc
else
IMG_SRC_SOC_NAME = $(AMBA_CHIP_ID)
endif

ifeq ($(wildcard $(srctree)/$(src)/$(IMG_MAIN_SRC)chan_table/$(AMBA_CHIP_ID)),)
IMG_CHAN_TABLE_SOC_NAME = comsoc
else
IMG_CHAN_TABLE_SOC_NAME = $(AMBA_CHIP_ID)
endif

$(OBJ_NAME)-objs += $(IMG_MAIN_SRC)AmbaImgSensorDrv_Chan.o      \
                    $(IMG_MAIN_SRC)AmbaImgMessage_Chan.o        \
                    $(IMG_MAIN_SRC)AmbaImgMainStatistics.o      \
                    $(IMG_MAIN_SRC)AmbaImgMainSync.o            \
                    $(IMG_MAIN_SRC)AmbaImgMainAe.o              \
                    $(IMG_MAIN_SRC)AmbaImgMainAwb.o             \
                    $(IMG_MAIN_SRC)AmbaImgMainAdj.o             \
                    $(IMG_MAIN_SRC)AmbaImgMainAf.o              \
                    $(IMG_MAIN_SRC)AmbaImgMain.o

$(OBJ_NAME)-objs += $(IMG_MAIN_SRC)$(IMG_SRC_SOC_NAME)/AmbaImgMainAe_Platform.o \
                    $(IMG_MAIN_SRC)$(IMG_SRC_SOC_NAME)/AmbaImgMainSync_Platform.o

$(OBJ_NAME)-objs += $(IMG_MAIN_SRC)chan_table/AmbaImgFramework_ChanTable.o
$(OBJ_NAME)-objs += $(IMG_MAIN_SRC)chan_table/$(IMG_CHAN_TABLE_SOC_NAME)/AmbaImgFramework_ChanTable_Platform.o

$(OBJ_NAME)-objs += $(IMG_MAIN_SRC)AmbaImgMainAvm.o
$(OBJ_NAME)-objs += $(IMG_MAIN_SRC)AmbaImgMainFov.o
$(OBJ_NAME)-objs += $(IMG_MAIN_SRC)AmbaImgMainEis.o

ifdef CONFIG_BUILD_IMGFRW_EFOV
$(OBJ_NAME)-objs += $(IMG_MAIN_SRC)AmbaImgMainEFov.o
endif

ccflags-y += -I$(IMG_MAIN_INC) \
             -I$(IMG_MAIN_INC)/$(IMG_SRC_SOC_NAME)

ccflags-y += -I$(COMSVC_PATH)/imgproc/imgaaa/inc/aaa \
             -I$(COMSVC_PATH)/imgproc/imgaaa/inc/iqparam

ccflags-y += -I$(COMSVC_PATH)/eis/inc \
             -I$(COMSVC_PATH)/imumanager/inc

ccflags-y += -I$(COMSVC_PATH)/af/inc

ccflags-y += -I$(COMSVC_PATH)/imgproc/inc/$(AMBA_CHIP_ID)

$(OBJ_NAME)-objs += $(IMG_MAIN_CMD)AmbaImgFrwCmdApp.o   \
                    $(IMG_MAIN_CMD)AmbaImgFrwCmdFlow.o

ccflags-y += -I$(IMG_MAIN_CMD_INC) \
             -I$(IMG_MAIN_CMD_TABLE)

ccflags-y += -I$(COMSVC_PATH)/misc \
             -I$(COMSVC_PATH)/shell/inc \
             -I$(COMSVC_PATH)/print

ccflags-y += -I$(VENDOR_SSP_INC) \
             -I$(VENDOR_SSP_INC)/dsp \
             -I$(VENDOR_SSP_INC)/dsp/$(AMBA_CHIP_ID) \
             -I$(VENDOR_SSP_INC)/io \
             -I$(VENDOR_SSP_INC)/io/$(AMBA_CHIP_ID) \
             -I$(VENDOR_SSP_INC)/perif
endif

endif
