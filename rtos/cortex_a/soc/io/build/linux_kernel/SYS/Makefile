# SPDX-License-Identifier: MIT

PWD         := $(shell pwd)

LINUXDIR    ?= /lib/modules/$(shell uname -r)/build
ARCH        ?= x86

ifneq ($(KERNELRELEASE),)

IO_OS_PATH := linux
export IO_OS_PATH

EXTRA_CFLAGS += -DCONFIG_LINUX
EXTRA_CFLAGS += -I$(AMBARELLA_PKG_DIR)/soc/osal
EXTRA_CFLAGS += -I$(AMBARELLA_PKG_DIR)/soc/osal/$(IO_OS_PATH)
EXTRA_CFLAGS += -I$(AMBARELLA_PKG_DIR)/vendors/ambarella/inc
EXTRA_CFLAGS += -I$(AMBARELLA_PKG_DIR)/vendors/ambarella/inc/io
EXTRA_CFLAGS += -I$(AMBARELLA_PKG_DIR)/vendors/ambarella/inc/io/$(AMBA_CHIP_ID)
EXTRA_CFLAGS += -I$(AMBARELLA_PKG_DIR)/soc/io/src/$(AMBA_CHIP_ID)/inc
EXTRA_CFLAGS += -I$(AMBARELLA_PKG_DIR)/soc/io/src/$(AMBA_CHIP_ID)/inc/csl
EXTRA_CFLAGS += -I$(AMBARELLA_PKG_DIR)/soc/io/src/$(AMBA_CHIP_ID)/inc/rtsl
EXTRA_CFLAGS += -I$(AMBARELLA_PKG_DIR)/soc/io/src/$(AMBA_CHIP_ID)/inc/reg
EXTRA_CFLAGS += -I$(AMBARELLA_PKG_DIR)/soc/io/src/$(AMBA_CHIP_ID)/inc/reg/debugport
EXTRA_CFLAGS += -I$(AMBARELLA_PKG_DIR)/soc/io/src/$(AMBA_CHIP_ID)/inc/linux_public

KBUILD_EXTRA_SYMBOLS = $(AMBARELLA_PKG_DIR)/soc/osal/build/linux_kernel/Module.symvers

obj-m += ambasys_sdk.o

# src
ambasys_sdk-y := ../../../src/common/io-mw/$(IO_OS_PATH)/SYS/main.o

else # No $(KERNELRELEASE)

all:
	$(MAKE) ARCH=$(ARCH) -C $(LINUXDIR) M=$(CURDIR) modules
	$(MAKE) ARCH=$(ARCH) -C $(LINUXDIR) M=$(CURDIR) modules_install

clean:
	@rm -rf *.o *.ko *.mod.c *~ .*.omd *.o.omd .*.o.omd \
		Module.symvers modules.order .tmp_versions modules.builtin
	$(MAKE) ARCH=$(ARCH) -C $(LINUXDIR) M=$(CURDIR) clean

endif # $(KERNELRELEASE)
