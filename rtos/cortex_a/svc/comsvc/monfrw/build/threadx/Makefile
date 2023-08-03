#### ThreadX
ifeq ($(CONFIG_BUILD_COMSVC_MONFRW),y)

ifneq ($(OBJ_NAME),)
ifneq ($(wildcard $(srctree)/$(src)/$(preflix)monfrw/src),)
include $(srctree)/$(src)/$(preflix)monfrw/src/build/threadx/Makefile
endif
endif

ifeq ($(CONFIG_BUILD_MONFRW_GRAPH),y)
ifneq ($(OBJ_NAME),)
ifneq ($(wildcard $(srctree)/$(src)/$(preflix)monfrw/main),)
include $(srctree)/$(src)/$(preflix)monfrw/main/build/threadx/Makefile
endif
endif
endif

ifeq ($(OBJ_NAME),)
preflix = ../../
OBJ_NAME = libmonfrw
obj-y := $(OBJ_NAME).a
install-files += $(OBJ_NAME).a
build_monfrw = MONFRW
$(info $(build_monfrw))

# monfrw src
MON_FRW_SRC = $(preflix)src/

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

ifeq ($(CONFIG_SOC_CV5),y)
AMBA_CHIP_ID_DSP:=cv5
else
AMBA_CHIP_ID_DSP:=$(AMBA_CHIP_ID)
endif

ccflags-y += -I$(MON_FRW_INC)
ccflags-y += -I$(MON_FRW_CMD_INC)
ccflags-y += -I$(MON_FRW_CMD_TABLE)

# monfrw graph
MON_MAIN_SRC = $(preflix)main/

MON_MAIN_CMD = $(MON_MAIN_SRC)cmd/

MON_MAIN_INC = $(src)/$(MON_MAIN_SRC)inc
MON_MAIN_CMD_INC = $(src)/$(MON_MAIN_CMD)inc
MON_MAIN_CMD_TABLE = $(src)/$(MON_MAIN_CMD)table

COMSVC_PATH = $(srctree)/svc/comsvc
VENDOR_SSP_INC = $(srctree)/vendors/ambarella/inc

ifeq ($(wildcard $(srctree)/$(src)/$(MON_MAIN_SRC)$(AMBA_CHIP_ID)),)
MON_SOC_NAME = comsoc
else
MON_SOC_NAME = $(AMBA_CHIP_ID)
endif

$(OBJ_NAME)-objs += $(MON_MAIN_SRC)AmbaMonMain.o \
                    $(MON_MAIN_SRC)AmbaMonInt.o \
                    $(MON_MAIN_SRC)AmbaMonVin.o \
                    $(MON_MAIN_SRC)AmbaMonDsp.o \
                    $(MON_MAIN_SRC)AmbaMonVout.o \
                    $(MON_MAIN_SRC)AmbaMonWdog.o \
                    $(MON_MAIN_SRC)AmbaMonListenVin.o \
                    $(MON_MAIN_SRC)AmbaMonListenVout.o \
                    $(MON_MAIN_SRC)AmbaMonListenDsp.o \
                    $(MON_MAIN_SRC)AmbaMonListenAaa.o \
                    $(MON_MAIN_SRC)AmbaMonListenSerdes.o \
                    $(MON_MAIN_SRC)AmbaMonStateVin.o \
                    $(MON_MAIN_SRC)AmbaMonStateVout.o \
                    $(MON_MAIN_SRC)AmbaMonStateDsp.o

$(OBJ_NAME)-objs += $(MON_MAIN_SRC)$(MON_SOC_NAME)/AmbaMonMain_Platform.o \
                    $(MON_MAIN_SRC)$(MON_SOC_NAME)/AmbaMonVin_Platform.o \
                    $(MON_MAIN_SRC)$(MON_SOC_NAME)/AmbaMonInt_Platform.o

ccflags-y += -I$(MON_MAIN_INC) \
             -I$(MON_MAIN_INC)/$(MON_SOC_NAME)

$(OBJ_NAME)-objs += $(MON_MAIN_CMD)AmbaMonFrwCmdApp.o

ccflags-y += -I$(MON_MAIN_CMD_INC)    \
             -I$(MON_MAIN_CMD_TABLE)

ccflags-y += -I$(COMSVC_PATH)/misc \
             -I$(COMSVC_PATH)/shell/inc \
             -I$(COMSVC_PATH)/print

ccflags-y += -I$(VENDOR_SSP_INC) \
             -I$(VENDOR_SSP_INC)/dsp \
             -I$(VENDOR_SSP_INC)/dsp/$(AMBA_CHIP_ID_DSP) \
             -I$(VENDOR_SSP_INC)/io \
             -I$(VENDOR_SSP_INC)/io/$(AMBA_CHIP_ID) \
             -I$(VENDOR_SSP_INC)/perif

ccflags-y += -I$(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc/rtsl
endif

endif
