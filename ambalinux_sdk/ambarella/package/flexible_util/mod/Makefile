# SPDX-License-Identifier: GPL-2.0

PWD		:= $(shell pwd)

LINUXDIR	?= /lib/modules/$(shell uname -r)/build
ARCH		?= x86

obj-m += ambad.o hw_timer.o

ambad-y := debug_core.o

#ccflags-y += -DDEBUG

all:
	$(Q)$(MAKE) --no-print-directory ARCH=$(ARCH) -C $(LINUXDIR) M=$(CURDIR) modules

modules_install:
	$(Q)$(MAKE) --no-print-directory ARCH=$(ARCH) -C $(LINUXDIR) M=$(CURDIR) modules_install

clean:
	@rm -rf *.o *.ko *.mod.c *~ .*.cmd *.o.cmd .*.o.cmd \
		Module.symvers modules.order .tmp_versions modules.builtin
	$(Q)$(MAKE) --no-print-directory ARCH=$(ARCH) -C $(LINUXDIR) M=$(CURDIR) clean

