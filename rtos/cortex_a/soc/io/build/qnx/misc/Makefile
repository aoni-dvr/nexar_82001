EXTRA_SRCVPATH := $(CURDIR)/../../../src/$(AMBA_CHIP_ID)/misc/qnx/
EXTRA_SRCVPATH += $(CURDIR)/../../../src/common/rtsl
EXTRA_SRCVPATH += $(CURDIR)/../../../src/common/csl

SRCS := lib.o
SRCS += main.o
SRCS +=	AmbaCSL_USB.o
SRCS += AmbaCSL_USB_PHY.o

LIST=CPU
include recurse.mk

