include $(objtree)/include/config/auto.conf

EXTRA_SRCVPATH := $(CURDIR)/../../../src/$(AMBA_CHIP_ID)/cehu/qnx/
EXTRA_SRCVPATH += $(CURDIR)/../../../src/$(AMBA_CHIP_ID)/rtsl
EXTRA_SRCVPATH += $(CURDIR)/../../../src/$(AMBA_CHIP_ID)/csl
EXTRA_SRCVPATH += $(CURDIR)/../../../src/common/csl

# src
SRCS := main.c
SRCS += lib.c
SRCS += AmbaRTSL_CEHU.c
SRCS += AmbaCSL_CEHU.c

LIST=CPU
include recurse.mk

