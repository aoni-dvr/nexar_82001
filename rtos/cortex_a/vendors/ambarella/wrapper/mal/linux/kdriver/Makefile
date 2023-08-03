# SPDX-License-Identifier: MIT

PWD		:= $(shell pwd)

LINUXDIR	?= /lib/modules/$(shell uname -r)/build
ARCH		?= x86

CVCHIP_ID  := $(shell echo $(PROJECT) | tr 'a-z' 'A-Z')

ifeq ($(CVCHIP_ID),CV6)
EXTRA_CFLAGS	        += -DAST_SUPPORT
endif

ifneq ($(KERNELRELEASE),)
EXTRA_CFLAGS            += -I$(AMBARELLA_PKG_DIR)/ambamal/inc
EXTRA_CFLAGS            += -I$(AMBARELLA_PKG_DIR)/ambamal/linux

obj-m 	+= ambamal.o

ambamal-y := AmbaMAL_Dev.o     \
			 AmbaMAL_Mmb.o     \
			 AmbaMAL_CMA.o	   \
			 AmbaMAL_CMAOps.o   \
			 AmbaMAL_AMAOps.o   \
			 AmbaMAL_Krn.o

else # No $(KERNELRELEASE)

all:
	$(MAKE) ARCH=$(ARCH) -C $(LINUXDIR) M=$(CURDIR) modules
	$(MAKE) ARCH=$(ARCH) -C $(LINUXDIR) M=$(CURDIR) modules_install

clean:
	@rm -rf *.o *.ko *.mod.c *~ .*.cmd *.o.cmd .*.o.cmd \
		Module.symvers modules.order .tmp_versions modules.builtin
	$(MAKE) ARCH=$(ARCH) -C $(LINUXDIR) M=$(CURDIR) clean

endif # $(KERNELRELEASE)
