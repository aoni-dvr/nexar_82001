EXTRA_SRCVPATH := $(CURDIR)/../../../../src/$(AMBA_CHIP_ID)/devc/qnx/ambarella/
EXTRA_SRCVPATH += $(CURDIR)/../../../../src/common/rtsl/
EXTRA_SRCVPATH += $(CURDIR)/../../../../src/common/csl/

# src
SRCS := main.c
SRCS += AmbaCSL_UART.c
SRCS += AmbaRTSL_UART.c
SRCS += init.c
SRCS += intr.c
SRCS += options.c
SRCS += pulse.c
SRCS += tedit.c
SRCS += tto.c

LIST=CPU
include recurse.mk
