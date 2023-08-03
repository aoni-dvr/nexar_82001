###############################################################
#                             ThreadX                         #
###############################################################
CURDIR := $(patsubst %/,%,$(dir $(abspath $(lastword $(MAKEFILE_LIST)))))
APP := $(notdir $(CURDIR))

include $(srctree)/$(CV_CORE_DIR)/arm_framework/cvchip.mk

ifeq ($(CONFIG_BUILD_MAL),y)
ccflags-y += -DENABLE_AMBA_MAL
endif

ccflags-y := -I$(srctree)/vendors/ambarella/inc
ccflags-y += -I$(srctree)/vendors/ambarella/inc/io
ccflags-y += -I$(srctree)/vendors/ambarella/inc/io/$(AMBA_CHIP_ID)
ccflags-y += -I$(srctree)/vendors/ambarella/inc/vision
ccflags-y += -I$(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc
ccflags-y += -I$(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc/rtsl
ccflags-y += -I$(srctree)/$(CV_CORE_DIR)/cv_common/inc
ccflags-y += -I$(srctree)/$(CV_CORE_DIR)/cv_common/inc/cvapi
ccflags-y += -I$(srctree)/svc/comsvc/print
ccflags-y += -I$(srctree)/svc/comsvc/shell/inc
ccflags-y += -I$(srctree)/svc/comsvc/misc
ccflags-y += -I$(OUTDIR)
ccflags-y += -O3 -g -DUSE_AMBA_KAL -D$(CVCHIP)

obj-y += libcv_$(APP).a

C_SRCS := $(patsubst $(CURDIR)/%,%,$(wildcard $(CURDIR)/*.c))
libcv_$(APP)-objs = $(C_SRCS:%.c=%.o)
install-files += libcv_$(APP).a
