CURDIR := $(patsubst %/build/linux/,%,$(dir $(abspath $(lastword $(MAKEFILE_LIST)))))
APP := $(notdir $(CURDIR))
obj-y += libcv_$(APP).a

libcv_$(APP)-objs := ../../vpstatus.o
libcv_$(APP)-objs += ../../os/os_wrap.o

ccflags-y := -I$(srctree)/vendors/ambarella/inc
ccflags-y += -I$(srctree)/vendors/ambarella/inc/io
ccflags-y += -I$(srctree)/vendors/ambarella/inc/io/$(AMBA_CHIP_ID)
ccflags-y += -I$(srctree)/vendors/ambarella/inc/vision
ccflags-y += -I$(srctree)/vendors/ambarella/inc/prfile2
ccflags-y += -I$(srctree)/vendors/ambarella/inc/threadx
ccflags-y += -I$(srctree)/soc/io/src/common/inc/arm/
ccflags-y += -I$(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc
ccflags-y += -I$(srctree)/$(CV_CORE_DIR)/cv_common/inc
ccflags-y += -I$(srctree)/$(CV_CORE_DIR)/cv_common/inc/cvapi
ccflags-y += -I$(srctree)/svc/comsvc/print
ccflags-y += -I$(srctree)/svc/comsvc/shell/inc
ccflags-y += -I$(srctree)/svc/comsvc/misc
ccflags-y += -I$(srctree)/$(CV_COMSVC_DIR)/cv_common/inc/cvapi
ccflags-y += -I$(srctree)/$(CV_CORE_DIR)/cv_common/inc/cvsched_common
ccflags-y += -I$(srctree)/$(CV_CORE_DIR)/arm_framework/scheduler/inc
ccflags-y += -I$(CURDIR)/os
ccflags-y += -I$(CURDIR)/include
ccflags-y += -I$(OUTDIR)
ccflags-y += -O3 -g

ifeq ($(CONFIG_BUILD_MAL),y)
ccflags-y += -DENABLE_AMBA_MAL
endif

install-files := libcv_$(APP).a
clean-files += libcv_$(APP).a $(libcv_$(APP)-objs)

