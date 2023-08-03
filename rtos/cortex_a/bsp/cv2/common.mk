ifndef QCONFIG
QCONFIG=qconfig.mk
endif
include $(QCONFIG)

include $(objtree)/include/config/auto.conf

EXTRA_SRCVPATH += $(PROJECT_ROOT)/$(AMBA_BSP_NAME)/

# src
SRCS := bsp.c

# secure boot & foss-mbedtls
ifeq ($(CONFIG_BUILD_FOSS_MBEDTLS),y)
ifeq ($(CONFIG_ENABLE_SECURITY),y)
SRCS += AmbaRsa.c
endif
endif

EXTRA_INCVPATH += $(PROJECT_ROOT)/$(AMBA_BSP_NAME)/
EXTRA_INCVPATH += $(PROJECT_ROOT)/../../vendors/ambarella/inc/io/
EXTRA_INCVPATH += $(PROJECT_ROOT)/../../vendors/ambarella/inc/io/$(AMBA_CHIP_ID)/
EXTRA_INCVPATH += $(PROJECT_ROOT)/../../svc/comsvc/print/
EXTRA_INCVPATH += $(PROJECT_ROOT)/../../output/

NAME=bsp

include $(MKFILES_ROOT)/qtargets.mk
