# SPDX-License-Identifier: MIT

PWD		:= $(shell pwd)

LINUXDIR	?= /lib/modules/$(shell uname -r)/build
ARCH		?= x86

ifneq ($(KERNELRELEASE),)
obj-m += hello.o

hello-objs := main.o
#hello-objs := chardev.o

#ccflags-y += -DDEBUG

else # No $(KERNELRELEASE)

all:
	$(Q)$(MAKE) ARCH=$(ARCH) -C $(LINUXDIR) M=$(CURDIR) modules
	$(Q)$(MAKE) ARCH=$(ARCH) -C $(LINUXDIR) M=$(CURDIR) modules_install

clean:
	@rm -rf *.o *.ko *.mod.c *~ .*.cmd *.o.cmd .*.o.cmd \
		Module.symvers modules.order .tmp_versions modules.builtin
	$(Q)$(MAKE) ARCH=$(ARCH) -C $(LINUXDIR) M=$(CURDIR) clean

endif # $(KERNELRELEASE)

