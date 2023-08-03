EXTRA_SRCVPATH := $(CURDIR)/../../../src/$(AMBA_CHIP_ID)/can/qnx/
#EXTRA_SRCVPATH := $(CURDIR)/../../../src/$(AMBA_CHIP_ID)/rtsl
EXTRA_SRCVPATH += $(CURDIR)/../../../src/common/rtsl
EXTRA_SRCVPATH += $(CURDIR)/../../../src/common/csl

# src
SRCS := main.c
SRCS += lib.c
SRCS += AmbaCSL_CAN.c
SRCS += AmbaRTSL_CAN.c
SRCS += wrap.c
#SRCS += AmbaRTSL_PLL_GetClkFreq.c

LIST=CPU
include recurse.mk

