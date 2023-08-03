# SPDX-License-Identifier: MIT

PWD		:= $(shell pwd)

LINUXDIR	?= /lib/modules/$(shell uname -r)/build
ARCH		?= x86

ifneq ($(KERNELRELEASE),)

PWD	:= $(shell pwd)

BUILD_SHA1 := $(git describe --dirty)
BUILD_USER := $(shell whoami)@$(shell hostname)
BUILD_TIME := $(shell date)

CVCHIP_ID  := $(shell echo $(PROJECT) | tr 'a-z' 'A-Z')

ifeq ($(CVCHIP_ID),CV2A)
EXTRA_CFLAGS	        += -DASIL_COMPLIANCE
endif

ifeq ($(CVCHIP_ID),CV22A)
EXTRA_CFLAGS	        += -DASIL_COMPLIANCE
endif

ifeq ($(CVCHIP_ID),CV2FS)
EXTRA_CFLAGS	        += -DASIL_COMPLIANCE
endif

ifeq ($(CVCHIP_ID),CV22FS)
EXTRA_CFLAGS	        += -DASIL_COMPLIANCE
endif

EXTRA_CFLAGS            += -DBUILD_SHA1="\"$(BUILD_SHA1)\""
EXTRA_CFLAGS            += -DBUILD_USER="\"$(BUILD_USER)\""
EXTRA_CFLAGS            += -DBUILD_TIME="\"$(BUILD_TIME)\""
EXTRA_CFLAGS            += -DCHIP_$(CVCHIP_ID)=1
EXTRA_CFLAGS	        += -DCONFIG_ARM64 -DUSE_AMBA_KAL -DENABLE_AMBA_MAL
EXTRA_CFLAGS            += -I$(AMBARELLA_PKG_DIR)/soc/osal
EXTRA_CFLAGS            += -I$(AMBARELLA_PKG_DIR)/vendors/ambarella/inc
EXTRA_CFLAGS            += -I$(AMBARELLA_PKG_DIR)/soc/vision/cv_common/inc/cavalry
EXTRA_CFLAGS            += -I$(AMBARELLA_PKG_DIR)/soc/vision/cv_common/inc/cvapi
EXTRA_CFLAGS            += -I$(AMBARELLA_PKG_DIR)/soc/vision/cv_common/inc/cvsched_common
EXTRA_CFLAGS            += -I$(AMBARELLA_PKG_DIR)/soc/vision/cv_common/inc
EXTRA_CFLAGS            += -I$(AMBARELLA_PKG_DIR)/soc/vision/arm_framework/scheduler/inc
EXTRA_CFLAGS            += -I$(AMBARELLA_PKG_DIR)/soc/vision/arm_framework/scheduler/kernel
EXTRA_CFLAGS            += -I$(AMBARELLA_PKG_DIR)/soc/vision/arm_framework/scheduler/kernel/linux

KBUILD_EXTRA_SYMBOLS += $(AMBARELLA_PKG_DIR)/soc/osal/build/linux_kernel/Module.symvers
KBUILD_EXTRA_SYMBOLS += $(AMBARELLA_PKG_DIR)/vendors/ambarella/wrapper/mal/build/linux_kernel/Module.symvers

obj-m 	+= ambacv_sdk.o

ambacv_sdk-y := ../../arm_framework/scheduler/kernel/ambacv_kernel.o     \
	    ../../arm_framework/scheduler/kernel/cache_kernel.o     		\
	    ../../arm_framework/scheduler/kernel/flexidag_kernel.o     	\
	    ../../arm_framework/scheduler/kernel/msg_kernel.o    	\
	    ../../arm_framework/scheduler/kernel/schdr_kernel.o    	\
	    ../../arm_framework/scheduler/kernel/util_kernel.o   		\
	    ../../arm_framework/scheduler/kernel/visorc_kernel.o    		\
	    ../../arm_framework/scheduler/kernel/idsporc_kernel.o    		\
	    ../../arm_framework/scheduler/kernel/vinorc_kernel.o    		\
	    ../../arm_framework/scheduler/kernel/linux/ambacv_dev.o    \
	    ../../arm_framework/scheduler/kernel/linux/cavalry_cma.o   \
	    ../../arm_framework/scheduler/kernel/linux/os_kernel.o

else # No $(KERNELRELEASE)

all:
	echo "make scheduler"
	$(MAKE) ARCH=$(ARCH) -C $(LINUXDIR) M=$(CURDIR) modules
	echo "install scheduler"
	$(MAKE) ARCH=$(ARCH) -C $(LINUXDIR) M=$(CURDIR) modules_install

clean:
	@rm -rf *.o *.ko *.mod.c *~ .*.cmd *.o.cmd .*.o.cmd \
		Module.symvers modules.order .tmp_versions modules.builtin
	$(MAKE) ARCH=$(ARCH) -C $(LINUXDIR) M=$(CURDIR) clean

endif # $(KERNELRELEASE)