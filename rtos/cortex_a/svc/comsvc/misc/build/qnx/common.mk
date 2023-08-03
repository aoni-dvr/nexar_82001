ifndef QCONFIG
QCONFIG=qconfig.mk
endif
include $(QCONFIG)

include $(objtree)/include/config/auto.conf

COMSVC_MISC_SRC = $(PROJECT_ROOT)/../..
COMSVC_MISC_INC = $(COMSVC_MISC_SRC)

EXTRA_SRCVPATH += $(COMSVC_MISC_SRC)

SRCS := AmbaDspInt.c AmbaSvcWrap.c

EXTRA_INCVPATH += $(COMSVC_MISC_INC)

EXTRA_INCVPATH += $(srctree)/vendors/ambarella/inc
EXTRA_INCVPATH += $(srctree)/vendors/ambarella/inc/dsp
EXTRA_INCVPATH += $(srctree)/vendors/ambarella/inc/dsp/$(AMBA_CHIP_ID)

EXTRA_INCVPATH += $(srctree)/soc/io/src/$(AMBA_CHIP_ID)/qnx/io-mw/public
EXTRA_INCVPATH += $(srctree)/vendors/ambarella/inc/io
EXTRA_INCVPATH += $(srctree)/vendors/ambarella/inc/io/$(AMBA_CHIP_ID)

EXTRA_INCVPATH += $(srctree)/svc/comsvc/print

NAME=comsvc_misc

include $(MKFILES_ROOT)/qtargets.mk
