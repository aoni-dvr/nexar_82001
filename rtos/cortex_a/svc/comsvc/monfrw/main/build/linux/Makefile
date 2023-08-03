#### Linux
ifneq ($(OBJ_NAME),)
MON_MAIN_SRC = $(preflix)monfrw/main/
MON_FRW_SRC = $(preflix)monfrw/src/
ccflags-y += -DAMBA_MON_FRW_LIB_COMSVC
else
preflix = ../../
OBJ_NAME = libmonfrw_graph
obj-y := $(OBJ_NAME).a
install-files += $(OBJ_NAME).a
MON_MAIN_SRC = $(preflix)
MON_FRW_SRC = $(preflix)../src/
build_monfrw = MONFRW_GRAPH
$(info $(build_monfrw))
endif

MON_MAIN_CMD = $(MON_MAIN_SRC)cmd/

MON_MAIN_INC = $(src)/$(MON_MAIN_SRC)inc
MON_MAIN_CMD_INC = $(src)/$(MON_MAIN_CMD)inc
MON_MAIN_CMD_TABLE = $(src)/$(MON_MAIN_CMD)table

MON_FRW_CMD = $(MON_FRW_SRC)cmd/

MON_FRW_INC = $(src)/$(MON_FRW_SRC)inc
MON_FRW_CMD_INC = $(src)/$(MON_FRW_CMD)inc

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

ccflags-y += -I$(MON_FRW_INC) \
             -I$(MON_FRW_CMD_INC)

ccflags-y += -I$(COMSVC_PATH)/misc \
             -I$(COMSVC_PATH)/shell/inc \
             -I$(COMSVC_PATH)/print

ccflags-y += -I$(VENDOR_SSP_INC) \
             -I$(VENDOR_SSP_INC)/dsp \
             -I$(VENDOR_SSP_INC)/dsp/$(AMBA_CHIP_ID) \
             -I$(VENDOR_SSP_INC)/io \
             -I$(VENDOR_SSP_INC)/io/$(AMBA_CHIP_ID) \
             -I$(VENDOR_SSP_INC)/perif

ccflags-y += -I$(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc/rtsl
