ifeq ($(CONFIG_BUILD_COMMON_SERVICE_CV),y)
################################################################
#                             ThreadX                          #
################################################################

################################################################################
#
# CV_CORE_DIR: relative path of core cv locate
# CV_COMSVC_DIR: relative path of comsvc cv locate
#
################################################################################

CV_CORE_DIR:=soc/vision
CV_COMSVC_DIR:=svc/comsvc/cv
export CV_CORE_DIR CV_COMSVC_DIR

################################################################################

CURDIR := $(patsubst %/,%,$(dir $(abspath $(lastword $(MAKEFILE_LIST)))))

include $(srctree)/$(src)/config/$(shell echo $(CONFIG_CV_CONFIG_FILE))


ccflags-y := -I$(srctree)/vendors/ambarella/inc
ccflags-y += -I$(srctree)/vendors/ambarella/inc/io
ccflags-y += -I$(srctree)/vendors/ambarella/inc/io/$(AMBA_CHIP_ID)
ccflags-y += -I$(srctree)/vendors/ambarella/inc/vision
ccflags-y += -I$(srctree)/soc/io/src/common/inc/arm/
ccflags-y += -I$(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc
ccflags-y += -I$(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc/reg
ccflags-y += -I$(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc/csl
ccflags-y += -I$(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc/rtsl
ccflags-y += -I$(srctree)/svc/comsvc/print
ccflags-y += -I$(srctree)/svc/comsvc/shell/inc
ccflags-y += -I$(srctree)/svc/comsvc/misc

-include $(CURDIR)/cv_inc.mk
ifeq ($(CONFIG_LINUX),y)
include $(srctree)/$(CV_CORE_DIR)/build/linux/linux_inc.mk
else
include $(srctree)/$(CV_CORE_DIR)/build/threadx/threadx_inc.mk
endif

endif

# To avoid clean srctree files since objtree would equal to srctree while "make clean"
ifneq ($(objtree), $(srctree))
clean-dirs  += arm_framework cvtask_tbar config *.*
endif
