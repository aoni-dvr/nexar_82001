EXTRA_SRCVPATH := $(CURDIR)/../../../src/common/rtc/qnx/
EXTRA_SRCVPATH += $(CURDIR)/../../../src/$(AMBA_CHIP_ID)/rtsl
EXTRA_SRCVPATH += $(CURDIR)/../../../src/$(AMBA_CHIP_ID)/csl
EXTRA_SRCVPATH += $(CURDIR)/../../../src/common/rtsl
EXTRA_SRCVPATH += $(CURDIR)/../../../src/common/csl

# src
SRCS := main.c
SRCS += lib.c
ifneq ("$(AMBA_CHIP_ID)","cv2fs") 
	SRCS += AmbaCSL_PWC.c
	SRCS += AmbaRTSL_PWC.c
endif
SRCS += AmbaCSL_RTC.c
SRCS += AmbaRTSL_RTC.c

LIST=CPU
include recurse.mk

