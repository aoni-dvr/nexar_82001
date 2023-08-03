ifndef QCONFIG
QCONFIG=qconfig.mk
endif
include $(QCONFIG)

USEFILE=$(PROJECT_ROOT)/comsvc-ambarella-test.use

NAME=comsvc-ambarella-test

define PINFO
PINFO DESCRIPTION=Test Application for Common Service libraries on the AMBA platform
endef

EXTRA_INCVPATH += $(srctree)/svc/comsvc/misc
EXTRA_INCVPATH += $(srctree)/svc/comsvc/shell/inc

LIBS += comsvc-ambarella-shell

include $(MKFILES_ROOT)/qmacros.mk

-include $(PROJECT_ROOT)/roots.mk
   INSTALL_ROOT_nto = $(PROJECT_ROOT)/../../output/install.qnx
   USE_INSTALL_ROOT=1
include $(MKFILES_ROOT)/qtargets.mk
