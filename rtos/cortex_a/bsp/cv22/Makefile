ifeq ($(CONFIG_QNX),y)
#### QNX
LIST=CPU
include recurse.mk

else ifeq ($(CONFIG_LINUX),y)
#### LINUX
obj-y += libbsp_$(AMBA_BSP_NAME).a

libbsp_$(AMBA_BSP_NAME)-objs :=
include $(src)/$(AMBA_BSP_NAME)/Makefile

ifeq ($(CONFIG_SVC_APPS_ICAM), y)
libbsp_$(AMBA_BSP_NAME)-objs += AmbaiCamPartition.o
endif

ifdef CONFIG_CC_ENABLE_INSTRUMENT_FUNC
libbsp_$(AMBA_BSP_NAME)-objs += AmbaFB_Utility.o
endif
libbsp_$(AMBA_BSP_NAME)-objs += AmbaTimerInfo.o
libbsp_$(AMBA_BSP_NAME)-objs += AmbaVerBsp.o

ifdef CONFIG_ENABLE_SECURITY
ifndef CONFIG_ATF_SPD_OPTEE
libbsp_$(AMBA_BSP_NAME)-objs += AmbaRsa.o
endif
endif   #CONFIG_ENABLE_SECURITY

install-files := libbsp_$(AMBA_BSP_NAME).a


#
CFLAGS_AmbaVerBsp.o = \
    -DAMBA_BUILD_DATE=$(AMBA_BUILD_DATE)         \
	-DAMBA_BUILD_DATE_NUM=$(AMBA_BUILD_DATE_NUM) \
	-DAMBA_BUILD_SEC_NUM=$(AMBA_BUILD_SEC_NUM)   \
	-DAMBA_BUILD_MACHINE=$(AMBA_BUILD_MACHINE)   \
	-DAMBA_CI_COUNT=$(AMBA_CI_COUNT)             \
	-DAMBA_CI_ID=$(AMBA_CI_ID)

