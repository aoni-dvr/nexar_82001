ifndef QCONFIG
QCONFIG=qconfig.mk
endif
include $(QCONFIG)

define PINFO
PINFO DESCRIPTION=FS engine driver
endef

ifndef USEFILE
USEFILE=$(PROJECT_ROOT)/fs.use
endif

EXTRA_INCVPATH += $(PROJECT_ROOT)/../../../../../vendors/ambarella/inc
EXTRA_INCVPATH += $(PROJECT_ROOT)/../../../../../vendors/ambarella/inc/io
EXTRA_INCVPATH += $(PROJECT_ROOT)/../../../../../vendors/ambarella/inc/io/$(AMBA_CHIP_ID)
EXTRA_INCVPATH += $(PROJECT_ROOT)/../../../../../svc/comsvc/misc

LIBS += drvrS \
	wrap_stdS m \
	comsvc-ambarella-misc

NAME=fs-ambarella
-include $(PROJECT_ROOT)/roots.mk

#####AUTO-GENERATED by packaging script... do not checkin#####
   INSTALL_ROOT_nto = $(PROJECT_ROOT)/../../../../../install
   USE_INSTALL_ROOT=1
##############################################################

include $(MKFILES_ROOT)/qtargets.mk
