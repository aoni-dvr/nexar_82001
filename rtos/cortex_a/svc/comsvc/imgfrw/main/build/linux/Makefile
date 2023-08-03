#### Linux
ifneq ($(OBJ_NAME),)
IMG_MAIN_SRC = $(preflix)imgfrw/main/
IMG_FRW_SRC = $(preflix)imgfrw/src/
ccflags-y += -DAMBA_IMG_FRW_LIB_COMSVC
else
preflix = ../../
OBJ_NAME = libimgfrw_aaa
obj-y := $(OBJ_NAME).a
install-files += $(OBJ_NAME).a
IMG_MAIN_SRC = $(preflix)
IMG_FRW_SRC = $(preflix)../src/
build_imgfrw = IMGFRW_AAA
$(info $(build_imgfrw))
endif

IMG_MAIN_CMD = $(IMG_MAIN_SRC)cmd/

IMG_MAIN_INC = $(src)/$(IMG_MAIN_SRC)inc
IMG_MAIN_CMD_INC = $(src)/$(IMG_MAIN_CMD)inc
IMG_MAIN_CMD_TABLE = $(src)/$(IMG_MAIN_CMD)table

IMG_FRW_CMD = $(IMG_FRW_SRC)cmd/

IMG_FRW_INC = $(src)/$(IMG_FRW_SRC)inc
IMG_FRW_CMD_INC = $(src)/$(IMG_FRW_CMD)inc

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

ccflags-y += -I$(IMG_FRW_INC) \
             -I$(IMG_FRW_CMD_INC)

ccflags-y += -I$(COMSVC_PATH)/misc \
             -I$(COMSVC_PATH)/shell/inc \
             -I$(COMSVC_PATH)/print

ccflags-y += -I$(VENDOR_SSP_INC) \
             -I$(VENDOR_SSP_INC)/dsp \
             -I$(VENDOR_SSP_INC)/dsp/$(AMBA_CHIP_ID) \
             -I$(VENDOR_SSP_INC)/io \
             -I$(VENDOR_SSP_INC)/io/$(AMBA_CHIP_ID) \
             -I$(VENDOR_SSP_INC)/perif
