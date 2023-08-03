ifndef QCONFIG
QCONFIG=qconfig.mk
endif
include $(QCONFIG)
USEFILE=$(PROJECT_ROOT)/amba_iout.use

NAME:=amba_iout

define PINFO
PINFO DESCRIPTION=Ambarella IOUT Application
endef

EXTRA_INCVPATH += $(srctree)/svc/comsvc/misc
EXTRA_INCVPATH += $(srctree)/svc/comsvc/shell/inc
EXTRA_INCVPATH += $(srctree)/vendors/ambarella/wrapper/kal/qnx/public/sys

EXTRA_INCVPATH += \
		  $(srctree)/vendors/ambarella/inc \
		  $(srctree)/vendors/ambarella/inc/io \
		  $(srctree)/vendors/ambarella/inc/io/$(AMBA_CHIP_ID) \
		  $(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc \
		  $(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc/rtsl \
		  $(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc/csl \
		  $(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc/reg \
		  $(srctree)/soc/io/src/common/inc \
		  $(srctree)/soc/io/src/common/inc/arm \
		  $(srctree)/svc/comsvc/shell/inc \
		  $(srctree)/svc/comsvc/print \
		  $(srctree)/svc/comsvc/misc

LIBS += wrap_std io-utility kal-ambarella comsvc-ambarella-print comsvc-ambarella-misc comsvc-ambarella-shell io-mw bsp perif

ifeq ($(CONFIG_BUILD_MAL),y)
LIBS += mal
endif
include $(MKFILES_ROOT)/qmacros.mk

-include $(PROJECT_ROOT)/roots.mk
   INSTALL_ROOT_nto = $(PROJECT_ROOT)/../../output/install.qnx
   USE_INSTALL_ROOT=1
include $(MKFILES_ROOT)/qtargets.mk
