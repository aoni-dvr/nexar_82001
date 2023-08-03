ifndef QCONFIG
QCONFIG=qconfig.mk
endif
include $(QCONFIG)

include $(objtree)/include/config/auto.conf

MON_FRW_SRC = $(PROJECT_ROOT)/../..
MON_FRW_CMD = $(MON_FRW_SRC)/cmd

MON_FRW_INC = $(MON_FRW_SRC)/inc
MON_FRW_CMD_INC = $(MON_FRW_CMD)/inc
MON_FRW_CMD_TABLE = $(MON_FRW_CMD)/table

EXTRA_SRCVPATH += $(MON_FRW_SRC)
EXTRA_SRCVPATH += $(MON_FRW_CMD)

SRCS := AmbaMonRing.c
SRCS += AmbaMonEvent.c
SRCS += AmbaMonMessage.c
SRCS += AmbaMonListen.c
SRCS += AmbaMonWatchdog.c
SRCS += AmbaMonVar.c

SRCS += AmbaMonFrwCmdMain.c
SRCS += AmbaMonFrwCmd.c
SRCS += AmbaMonFrwCmdVar.c

EXTRA_INCVPATH += $(MON_FRW_INC)
EXTRA_INCVPATH += $(MON_FRW_CMD_INC)
EXTRA_INCVPATH += $(MON_FRW_CMD_TABLE)

EXTRA_INCVPATH += $(srctree)/svc/comsvc/qnx/shell/public/svc/comsvc
EXTRA_INCVPATH += $(srctree)/svc/comsvc/shell/inc
EXTRA_INCVPATH += $(srctree)/svc/comsvc/qnx/print/public/svc/comsvc
EXTRA_INCVPATH += $(srctree)/svc/comsvc/print/

NAME=monfrw_src

include $(MKFILES_ROOT)/qtargets.mk
