# SPDX-License-Identifier: MIT

PWD		:= $(shell pwd)

LINUXDIR	?= /lib/modules/$(shell uname -r)/build
ARCH		?= x86

ifneq ($(KERNELRELEASE),)

PWD	:= $(shell pwd)

BUILD_SHA1 := $(git describe --dirty)
BUILD_USER := $(shell whoami)@$(shell hostname)
BUILD_TIME := $(shell date)

EXTRA_CFLAGS            += -I$(AMBARELLA_PKG_DIR)/vendors/ambarella/inc/
EXTRA_CFLAGS            += -I$(AMBARELLA_PKG_DIR)/vendors/ambarella/wrapper/mal/linux/
obj-m 	+= ambamal.o

ambamal-y := ../../linux/kdriver/AmbaMAL_Dev.o     \
			 ../../linux/kdriver/AmbaMAL_Mmb.o     \
			 ../../linux/kdriver/AmbaMAL_CMA.o	   \
			 ../../linux/kdriver/AmbaMAL_CMAOps.o	\
			 ../../linux/kdriver/AmbaMAL_AMAOps.o	\
			 ../../linux/kdriver/AmbaMAL_Krn.o

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