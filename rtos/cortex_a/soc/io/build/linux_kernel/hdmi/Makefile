# SPDX-License-Identifier: MIT

PWD         := $(shell pwd)

LINUXDIR    ?= /lib/modules/$(shell uname -r)/build
ARCH        ?= x86

ifneq ($(KERNELRELEASE),)

IO_OS_PATH := linux
export IO_OS_PATH

EXTRA_CFLAGS += -DCONFIG_LINUX
ifeq ($(AMBA_CHIP_ID), cv5x)
EXTRA_CFLAGS += -DCONFIG_SOC_CV5
else
EXTRA_CFLAGS += -DCONFIG_SOC_$(shell echo $(AMBA_CHIP_ID) | tr '[a-z]' '[A-Z]')
endif
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
EXTRA_CFLAGS += -I$(AMBARELLA_PKG_DIR)/soc/io/src/common/inc
EXTRA_CFLAGS += -I$(AMBARELLA_PKG_DIR)/soc/io/src/common/inc/rtsl
EXTRA_CFLAGS += -I$(AMBARELLA_PKG_DIR)/soc/io/src/common/inc/csl
EXTRA_CFLAGS += -I$(AMBARELLA_PKG_DIR)/soc/io/src/common/inc/reg

KBUILD_EXTRA_SYMBOLS = $(AMBARELLA_PKG_DIR)/soc/osal/build/linux_kernel/Module.symvers
ifneq ($(wildcard $(AMBARELLA_PKG_DIR)/soc/io/src/$(AMBA_CHIP_ID)/hdmi/$(IO_OS_PATH)),)
obj-m += ambahdmi_sdk.o

# src
ambahdmi_sdk-y := ../../../src/$(AMBA_CHIP_ID)/hdmi/$(IO_OS_PATH)/main.o
ambahdmi_sdk-y += ../../../src/$(AMBA_CHIP_ID)/hdmi/$(IO_OS_PATH)/lib.o

ambahdmi_sdk-y += ../../../src/common/csl/AmbaCSL_HDMI.o
ambahdmi_sdk-y += ../../../src/common/rtsl/AmbaRTSL_HDMI.o
ambahdmi_sdk-y += ../../../src/common/rtsl/AmbaRTSL_HDMI_Video.o
ambahdmi_sdk-y += ../../../src/common/rtsl/AmbaRTSL_HDMI_Audio.o

ambahdmi_sdk-y += ../../../src/$(AMBA_CHIP_ID)/hdmi/$(IO_OS_PATH)/AmbaHDMI_Info.o
ambahdmi_sdk-y += ../../../src/$(AMBA_CHIP_ID)/hdmi/$(IO_OS_PATH)/AmbaRTSL_VOUT_TV.o
endif
else # No $(KERNELRELEASE)

all:
	$(MAKE) ARCH=$(ARCH) -C $(LINUXDIR) M=$(CURDIR) modules
	$(MAKE) ARCH=$(ARCH) -C $(LINUXDIR) M=$(CURDIR) modules_install

clean:
	@rm -rf *.o *.ko *.mod.c *~ .*.omd *.o.omd .*.o.omd \
		Module.symvers modules.order .tmp_versions modules.builtin
	$(MAKE) ARCH=$(ARCH) -C $(LINUXDIR) M=$(CURDIR) clean

endif # $(KERNELRELEASE)
