# SPDX-License-Identifier: MIT

PWD         := $(shell pwd)

LINUXDIR	?= /lib/modules/$(shell uname -r)/build
ARCH		?= x86

ifneq ($(KERNELRELEASE),)

EXTRA_CFLAGS            += -I$(AMBARELLA_PKG_DIR)/soc/osal
EXTRA_CFLAGS            += -I$(AMBARELLA_PKG_DIR)/vendors/ambarella/inc

obj-m 	+= osal.o

osal-y := ../../linux/osal_linux.o	\
          ../../linux/osal_dev.o		\
          ../../linux/osal_utility.o

else # No $(KERNELRELEASE)

all:
	$(MAKE) ARCH=$(ARCH) -C $(LINUXDIR) M=$(CURDIR) modules
	$(MAKE) ARCH=$(ARCH) -C $(LINUXDIR) M=$(CURDIR) modules_install

clean:
	@rm -rf *.o *.ko *.mod.c *~ .*.cmd *.o.cmd .*.o.cmd \
		Module.symvers modules.order .tmp_versions modules.builtin
	$(MAKE) ARCH=$(ARCH) -C $(LINUXDIR) M=$(CURDIR) clean

endif # $(KERNELRELEASE)