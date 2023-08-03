ifndef QCONFIG
QCONFIG=qconfig.mk
endif
include $(QCONFIG)

#include $(PROJECT_ROOT)/prodroot_pkt.mk

USEFILE=$(PROJECT_ROOT)/lib_io_utility.use

EXTRA_INCVPATH += $(PROJECT_ROOT)/../../../../../vendors/ambarella/inc
EXTRA_INCVPATH += $(PROJECT_ROOT)/../../../../../vendors/ambarella/inc/io
EXTRA_INCVPATH += $(PROJECT_ROOT)/../../../../../vendors/ambarella/inc/io/$(AMBA_CHIP_ID)
EXTRA_INCVPATH += $(PROJECT_ROOT)/../../../src/common/inc


LIBS = fdt

NAME=io-utility

define PINFO
PINFO DESCRIPTION=
endef

include $(MKFILES_ROOT)/qmacros.mk

include $(PROJECT_ROOT)/pinfo.mk
-include $(PROJECT_ROOT)/roots.mk
   INSTALL_ROOT_nto = $(PROJECT_ROOT)/../../../../../output/install.qnx
   USE_INSTALL_ROOT=1
include $(MKFILES_ROOT)/qtargets.mk
