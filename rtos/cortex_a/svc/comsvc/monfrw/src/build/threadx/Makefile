#### ThreadX
ifneq ($(OBJ_NAME),)
MON_FRW_SRC = $(preflix)monfrw/src/
else
preflix = ../../
OBJ_NAME = libmonfrw_src
obj-y := $(OBJ_NAME).a
install-files += $(OBJ_NAME).a
MON_FRW_SRC = $(preflix)
endif

MON_FRW_CMD = $(MON_FRW_SRC)cmd/

MON_FRW_INC = $(src)/$(MON_FRW_SRC)inc
MON_FRW_CMD_INC = $(src)/$(MON_FRW_CMD)inc
MON_FRW_CMD_TABLE = $(src)/$(MON_FRW_CMD)table

COMSVC_PATH = $(srctree)/svc/comsvc
VENDOR_SSP_INC = $(srctree)/vendors/ambarella/inc

$(OBJ_NAME)-objs += $(MON_FRW_SRC)AmbaMonRing.o \
                    $(MON_FRW_SRC)AmbaMonEvent.o \
                    $(MON_FRW_SRC)AmbaMonMessage.o \
                    $(MON_FRW_SRC)AmbaMonListen.o \
                    $(MON_FRW_SRC)AmbaMonWatchdog.o \
                    $(MON_FRW_SRC)AmbaMonVar.o

$(OBJ_NAME)-objs += $(MON_FRW_CMD)AmbaMonFrwCmdMain.o \
                    $(MON_FRW_CMD)AmbaMonFrwCmd.o \
                    $(MON_FRW_CMD)AmbaMonFrwCmdVar.o

ccflags-y += -I$(MON_FRW_INC)
ccflags-y += -I$(MON_FRW_CMD_INC)
ccflags-y += -I$(MON_FRW_CMD_TABLE)

ccflags-y += -I$(COMSVC_PATH)/misc \
             -I$(COMSVC_PATH)/print \
             -I$(COMSVC_PATH)/shell/inc

ccflags-y += -I$(VENDOR_SSP_INC) \
             -I$(VENDOR_SSP_INC)/dsp \
             -I$(VENDOR_SSP_INC)/dsp/$(AMBA_CHIP_ID) \
             -I$(VENDOR_SSP_INC)/io \
             -I$(VENDOR_SSP_INC)/io/$(AMBA_CHIP_ID)
