# SPDX-License-Identifier: MIT

PWD		:= $(shell pwd)

LINUXDIR	?= /lib/modules/$(shell uname -r)/build
ARCH		?= x86

ifneq ($(KERNELRELEASE),)

IO_OS_PATH := linux
export IO_OS_PATH

obj-m += ambavio_sdk.o

# src
ambavio_sdk-y := ../../../src/common/vio/$(IO_OS_PATH)/main.o

else # No $(KERNELRELEASE)

all:
	$(MAKE) ARCH=$(ARCH) -C $(LINUXDIR) M=$(CURDIR) modules
	$(MAKE) ARCH=$(ARCH) -C $(LINUXDIR) M=$(CURDIR) modules_install

clean:
	@rm -rf *.o *.ko *.mod.c *~ .*.omd *.o.omd .*.o.omd \
		Module.symvers modules.order .tmp_versions modules.builtin
	$(MAKE) ARCH=$(ARCH) -C $(LINUXDIR) M=$(CURDIR) clean

endif # $(KERNELRELEASE)
