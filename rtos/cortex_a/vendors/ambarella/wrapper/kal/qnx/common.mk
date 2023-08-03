ifndef QCONFIG
QCONFIG=qconfig.mk
endif
include $(QCONFIG)

include ../../prodroot_pkt.mk

USEFILE=$(PROJECT_ROOT)/libkal-ambarella.use

## To build unit test code
#1. mv aarch64/dll.le/ aarch64/le
#2. do not set EXCLUDE_OBJS
EXCLUDE_OBJS=main.o
EXTRA_INCVPATH += $(PROJECT_ROOT)/../../../inc
EXTRA_INCVPATH += $(PROJECT_ROOT)/../../../inc/io
EXTRA_INCVPATH += $(PROJECT_ROOT)/../../../inc/io/$(AMBA_CHIP_ID)

LIBS = drvrS cache

NAME=kal-ambarella

define PINFO
PINFO DESCRIPTION=
endef

include $(MKFILES_ROOT)/qmacros.mk

include $(PROJECT_ROOT)/pinfo.mk
-include $(PROJECT_ROOT)/roots.mk
   INSTALL_ROOT_nto = $(PROJECT_ROOT)/../../../../../output/install.qnx
   USE_INSTALL_ROOT=1
include $(MKFILES_ROOT)/qtargets.mk
