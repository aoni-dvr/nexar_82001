PWD		:= $(shell pwd)

LINUXDIR	?= /lib/modules/$(shell uname -r)/build
ARCH		?= x86

ccflags-y += -Dambarella_soc_$(AMBARELLA_ARCH_VAR)

#obj-y += ambdram_statis.o
obj-m += ambdram_statis.o

.PHONY: all install modules modules_install clean

all:
	$(MAKE) --no-print-directory ARCH=$(ARCH) -C $(LINUXDIR) M=$(CURDIR)

install:
	$(MAKE) --no-print-directory ARCH=$(ARCH) -C $(LINUXDIR) M=$(CURDIR) install

modules:
	$(MAKE) --no-print-directory ARCH=$(ARCH) -C $(LINUXDIR) M=$(CURDIR) modules

modules_install:
	$(MAKE) --no-print-directory ARCH=$(ARCH) -C $(LINUXDIR) M=$(CURDIR) modules_install

outdbg:
	echo $(ccflags-y)
	echo $(obj-m)
	echo $(obj-y)
	echo $(ARCH)
	echo $(LINUXDIR)
	echo $(CURDIR)
	echo $(BR2_PACKAGE_AMBA_DRAM_STATIS_VAR)
	echo $(AMBARELLA_ARCH_VAR)
	echo $(AMBA_DRAM_STATIS_DRV_VAR)

clean:
	@rm -rf *.o *.ko *.mod.c *~ .*.cmd *.o.cmd .*.o.cmd \
		Module.symvers modules.order .tmp_versions modules.builtin
	$(Q)$(MAKE) --no-print-directory ARCH=$(ARCH) -C $(LINUXDIR) M=$(CURDIR) clean

