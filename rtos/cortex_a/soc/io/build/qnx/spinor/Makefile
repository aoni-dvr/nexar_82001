BSP_DIR = $(shell grep "CONFIG_BSP_NAME"  $(CURDIR)/../../../../../output/.config | sed 's/CONFIG_BSP_NAME="//g' | sed 's/"//g')
CHIP_ID = $(shell grep "CONFIG_SOC_NAME"  $(CURDIR)/../../../../../output/.config | sed 's/CONFIG_SOC_NAME="//g' | sed 's/"//g')
SPINOR_DRIVER = $(shell find $(CURDIR)/../../../../../bsp/$(CHIP_ID)/$(BSP_DIR)/ -name "AmbaSpiNOR*" -exec basename {} \; | cut -d"." -f 1).o

#$(warning $(BSP_DIR))
#$(warning $(CHIP_ID))
#$(warning $(SPINOR_DRIVER))

include $(objtree)/include/config/auto.conf

EXTRA_SRCVPATH += $(CURDIR)/../../../../../bsp/$(CHIP_ID)
EXTRA_SRCVPATH += $(CURDIR)/../../../../../bsp/$(CHIP_ID)/$(BSP_DIR)
EXTRA_SRCVPATH += $(CURDIR)/../../../src/common/rtsl
EXTRA_SRCVPATH += $(CURDIR)/../../../src/common/csl
EXTRA_SRCVPATH += $(CURDIR)/../../../src/common/io-mw/qnx
EXTRA_SRCVPATH += $(CURDIR)/../../../src/$(AMBA_CHIP_ID)/csl
EXTRA_SRCVPATH += $(CURDIR)/../../../src/$(AMBA_CHIP_ID)/io-mw/qnx
EXTRA_SRCVPATH += $(CURDIR)/../../../src/$(AMBA_CHIP_ID)/rtsl
EXTRA_SRCVPATH += $(srctree)/svc/comsvc/misc
EXTRA_SRCVPATH += $(CURDIR)/../../../src/common/spinor/qnx/

SRCS := \
	$(SPINOR_DRIVER) \
	\
	AmbaCSL_SpiNOR.o \
	AmbaRTSL_SpiNOR.o \
	AmbaRTSL_SpiNOR_Partition.o \
	AmbaSpiNOR.o \
	AmbaSpiNOR_Partition.o

ifeq ($(CONFIG_SVC_APPS_ICAM),y)
SRCS += AmbaiCamPartition.o
else
SRCS += AmbaUserNandPartition.o
endif

LIST=CPU
include recurse.mk
