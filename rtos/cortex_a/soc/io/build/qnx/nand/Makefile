BSP_DIR = $(shell grep "CONFIG_BSP_NAME"  $(CURDIR)/../../../../../output/.config | sed 's/CONFIG_BSP_NAME="//g' | sed 's/"//g')
CHIP_ID = $(shell grep "CONFIG_SOC_NAME"  $(CURDIR)/../../../../../output/.config | sed 's/CONFIG_SOC_NAME="//g' | sed 's/"//g')
SPINAND_DRIVER = $(shell find $(CURDIR)/../../../../../bsp/$(CHIP_ID)/$(BSP_DIR)/ -name "AmbaSPINAND*" -exec basename {} \; | cut -d"." -f 1).o
NAND_DRIVER    = $(shell find $(CURDIR)/../../../../../bsp/$(CHIP_ID)/$(BSP_DIR)/ -name "AmbaNAND*" -exec basename {} \; | cut -d"." -f 1).o

#$(warning $(BSP_DIR))
#$(warning $(CHIP_ID))
#$(warning $(NAND_DRIVER))

include $(objtree)/include/config/auto.conf

EXTRA_SRCVPATH += $(CURDIR)/../../../../../bsp/$(CHIP_ID)
EXTRA_SRCVPATH += $(CURDIR)/../../../../../bsp/$(CHIP_ID)/$(BSP_DIR)
EXTRA_SRCVPATH += $(CURDIR)/../../../src/common/io-mw/qnx
EXTRA_SRCVPATH += $(CURDIR)/../../../src/common/rtsl
EXTRA_SRCVPATH += $(CURDIR)/../../../src/common/csl
EXTRA_SRCVPATH += $(CURDIR)/../../../src/$(AMBA_CHIP_ID)/io-mw/qnx
EXTRA_SRCVPATH += $(CURDIR)/../../../src/$(AMBA_CHIP_ID)/rtsl
EXTRA_SRCVPATH += $(CURDIR)/../../../src/$(AMBA_CHIP_ID)/csl
EXTRA_SRCVPATH += $(srctree)/svc/comsvc/misc

ifeq ($(CONFIG_ENABLE_NAND_BOOT),y)
SRCS := $(NAND_DRIVER) AmbaNAND.o
else
SRCS := $(SPINAND_DRIVER) AmbaSPINAND.o
endif

SRCS += \
	AmbaCSL_NAND.o \
	AmbaCSL_SPINAND.o \
	AmbaCSL_FIO.o \
	\
	AmbaRTSL_FIO.o \
	AmbaRTSL_SPINAND.o \
	AmbaRTSL_NAND.o \
	AmbaRTSL_NAND_OP.o \
	AmbaRTSL_NAND_BBM.o \
	AmbaRTSL_NAND_Partition.o \
	\
	AmbaNAND_Partition.o \
	AmbaNAND_BBM.o \
	AmbaNAND_OP.o \
	\
	AmbaNAND_FTL.o \
	AmbaNFTL.o

ifeq ($(CONFIG_SVC_APPS_ICAM),y)
SRCS += AmbaiCamPartition.o
else
SRCS += AmbaUserNandPartition.o
endif

LIST=CPU
include recurse.mk