ccflags-y := -I$(srctree)/vendors/ambarella/inc
ccflags-y += -I$(srctree)/vendors/ambarella/inc/io
ccflags-y += -I$(srctree)/vendors/ambarella/inc/io/$(AMBA_CHIP_ID)
ccflags-y += -I$(srctree)/vendors/ambarella/inc/threadx
ifeq ($(CONFIG_SVC_APPS_ICAM), y)
ccflags-y += -I$(O)
endif
ccflags-y += -I$(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc
ccflags-y += -I$(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc/csl
ccflags-y += -I$(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc/reg
ccflags-y += -I$(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc/rtsl
ccflags-y += -I$(srctree)/soc/io/src/common/inc
ccflags-y += -I$(srctree)/soc/io/src/common/inc/arm
ccflags-y += -I$(srctree)/soc/io/src/common/inc/csl
ccflags-y += -I$(srctree)/soc/io/src/common/inc/reg
ccflags-y += -I$(srctree)/soc/io/src/common/inc/rtsl
ccflags-y += -I$(srctree)/fwprog/inc
ccflags-y += -I$(srctree)/svc/comsvc/misc
ccflags-y += -I$(srctree)/svc/comsvc/shell/inc
ccflags-y += -I$(srctree)/svc/comsvc/print
asflags-y := -I$(srctree)/vendors/ambarella/inc

ifdef CONFIG_BUILD_FOSS_MBEDTLS
ccflags-y += -I$(srctree)
ccflags-y += -I$(srctree)/vendors/ambarella/inc
ccflags-y += -I$(srctree)/vendors/ambarella/inc/io
ccflags-y += -I$(srctree)/vendors
ccflags-y += -I$(srctree)/vendors/foss/mbedtls/src/ambarella/inc
ccflags-y += -I$(srctree)/vendors/foss/mbedtls/src/ambarella/inc/std
ccflags-y += -I$(srctree)/vendors/foss/mbedtls/src/mbedtls-2.16.4/include
ccflags-y += -I$(srctree)/vendors/foss/mbedtls/src/mbedtls-2.16.4/include/mbedtls

ccflags-y += -DMBEDTLS_USER_CONFIG_FILE='<config.h>'
ccflags-y += -D_FILE_OFFSET_BITS=64

endif	#CONFIG_BUILD_FOSS_MBEDTLS

else
ifdef CONFIG_ARM32

obj-y += libbsp_$(AMBA_BSP_NAME).a

libbsp_$(AMBA_BSP_NAME)-objs :=
include $(src)/$(AMBA_BSP_NAME)/Makefile

ifeq ($(CONFIG_SVC_APPS_ICAM), y)
libbsp_$(AMBA_BSP_NAME)-objs += AmbaiCamPartition.o
endif

ifdef CONFIG_CC_ENABLE_INSTRUMENT_FUNC
libbsp_$(AMBA_BSP_NAME)-objs += AmbaFB_Utility.o
endif
libbsp_$(AMBA_BSP_NAME)-objs += AmbaTimerInfo.o
libbsp_$(AMBA_BSP_NAME)-objs += AmbaVerBsp.o

ifdef CONFIG_ENABLE_SECURITY
ifndef CONFIG_ATF_SPD_OPTEE
libbsp_$(AMBA_BSP_NAME)-objs += AmbaRsa.o
endif
endif   #CONFIG_ENABLE_SECURITY

install-files := libbsp_$(AMBA_BSP_NAME).a


#
CFLAGS_AmbaVerBsp.o = \
    -DAMBA_BUILD_DATE=$(AMBA_BUILD_DATE)         \
	-DAMBA_BUILD_DATE_NUM=$(AMBA_BUILD_DATE_NUM) \
	-DAMBA_BUILD_SEC_NUM=$(AMBA_BUILD_SEC_NUM)   \
	-DAMBA_BUILD_MACHINE=$(AMBA_BUILD_MACHINE)   \
	-DAMBA_CI_COUNT=$(AMBA_CI_COUNT)             \
	-DAMBA_CI_ID=$(AMBA_CI_ID)

ccflags-y := -I$(srctree)/vendors/ambarella/inc
ccflags-y += -I$(srctree)/vendors/ambarella/inc/io
ccflags-y += -I$(srctree)/vendors/ambarella/inc/io/$(AMBA_CHIP_ID)
ccflags-y += -I$(srctree)/vendors/ambarella/inc/threadx
ifeq ($(CONFIG_SVC_APPS_ICAM), y)
ccflags-y += -I$(O)
endif
ccflags-y += -I$(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc
ccflags-y += -I$(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc/csl
ccflags-y += -I$(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc/reg
ccflags-y += -I$(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc/rtsl
ccflags-y += -I$(srctree)/soc/io/src/common/inc
ccflags-y += -I$(srctree)/soc/io/src/common/inc/arm
ccflags-y += -I$(srctree)/soc/io/src/common/inc/csl
ccflags-y += -I$(srctree)/soc/io/src/common/inc/reg
ccflags-y += -I$(srctree)/soc/io/src/common/inc/rtsl
ccflags-y += -I$(srctree)/fwprog/inc
ccflags-y += -I$(srctree)/svc/comsvc/misc
ccflags-y += -I$(srctree)/svc/comsvc/shell/inc
ccflags-y += -I$(srctree)/svc/comsvc/print
asflags-y := -I$(srctree)/vendors/ambarella/inc

ifdef CONFIG_BUILD_FOSS_MBEDTLS
ccflags-y += -I$(srctree)
ccflags-y += -I$(srctree)/vendors/ambarella/inc
ccflags-y += -I$(srctree)/vendors/ambarella/inc/io
ccflags-y += -I$(srctree)/vendors
ccflags-y += -I$(srctree)/vendors/foss/mbedtls/src/ambarella/inc
ccflags-y += -I$(srctree)/vendors/foss/mbedtls/src/ambarella/inc/std
ccflags-y += -I$(srctree)/vendors/foss/mbedtls/src/mbedtls-2.16.4/include
ccflags-y += -I$(srctree)/vendors/foss/mbedtls/src/mbedtls-2.16.4/include/mbedtls

ccflags-y += -DMBEDTLS_USER_CONFIG_FILE='<config.h>'
ccflags-y += -D_FILE_OFFSET_BITS=64

endif	#CONFIG_BUILD_FOSS_MBEDTLS

endif
endif
