ifndef QCONFIG
QCONFIG=qconfig.mk
endif
include $(QCONFIG)

USEFILE=$(PROJECT_ROOT)/comsvc-ambarella-print.use

NAME=comsvc-ambarella-print
LIBS = kal-ambarella comsvc-ambarella-misc wrap_std m

define PINFO
PINFO DESCRIPTION=Common Service Print library for the AMBA platform
endef

include $(MKFILES_ROOT)/qmacros.mk

-include $(PROJECT_ROOT)/roots.mk
   INSTALL_ROOT_nto = $(PROJECT_ROOT)/../../../../../output/install.qnx
   USE_INSTALL_ROOT=1
include $(MKFILES_ROOT)/qtargets.mk
