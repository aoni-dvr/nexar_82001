################################################################
#                             QNX                              #
################################################################
include $(objtree)/include/config/auto.conf

VIS_WRAP_OS_PATH:=qnx
export VIS_WRAP_OS_PATH

VIS_WRAP_MOD_ROOT := $(CURDIR)/../../
export VIS_WRAP_MOD_ROOT

EXTRA_SRCVPATH += $(VIS_WRAP_MOD_ROOT)/src/$(VIS_WRAP_OS_PATH)

# src/common
SRCS := dsp_api.c
SRCS += dsp_wrapper_osal.c

EXTRA_INCVPATH += $(VIS_WRAP_MOD_ROOT)/inc
EXTRA_INCVPATH += $(VIS_WRAP_MOD_ROOT)/inc/qnx
EXTRA_INCVPATH += $(VIS_WRAP_MOD_ROOT)/../inc/common
EXTRA_INCVPATH += $(VIS_WRAP_MOD_ROOT)/../../../../vendors/ambarella/inc/dsp/
EXTRA_INCVPATH += $(VIS_WRAP_MOD_ROOT)/../../../../vendors/ambarella/inc/dsp/$(AMBA_CHIP_ID)
EXTRA_INCVPATH += $(VIS_WRAP_MOD_ROOT)/../../../../vendors/ambarella/inc/io/
EXTRA_INCVPATH += $(VIS_WRAP_MOD_ROOT)/../../../../vendors/ambarella/inc/io/$(AMBA_CHIP_ID)
EXTRA_INCVPATH += $(VIS_WRAP_MOD_ROOT)/../../../../svc/comsvc/print/
EXTRA_INCVPATH += $(VIS_WRAP_MOD_ROOT)/../../../../svc/comsvc/misc
#EXTRA_INCVPATH += $(VIS_WRAP_MOD_ROOT)/../../../io/src/$(AMBA_CHIP_ID)/inc/qnx_public
#EXTRA_INCVPATH += $(VIS_WRAP_MOD_ROOT)/../../../io/src/$(AMBA_CHIP_ID)/inc/qnx_public/hw

LIST=CPU
include recurse.mk
