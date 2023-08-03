ifeq ($(CONFIG_BUILD_CV),y)

################################################################################
#
# CV_CORE_DIR: relative path of core cv locate
#
################################################################################

CV_CORE_DIR:=soc/vision
export CV_CORE_DIR

################################################################################

CURDIR := $(patsubst %/,%,$(dir $(abspath $(lastword $(MAKEFILE_LIST)))))

include $(srctree)/$(src)/config/$(shell echo $(CONFIG_CV_CONFIG_FILE))


ccflags-y := -I$(srctree)/vendors/ambarella/inc
ccflags-y += -I$(srctree)/vendors/ambarella/inc/io
ccflags-y += -I$(srctree)/vendors/ambarella/inc/io/$(AMBA_CHIP_ID)
ccflags-y += -I$(srctree)/vendors/ambarella/inc/vision
ccflags-y += -I$(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc
ccflags-y += -I$(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc/rtsl
ccflags-y += -I$(srctree)/svc/comsvc/print
ccflags-y += -I$(srctree)/svc/comsvc/shell/inc
ccflags-y += -I$(srctree)/svc/comsvc/misc
ifneq ($(CONFIG_BSP_NAME),)
ccflags-y += -I$(srctree)/bsp/$(AMBA_CHIP_ID)/$(shell echo $(CONFIG_BSP_NAME))
endif

ifeq ($(CONFIG_LINUX),y)
include $(srctree)/$(CV_CORE_DIR)/build/linux/linux.mk
include $(srctree)/$(CV_CORE_DIR)/build/linux/linux_inc.mk
else
include $(srctree)/$(CV_CORE_DIR)/build/threadx/threadx.mk
include $(srctree)/$(CV_CORE_DIR)/build/threadx/threadx_inc.mk
endif

endif

# To avoid clean srctree files since objtree would equal to srctree while "make clean"
ifneq ($(objtree), $(srctree))
clean-dirs  += arm_framework cvtask_tbar config *.*
endif
