ifndef QCONFIG
QCONFIG=qconfig.mk
endif
include $(QCONFIG)

include ../../prodroot_pkt.mk

USEFILE=$(PROJECT_ROOT)/libiomw.use

EXTRA_INCVPATH += $(PROJECT_ROOT)/../../../../../vendors/ambarella/inc
EXTRA_INCVPATH += $(PROJECT_ROOT)/../../../../../vendors/ambarella/inc/io
EXTRA_INCVPATH += $(PROJECT_ROOT)/../../../../../vendors/ambarella/inc/io/$(AMBA_CHIP_ID)
EXTRA_INCVPATH += $(PROJECT_ROOT)/../../../src/$(AMBA_CHIP_ID)/inc
EXTRA_INCVPATH += $(PROJECT_ROOT)/../../../src/$(AMBA_CHIP_ID)/inc/csl
EXTRA_INCVPATH += $(PROJECT_ROOT)/../../../src/$(AMBA_CHIP_ID)/inc/rtsl
EXTRA_INCVPATH += $(PROJECT_ROOT)/../../../src/$(AMBA_CHIP_ID)/inc/reg
EXTRA_INCVPATH += $(PROJECT_ROOT)/../../../src/$(AMBA_CHIP_ID)/inc/reg/debugport
EXTRA_INCVPATH += $(PROJECT_ROOT)/../../../src/$(AMBA_CHIP_ID)/inc/qnx_public
EXTRA_INCVPATH += $(PROJECT_ROOT)/../../../src/common/inc
EXTRA_INCVPATH += $(PROJECT_ROOT)/../../../src/common/inc/arm
EXTRA_INCVPATH += $(PROJECT_ROOT)/../../../src/common/inc/csl
EXTRA_INCVPATH += $(PROJECT_ROOT)/../../../src/common/inc/rtsl
EXTRA_INCVPATH += $(PROJECT_ROOT)/../../../src/common/inc/reg
EXTRA_INCVPATH += $(PROJECT_ROOT)/../../../src/common/inc/qnx_public

## To build unit test code
#1. mv aarch64/dll.le/ aarch64/le
#2. do not set EXCLUDE_OBJS
EXCLUDE_OBJS=main.o

LIBS = drvrS kal-ambarella io-utility wrap_std m

NAME=io-mw

define PINFO
PINFO DESCRIPTION=
endef

include $(MKFILES_ROOT)/qmacros.mk

include $(PROJECT_ROOT)/pinfo.mk
-include $(PROJECT_ROOT)/roots.mk
   INSTALL_ROOT_nto = $(PROJECT_ROOT)/../../../../../output/install.qnx
   USE_INSTALL_ROOT=1
include $(MKFILES_ROOT)/qtargets.mk
