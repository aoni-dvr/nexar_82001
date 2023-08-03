ifndef QCONFIG
QCONFIG=qconfig.mk
endif
include $(QCONFIG)

include $(objtree)/include/config/auto.conf

MON_MAIN_SRC = $(PROJECT_ROOT)/../..
MON_MAIN_CMD = $(MON_MAIN_SRC)/cmd

MON_MAIN_INC = $(MON_MAIN_SRC)/inc
MON_MAIN_CMD_INC = $(MON_MAIN_CMD)/inc
MON_MAIN_CMD_TABLE = $(MON_MAIN_CMD)/table

MON_FRW_SRC = $(PROJECT_ROOT)/../../../src
MON_FRW_CMD = $(MON_FRW_SRC)/cmd

MON_FRW_INC = $(MON_FRW_SRC)/inc
MON_FRW_CMD_INC = $(MON_FRW_CMD)/inc

ifeq ($(wildcard $(AMBA_CHIP_ID)),)
MON_MAIN_SOC_NAME = comsoc
else
MON_MAIN_SOC_NAME = $(AMBA_CHIP_ID)
endif

EXTRA_SRCVPATH += $(MON_MAIN_SRC)
EXTRA_SRCVPATH += $(MON_MAIN_SRC)/$(MON_MAIN_SOC_NAME)
EXTRA_SRCVPATH += $(MON_MAIN_CMD)

SRCS := AmbaMonMain.c
SRCS += AmbaMonInt.c
SRCS += AmbaMonVin.c
SRCS += AmbaMonDsp.c
SRCS += AmbaMonVout.c
SRCS += AmbaMonWdog.c
SRCS += AmbaMonListenVin.c
SRCS += AmbaMonListenVout.c
SRCS += AmbaMonListenDsp.c
SRCS += AmbaMonListenAaa.c
SRCS += AmbaMonListenSerdes.c
SRCS += AmbaMonStateVin.c
SRCS += AmbaMonStateVout.c
SRCS += AmbaMonStateDsp.c

SRCS += AmbaMonMain_Platform.c
SRCS += AmbaMonVin_Platform.c
SRCS += AmbaMonInt_Platform.c

SRCS += AmbaMonFrwCmdApp.c

EXTRA_INCVPATH += $(MON_MAIN_INC)
EXTRA_INCVPATH += $(MON_MAIN_INC)/$(MON_MAIN_SOC_NAME)
EXTRA_INCVPATH += $(MON_MAIN_CMD_INC)
EXTRA_INCVPATH += $(MON_MAIN_CMD_TABLE)

EXTRA_INCVPATH += $(MON_FRW_INC)
EXTRA_INCVPATH += $(MON_FRW_CMD_INC)

EXTRA_INCVPATH += $(srctree)/vendors/ambarella/inc
EXTRA_INCVPATH += $(srctree)/vendors/ambarella/inc/dsp
EXTRA_INCVPATH += $(srctree)/vendors/ambarella/inc/dsp/$(AMBA_CHIP_ID)
EXTRA_INCVPATH += $(srctree)/vendors/ambarella/inc/io
EXTRA_INCVPATH += $(srctree)/vendors/ambarella/inc/io/$(AMBA_CHIP_ID)
EXTRA_INCVPATH += $(srctree)/vendors/ambarella/inc/perif

EXTRA_INCVPATH += $(srctree)/svc/comsvc/misc

EXTRA_INCVPATH += $(srctree)/svc/comsvc/qnx/shell/public/svc/comsvc
EXTRA_INCVPATH += $(srctree)/svc/comsvc/shell/inc
EXTRA_INCVPATH += $(srctree)/svc/comsvc/qnx/print/public/svc/comsvc
EXTRA_INCVPATH += $(srctree)/svc/comsvc/print/

NAME=monfrw_graph

include $(MKFILES_ROOT)/qtargets.mk
