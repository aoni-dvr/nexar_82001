EXTRA_SRCVPATH := $(CURDIR)/../../../src/common/spi/qnx/
EXTRA_SRCVPATH += $(CURDIR)/../../../src/common/rtsl/
EXTRA_SRCVPATH += $(CURDIR)/../../../src/common/csl/

# src
SRCS := main.c
SRCS += AmbaCSL_SPI.c
SRCS += AmbaRTSL_SPI.c
SRCS += intr.c
SRCS += lib.c
SRCS += wait.c

LIST=CPU
ifndef QRECURSE
QRECURSE=recurse.mk
ifdef QCONFIG
QRDIR=$(dir $(QCONFIG))
endif
endif
include $(QRDIR)$(QRECURSE)
