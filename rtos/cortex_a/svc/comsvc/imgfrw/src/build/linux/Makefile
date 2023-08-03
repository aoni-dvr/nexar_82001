#### Linux
ifneq ($(OBJ_NAME),)
IMG_FRW_SRC = $(preflix)imgfrw/src/
else
preflix = ../../
OBJ_NAME = libimgfrw_src
obj-y := $(OBJ_NAME).a
install-files += $(OBJ_NAME).a
IMG_FRW_SRC = $(preflix)
build_imgfrw = IMGFRW_SRC
$(info $(build_imgfrw))
endif

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
