ifndef QCONFIG
QCONFIG=qconfig.mk
endif
include $(QCONFIG)

include ../prodroot_pkt.mk

USEFILE=../audio_wrapper.use

EXCLUDE_OBJS=

LIBS = 

EXTRA_SRCVPATH += $(PROJECT_ROOT)/src/
EXTRA_SRCVPATH += $(PROJECT_ROOT)/src/qnx
EXTRA_INCVPATH += $(PROJECT_ROOT)/inc
EXTRA_INCVPATH += $(PROJECT_ROOT)/inc/qnx
EXTRA_INCVPATH += $(PROJECT_ROOT)/../../../io/$(AMBA_CHIP_ID)/qnx/vin/public
EXTRA_INCVPATH += $(PROJECT_ROOT)/../../../io/$(AMBA_CHIP_ID)/qnx/vin/public/hw
EXTRA_INCVPATH += $(PROJECT_ROOT)/../../../io/$(AMBA_CHIP_ID)/qnx/vout/public
EXTRA_INCVPATH += $(PROJECT_ROOT)/../../../io/$(AMBA_CHIP_ID)/qnx/vout/public/hw
EXTRA_INCVPATH += $(PROJECT_ROOT)/../../../../vendors/ambarella/inc/dsp
EXTRA_INCVPATH += $(PROJECT_ROOT)/../../../../vendors/ambarella/inc/dsp/$(AMBA_CHIP_ID)/

NAME=audio_wrapper

define PINFO
PINFO DESCRIPTION=
endef

include $(MKFILES_ROOT)/qmacros.mk

include ../pinfo.mk
-include $(PROJECT_ROOT)/roots.mk
   INSTALL_ROOT_nto = $(PROJECT_ROOT)/../../../install
   USE_INSTALL_ROOT=1
include $(MKFILES_ROOT)/qtargets.mk
