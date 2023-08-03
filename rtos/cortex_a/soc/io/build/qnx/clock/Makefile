EXTRA_SRCVPATH := $(CURDIR)/../../../src/$(AMBA_CHIP_ID)/clock/qnx/
EXTRA_SRCVPATH += $(CURDIR)/../../../src/$(AMBA_CHIP_ID)/rtsl/
EXTRA_SRCVPATH += $(CURDIR)/../../../src/$(AMBA_CHIP_ID)/csl

# src
SRCS := main.c
SRCS += AmbaCSL_PLL.c
SRCS += AmbaCSL_RCT.c
SRCS += AmbaRTSL_ORC.c
SRCS += AmbaRTSL_PLL.c
SRCS += AmbaRTSL_PLL_Config.c
SRCS += AmbaRTSL_PLL_GetClkFreq.c
SRCS += AmbaRTSL_PLL_SetClkFreq.c
SRCS += AmbaRTSL_PWC.c
SRCS += lib.c

LIST=CPU
include recurse.mk

