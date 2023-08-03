ifndef QCONFIG
QCONFIG=qconfig.mk
endif
include $(QCONFIG)

USEFILE=$(PROJECT_ROOT)/comsvc-ambarella-misc.use

NAME=comsvc-ambarella-misc

define PINFO
PINFO DESCRIPTION=Common Service MISC for the AMBA platform
endef

EXTRA_SRCVPATH += $(PROJECT_ROOT)/../../..
EXTRA_INCVPATH += $(srctree)/svc/comsvc/misc

SRCS := AmbaUtility.c
SRCS += AmbaUtility_Crc32Hw.S

include $(MKFILES_ROOT)/qmacros.mk

-include $(PROJECT_ROOT)/roots.mk
   INSTALL_ROOT_nto = $(PROJECT_ROOT)/../../output/install.qnx
   USE_INSTALL_ROOT=1
include $(MKFILES_ROOT)/qtargets.mk
