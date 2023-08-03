BSP_DIR = $(shell grep "CONFIG_BSP_NAME"  $(CURDIR)/../../../../../output/.config | sed 's/CONFIG_BSP_NAME="//g' | sed 's/"//g')
CHIP_ID = $(shell grep "CONFIG_SOC_NAME"  $(CURDIR)/../../../../../output/.config | sed 's/CONFIG_SOC_NAME="//g' | sed 's/"//g')

EXTRA_SRCVPATH := $(CURDIR)/../../../../../bsp/$(CHIP_ID)/$(BSP_DIR)
EXTRA_SRCVPATH += $(CURDIR)/../../../src/common/io-mw/qnx
EXTRA_SRCVPATH += $(CURDIR)/../../../src/common/rtsl
EXTRA_SRCVPATH += $(CURDIR)/../../../src/common/csl
EXTRA_SRCVPATH += $(CURDIR)/../../../src/$(AMBA_CHIP_ID)/csl
EXTRA_SRCVPATH += $(CURDIR)/../../../src/$(AMBA_CHIP_ID)/rtsl
EXTRA_SRCVPATH += $(CURDIR)/../../../src/$(AMBA_CHIP_ID)/io-mw/qnx


# src
SRCS := AmbaRTSL_SD_Ctrl.c
SRCS += AmbaSD_DelayCtrl.c
SRCS += AmbaCSL_SD.c
SRCS += bsp.c

LIST=CPU
include recurse.mk
