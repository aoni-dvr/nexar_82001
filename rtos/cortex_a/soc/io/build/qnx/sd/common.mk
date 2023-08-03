ifndef QCONFIG
QCONFIG=qconfig.mk
endif
include $(QCONFIG)

define PINFO
PINFO DESCRIPTION=SD driver
endef

ifndef USEFILE
USEFILE=$(PROJECT_ROOT)/sd.use
endif

EXTRA_INCVPATH += $(PROJECT_ROOT)/../../../../../vendors/ambarella/inc
EXTRA_INCVPATH += $(PROJECT_ROOT)/../../../../../vendors/ambarella/inc/io
EXTRA_INCVPATH += $(PROJECT_ROOT)/../../../../../vendors/ambarella/inc/io/$(AMBA_CHIP_ID)
EXTRA_INCVPATH += $(PROJECT_ROOT)/../../../src/$(AMBA_CHIP_ID)/inc
EXTRA_INCVPATH += $(PROJECT_ROOT)/../../../src/$(AMBA_CHIP_ID)/inc/rtsl
EXTRA_INCVPATH += $(PROJECT_ROOT)/../../../src/$(AMBA_CHIP_ID)/inc/csl
EXTRA_INCVPATH += $(PROJECT_ROOT)/../../../src/$(AMBA_CHIP_ID)/inc/reg
EXTRA_INCVPATH += $(PROJECT_ROOT)/../../../src/common/inc
EXTRA_INCVPATH += $(PROJECT_ROOT)/../../../src/common/inc/rtsl
EXTRA_INCVPATH += $(PROJECT_ROOT)/../../../src/common/inc/csl
EXTRA_INCVPATH += $(PROJECT_ROOT)/../../../src/common/inc/reg

LIBS += drvrS wrap_stdS m kal-ambarella

NAME=sd-ambarella
-include $(PROJECT_ROOT)/roots.mk

#####AUTO-GENERATED by packaging script... do not checkin#####
   INSTALL_ROOT_nto = $(PROJECT_ROOT)/../../../../../install
   USE_INSTALL_ROOT=1
##############################################################

include $(MKFILES_ROOT)/qtargets.mk
